#ifndef RINGBUFFER_H
#define RINGBUFFER_H

#include <cstring>
#include <cassert>
#include <iostream>
#include <stdlib.h>
#include <stdint.h>

template <typename T>
class RingBuffer {
public:
    RingBuffer();
    ~RingBuffer();
    bool init(uint32_t size);

    bool isFull() const;
    bool isEmpty() const;
    inline uint32_t getSize() const { return _size; }
    void setSize(uint32_t size);

    inline uint32_t getSpace() const { return _space; }
    inline uint32_t getFillLevel() const { return _size - _space; }

    uint32_t read(T* dest, uint32_t destsize, uint32_t num = 0);
    uint32_t write(const T* src, uint32_t num, bool partialWrite = false);
    uint32_t write(const void* src, uint32_t num, bool partialWrite = false);

private:
    T* _buffer;
    uint32_t _size;
    uint32_t _space;
    T* _rptr;
    T* _wptr;
};

template <typename T>
RingBuffer<T>::RingBuffer()
: _buffer(0),
  _size(0),
  _space(0),
  _rptr(0),
  _wptr(0)
{
}
template <typename T>
RingBuffer<T>::~RingBuffer()
{
    delete[] _buffer;
}

template <typename T>
bool RingBuffer<T>::init(uint32_t size)
{
    if (_buffer) {
        delete[] _buffer;
        _buffer = 0;
    }

    _buffer = new T[size];
    if (!_buffer)
        return false;

    _rptr = _buffer;
    _wptr = _buffer;
    _size = size;
    _space = size;
    return true;
}
template <typename T>
void RingBuffer<T>::setSize(uint32_t size)
{
    init(size);
}
template <typename T>
bool RingBuffer<T>::isEmpty() const
{
    if (_space == _size)
        return true;
    else
        return false;
}
template <typename T>
bool RingBuffer<T>::isFull() const
{
    if (_space == 0)
        return true;
    else
        return false;
}

template <typename T>
unsigned int RingBuffer<T>::read(T* dest, uint32_t size, uint32_t num)
{
    // are the buffers valid?
    if (!dest || !_buffer)
        return 0;

    // can we actually fulfill the request?
    if (isEmpty() || (num > getFillLevel()))
        return 0;

    T* endptr = _buffer + _size;
    uint32_t numBytes;
    uint32_t numElements;
    // read all
    if (num == 0)
        numElements = getFillLevel();
    // read number of elements
    else
        numElements = num;

    numElements = (size >= numElements) ? numElements : size;
    numBytes = numElements * sizeof(T);

    //std::cout << "NB: " << numElements << std::endl;
    // content is between begin & end
    if (_rptr + numElements <= endptr) {
        memcpy(dest, _rptr, numBytes);
        _rptr += numElements;
    }
    // content wraps around
    else {
        uint32_t from_rptr = endptr - _rptr;
        uint32_t from_begin = numElements - from_rptr;
        memcpy(dest, _rptr, from_rptr * sizeof(T));
        dest += from_rptr;
        memcpy(dest, _buffer, from_begin * sizeof(T));
        _rptr = _buffer + from_begin;
    }

    _space += numElements;
    assert(_space <= _size);
    return numElements;
}

template <typename T>
uint32_t RingBuffer<T>::write(const void* src, uint32_t num, bool partialWrite)
{
    return write(static_cast<const T*>(src), num, partialWrite);
}

template <typename T>
uint32_t RingBuffer<T>::write(const T* src, uint32_t num, bool partialWrite)
{
    // are the buffers valid?
    assert(src && _buffer);
    uint32_t numBytes;
    uint32_t numElements;

    // can we actually fulfill the request?
    if (isFull())
        return 0;

    if (partialWrite) {
        if (num <= _space)
            numElements = num;
        else
            numElements = _space;
    }
    else {
        if (num <= _space)
            numElements = num;
        else
            return 0;
    }
    T* endptr = _buffer + _size;

    numBytes = numElements * sizeof(T);

    if (_wptr + numElements <= endptr) {
        memcpy(_wptr, src, numBytes);
        _wptr += numElements;
    }
    else {
        uint32_t from_wptr = endptr - _wptr;
        uint32_t from_begin = numElements - from_wptr;
        memcpy(_wptr, src, from_wptr * sizeof(T));
        src += from_wptr;
        memcpy(_buffer, src, from_begin * sizeof(T));
        _wptr = _buffer + from_begin;
    }

    _space -= numElements;
    return numElements;
}

#endif // RINGBUFFER_H
