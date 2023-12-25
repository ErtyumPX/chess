# Chess

An attemp to create a chess environment with C++ and SDL2. The project is still being worked on, check [issues](https://github.com/ErtyumPX/chess/issues) and [milestones](https://github.com/ErtyumPX/chess/milestones) for the process.

## License

This project is licensed under the [GNU GPL-3.0](https://github.com/ErtyumPX/chess/blob/main/LICENSE) license.

## Setup

Project is currently compiled with GNU G++ 13.2.1.

The only third-party dependency is SDL2. Used version of SDL2 in project is 2.28.5, should work as long as is SDL2 and not SDL.

For compiling and linking rules GNU Make 4.4.1 was used. After downloading dependencies, make sure to change the include paths inside the Makefile.

```
# change the paths for '-I' 
CC := g++
CFLAGS := -Wall -std=c++11 -I/usr/include/SDL2           # here
TEST_CFLAGS := -Wall -std=c++11 -Iinclude/SDL2 -Isrc/    # and here
LDFLAGS := -lSDL2 -lSDL2_image
...
```

After fulfilling dependencies, download or clone the project and use Makefile to easily compile:

```
> make all
> make run
```

To run tests and check if everything works:

```
> make test
```

Since SDL2 is a cross-platform media library, output should work on Windows, MacOS and Linux.


## Project

For now, looks like this:

----------

![screenshot-2023-12-26_02:37:12](https://github.com/ErtyumPX/chess/assets/49292808/79e8ca6b-f34f-47a5-bbc9-d6ece3a45670)

----------

Details will be  written soon...
