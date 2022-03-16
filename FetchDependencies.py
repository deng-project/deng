# DENG: dynamic engine - powerful 3D game engine
# licence: Apache, see LICENCE file
# file: FetchDependencies.py - Python script to fetch all required dependencies for build
# author: Karl-Mihkel Ott

import os
import requests
import tarfile

# Version definitions
TARGET_DIR = 'deps'
LUA_TARBALL = 'lua-5.4.4.tar.gz'

# Dependency URLs
LUA_TARBALL_URL = f"https://www.lua.org/ftp/{LUA_TARBALL}"
IMGUI_GIT_URL = 'https://github.com/ocornut/imgui'
NEKOWIN_GIT_URL = 'https://git.sadblog.xyz/nekowin'
LIBDAS_GIT_URL = 'https://git.sadblog.xyz/inugami-dev64/libdas'
SHADERC_GIT_URL = 'https://github.com/google/shaderc'

# Fetch and extract lua source code
def FetchLua():
    req = requests.get(LUA_TARBALL_URL, allow_redirects=True)
    open(f"{TARGET_DIR}/{LUA_TARBALL}", 'wb').write(req.content)
    print("Fetched lua tarball, extracting ...")
    
    # Extract contents
    tarball = tarfile.open(f"{TARGET_DIR}/{LUA_TARBALL}")
    tarball.extractall("{TARGET_DIR}/lua")
    tarball.close()

    os.remove(f"{TARGET_DIR}/{LUA_TARBALL}")


def CloneImGui():

