# DENG

![logo](logo/logo_full.png)  
DENG (deadly game engine) is an early-stage rendering engine for Windows. The primary goal of this project, is to create a 
suitable environment for 3D game development with many features such as Python scripting, game editor, physics support, 
audio support and networking. 

## Building

Make sure that Vulkan SDK is installed with all necessary Vulkan capable drivers. You can check this by running vulkaninfo.exe and 
vkcube.exe applications that are provided with the SDK.

In order to build DENG you will need Visual Studio with Microsoft Visual C++ toolchain (tested on VS2019) and Python or pypy.  

Clone the repository and init/update all submodules:
```
git clone https://git.dengproject.org/deng/deng
cd deng
git submodule init
git submodule update
```

Download vcpkg and third party dependencies:  
```
py.exe vcpkg-init.py
```

Generate Visual Studio solution files using CMake
```
mkdir build
cd Build
cmake ..
```