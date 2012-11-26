#ifndef RINGBUFFER_H
#define RINGBUFFER_H

#include <cstring>
#include <cassert>
#include <iostream>
#include <stdlib.h>

template<typename T>
class RingBuffer
{
public:
    RingBuffer();
    ~RingBuffer();
    bool init(unsigned int size);

    bool isFull() const;
    bool isEmpty() const;
    inline unsigned int getSize() const { return _size; }
    void setSize(unsigned int size); 

    inline unsigned int getSpace() const { return _space; }
    inline unsigned int getFillLevel() const { return _size - _space; }

    unsigned int read(T* dest, unsigned int num = 0);
	unsigned int write(const T* src, unsigned int num, bool partialWrite = false);
	unsigned int write(const void* src, unsigned int num, bool partialWrite = false);

private:
	T *_buffer;	
	unsigned int _size;
	unsigned int _space;
	T *_rptr;
	T *_wptr;
};

template<typename T>
RingBuffer<T>::RingBuffer()
: 	_buffer(0),
	_size(0),
	_space(0),
	_rptr(0),
	_wptr(0)	
{
}
template<typename T>
RingBuffer<T>::~RingBuffer()
{
	delete[] _buffer;
}

template<typename T>
bool RingBuffer<T>::init(unsigned int size)
{
	if(_buffer) {
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
template<typename T>
void RingBuffer<T>::setSize(unsigned int size)
{
	init(size);
}
template<typename T>
bool RingBuffer<T>::isEmpty() const
{
	if (_space == _size)
		return true;
	else 
		return false;
}
template<typename T>
bool RingBuffer<T>::isFull() const
{
	if (_space == 0)
		return true;
	else 
		return false;
}

template<typename T>
unsigned int RingBuffer<T>::read(T* dest, unsigned int num)
{
	// are the buffers valid?
	if(!dest || !_buffer)
		return 0;

	// can we actually fulfill the request?
	if(isEmpty() || (num > getFillLevel()))
		return 0;

	T* endptr = _buffer + (_size * sizeof(T));
	unsigned int numBytes;
	unsigned int numElements;
	// read all 
	if (num == 0) 
		numElements = getFillLevel();
	// read number of elements
	else
		numElements = num;

	numBytes = numElements * sizeof(T);

	// content is between begin & end
	if(_rptr + numBytes < endptr) {
		memcpy(dest, _rptr, numBytes);
		_rptr += numBytes;
	}
	// content wraps around
	else {
		unsigned int from_rptr = endptr - _rptr;
		unsigned int from_begin = numBytes - from_rptr; 
		memcpy(dest, _rptr, from_rptr);
		dest += from_rptr;
		memcpy(dest, _buffer, from_begin);
		_rptr = _buffer + from_begin;
	}
	
	_space += numElements;
	assert(_space <= _size);
	return numElements;
}

template<typename T>
unsigned int RingBuffer<T>::write(const void* src, unsigned int num, bool partialWrite)
{
	return write(static_cast<const T*>(src),num,partialWrite);
}

template<typename T>
unsigned int RingBuffer<T>::write(const T* src, unsigned int num, bool partialWrite)
{
	// are the buffers valid?
	if(!src || !_buffer)
		return 0;

	// can we actually fulfill the request?
	if(isFull())
		return 0;
	if(!partialWrite && (num > getSpace()))
		return 0;

	T* endptr = _buffer + (_size * sizeof(T));
	unsigned int numBytes;
	unsigned int numElements = num;
	// read all 
	if (partialWrite) {
		if (num <= getSpace()) 
			numElements = num;
		else
			numElements = getSpace();
	}
	// read number of elements
	else
		numElements = num;

	numBytes = numElements * sizeof(T);
    /*
    std::cout << "_buffer" << _buffer << std::endl;
    std::cout << "_wptr" << _wptr << std::endl;
    std::cout << "endptr" << endptr << std::endl;
    std::cout << "numElements" << numElements << std::endl;
    std::cout << "numBytes" << numBytes << std::endl;
    */

	if(_wptr + numBytes < endptr) {
		memcpy(_wptr, src, numBytes);
		_wptr += numElements;
	} 
	else {
		unsigned int from_wptr = endptr - _wptr;
		unsigned int from_begin = numBytes - from_wptr;
		memcpy(_wptr, src, from_wptr);
		src += from_wptr/sizeof(T);
		memcpy(_buffer, src, from_begin);
		_wptr = _buffer + from_begin;
	}

	_space -= numElements;
	return numElements;
}

#endif // RINGBUFFER_H
