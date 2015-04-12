Development in progress
=======================

This software does not work yet ! :)

Joy2Jack
=======

joy2jack is a tool to convert joystick events to jack midi events.  It
was developped to use the Wii Rockband Drum USB controller as a
virtual MIDI drum kit (using
[hydrogen](http://www.hydrogen-music.org/hcms/) for example).




Compile requirements
====================
* Jack development library
* make

Runtime requirements
====================
* A joystick that is supported by the linux kernel (it should be
  available as `/dev/input/js[0-9]`)
* A running jack audio server

