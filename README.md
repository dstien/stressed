# stressed

**Stunts/4D [Sports] Driving resource editor**

Stressed is a resource editor for the PC version of the game "Stunts" (Broderbund), also published as "4D Sports Driving" (Mindscape) and "4D Driving" (Electronic Arts).

Currently only files containing plain text, pixel graphics or 3d shapes are supported. Stunts 1.0 files are not supported.

Stressed runs in all desktop environments where Qt and OpenGL are available.

## Dependencies

* Qt 5.0 (or higher) development tools and libraries
* OpenGL
* cmake and a supported C++ toolchain

## Building

0. `git clone https://github.com/dstien/stressed.git`
1. `cd stressed`
2. `mkdir build`
3. `cd build`
4. `cmake ..`
5. `make`
6. Run `src/app/stressed`

*TODO: Current Windows build instructions.*

## Usage

Stressed can optionally load a file on startup if a valid path is given as the first non-Qt parameter, allowing the program to be used as the default handler for Stunts related files in a desktop environment.

The [user reference](https://wiki.stunts.hu/wiki/Stressed_user_reference) extensively documents the stressed's capabilities.

## Technical documentation

The resource file format is documented on the [Stunts Wiki](https://wiki.stunts.hu/wiki/Resource_file_format).

## Licence

GNU GPL version 2, see COPYING.

## Contact

* [Daniel Stien](mailto:daniel@stien.org)
* [Daniel Mlot](mailto:duplode_1@yahoo.com.br)
* https://github.com/dstien/stressed
