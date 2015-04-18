Joy2Jack
=======

joy2jack is a tool to convert joystick events to jack midi events.  It
was developped to use the Wii Rockband Drum USB controller as a
virtual MIDI drum kit (using
[hydrogen](http://www.hydrogen-music.org/hcms/) or
[Qsynth](http://qsynth.sourceforge.net/qsynth-index.html) for
example).




Compile requirements
====================
* Jack development library
* make

Runtime requirements
====================
* A joystick that is supported by the linux kernel (it should be
  available as `/dev/input/js[0-9]`)
* A running jack audio server

Invocation
==========
The `joy2jack` command is launched using:
    joy2jack <path_to_config_file> [joystick_device_file]

The first parameter is mandatory and is a path to a configuration file that configures
mappings from joystick events to MIDI actions. An example is provided
under `examples/wii_rock_brand_drum_kit.conf`.

The second parameter is optional and can be used to select which
joystick to use. Defaults to `/dev/input/js0`.


