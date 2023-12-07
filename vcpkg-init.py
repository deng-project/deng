import os
import sys
import subprocess

VCPKG_PATH = "./ThirdParty/vcpkg"
PLATFORM = ""

if sys.platform.startswith("linux"):
    PLATFORM = "linux";
elif sys.platform.startswith("win32"):
    PLATFORM = "windows"
elif sys.platform.startswith("darwin"):
    PLATFORM = "osx"

VCPKG_PACKAGES = [
    f"zstd:x64-{PLATFORM}",
    f"boost-iostreams:x64-{PLATFORM}",
    f"boost-multiprecision:x64-{PLATFORM}",
    f"entt:x64-{PLATFORM}",
    f"directx-dxc:x64-{PLATFORM}",
    f"sdl2[vulkan]:x64-{PLATFORM}",
    f"bullet3:x64-{PLATFORM}",
    f"glm:x64-{PLATFORM}"
]

def check_and_install_vcpkg():
    # Check if the platform is not windows
    if(os.name != "nt"):
        print("DENG does not support non-windows operating systems :(")
        quit(0);
    else:
        vcpkg_exec = VCPKG_PATH + "/vcpkg.exe"
        if not os.path.isfile(vcpkg_exec):
            cmd = [ "cmd.exe", "/C", os.path.abspath(VCPKG_PATH + "/bootstrap-vcpkg.bat"), "-disableMetric" ]
            p = subprocess.run(cmd)

        cmd = [ os.path.abspath(vcpkg_exec), "install" ]            
        for dep in VCPKG_PACKAGES:
            cmd.append(dep)
        
        p = subprocess.run(cmd)
          
          
check_and_install_vcpkg()
