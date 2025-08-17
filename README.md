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

- Install raylib
``` shell
sudo apt install build-essential
```

``` shell
make
```
