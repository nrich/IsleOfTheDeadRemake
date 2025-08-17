# Isle of the Dead (Remake)

A remake of the 1993 DOS game [Isle of the Dead](https://en.wikipedia.org/wiki/Isle_of_the_Dead_(video_game)) by Rainmaker Software.



## Build Instructions

### MinGW

- Download and run the installer from https://www.msys2.org/
- Install the build dependencies from the shell
``` shell
pacman -S git mingw-w64-x86_64-toolchain make unzip
```
- Build the executable
``` shell
make
```

### MinGW (Linux cross compile)
- Ubuntu / Debian
- Install the build toolchain
``` shell
sudo apt install gcc-mingw-w64-x86-64  g++-mingw-w64-x86-64 make
```

- Build the executable
``` shell
CONFIG_W64=1 make
```

### Linux

- Ubuntu / Debian

- Install the build toolchain
``` shell
sudo apt install build-essential
```
- Build the executable
``` shell
make
```

## Installing game content

The original game files are required to run the game. A copy of the original DOS game is hosted on the Internet Archive.

```
wget https://archive.org/download/msdos_Isle_of_the_Dead_1993/Isle_of_the_Dead_1993.zip
unzip Isle_of_the_Dead_1993.zip
```
