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

### Windows

**NOTE: Building DENG on Windows requires you to build shaderc library from source. Using shaderc provided in Vulkan SDK gives you a really really bad time. Shaderc library building is managed by CMake.**  
**Windows support is still not complete and thus unexpected bugs might occur that would otherwise not be present in GNU/Linux systems!**  

Make sure that Vulkan SDK is installed and all necessary Vulkan capable drivers are installed. You can check this by running vkcube.exe provided in the SDK.
For building DENG you will need Visual Studio with Microsoft Visual C++ toolchain (tested on VS2022), Python, Git and CMake. Following commands assume that you are using Powershell.  

Clone the repository and all of its submodules using following command:
```
PS > git clone --recursive https://github.com/inugami-dev64/deng
```

Generate build files using CMake
```
PS > New-Item build
PS > Set-Location build
PS > cmake '-GVisual Studio 17 2022' -DCMAKE_BUILD_TYPE=<Release|Debug> -DBUILD_DEPS=ON -DVULKAN_SDK_PATH=<path-to-sdk> ..
```

After generating Visual Studio solution, open it and build targets.

### GNU/Linux (Debian)

Download Vulkan SDK tarball and extract it to familiar location.

On Debian based distributions you can install following packages that are required for building DENG  
```
$ sudo apt install cmake libx11-6 libx11-dev libxcursor1 libxcursor-dev vulkan-validationlayers libglvnd0 libglvnd-dev
```

Clone the repository and all of its submodules with following command:  
```
$ git clone --recursive https://github.com/inugami-dev64/deng
```

Generate build files using CMake
```
$ mkdir build
$ cd build
$ cmake -GVisual Studio 17 2022 -DCMAKE_BUILD_TYPE=<Release|Debug> -DBUILD_DEPS=ON -DVULKAN_SDK_PATH=<path-to-sdk> ..
```

Build DENG using specified toolchain.

## Usage
Refer to [USAGE.md](USAGE.md) for more information.
