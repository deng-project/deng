# DENG

![logo](logo/logo_full.png)  
DENG is a small and portable game engine designed to work on GNU/Linux and Windows operating systems.
It supports both Vulkan and OpenGL backends and provides an easy renderer api interface with numerous 
components that are considered to be useful for game development.  

The goal of the current animations branch is following:  
* Model loading from DAS format  
* Animation loading  
* ImGui integration  
* Project management  
* Easy to use Lua scripting environment  
* Integration to CMake build system  

## Building

DENG uses CMake as its build environment, so make sure that CMake as well as Vulkan SDK is installed on your system.

On Debian based distributions you can install following packages that are required for building DENG  
```
$ sudo apt install cmake libx11-6 libx11-dev libxcursor1 libxcursor-dev vulkan-validationlayers libglvnd0 libglvnd-dev
```

Clone the repository and all of its submodules with following command:  
```
$ git clone --recursive https://github.com/inugami-dev64/deng
```

Generate build files CMake
```
$ mkdir build
$ cd build
$ cmake <'-GUnix Makefiles'|'-GVisual Studio 17 2022'> -DCMAKE_BUILD_TYPE=<Release|Debug> -DBUILD_DEPS=ON ..
```

Build DENG using specified toolchain.

## Usage
Refer to [USAGE.md](USAGE.md) for more information.
