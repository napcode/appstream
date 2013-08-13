appstream
=========

An icecast-based line-in streaming tool intended for small (web-)radio stations. It runs on Linux, MacOS and Windows and supports mp3 & vorbis recording.  
[Edcast](http://www.oddsock.org/) and [butt (broadcast using this tool)](http://sourceforge.net/projects/butt/?source=directory) are similar tools but appStream tries to improve on these things without getting complex.
![Screenshot](https://chili.apparatus.de/attachments/download/4/snapshot1.png)
The project website can be found [here](https://chili.apparatus.de/projects/appstream). 

Building
--------

Appstream uses qmake as build solution. Hence, building should be quite easy on Linux provided that the necessary prerequesites are met:
`qmake` followed by `make`

For windows users there is a VS-project file included. All dependencies need to be placed in a special folder hierarchy to use the project file:
* ./dep/icecast
* ./dep/lame
* ./dep/libogg
* ./dep/libshout
* ./dep/libvorbis
* ./dep/portaudio
* ./dep/pthreads

Building on Windows can be quite tedious. A ready-to-use binary can be found [here](https://chili.apparatus.de/attachments/download/6/appstream_090_w32.zip). 

Dependencies
------------

Most things should be already there on Linux but here's a list:
- Portaudio
- MP3/lame
- shoutcast (libshout)
- libogg, libvorbis
- Pthreads
- QT4

