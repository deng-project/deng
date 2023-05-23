import os
import subprocess

VCPKG_PATH = "./ThirdParty/vcpkg"
VCPKG_PACKAGES = [
    "entt:x64-windows",
    "shaderc:x64-windows",
    "sdl2[vulkan]:x64-windows",
    "bullet3:x64-windows",
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
