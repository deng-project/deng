# DENG: dynamic engine - powerful 3D game engine
# licence: Apache, see LICENCE file
# file: FetchDependencies.py - Python script to fetch all required dependencies for build
# author: Karl-Mihkel Ott

import os
import sys
import requests
import tarfile
import subprocess
from pathlib import Path

# Version definitions
TARGET_DIR = 'deps'
LUA_TARBALL = 'lua-5.4.4.tar.gz'
IMGUI_COMMIT_HASH = '28eabcb0994be436d5ab3bf62761f35a2a4eea1a'
NEKOWIN_COMMIT_HASH = '0c9380deeda6e1e5a38cb0e16b065f0f3f187272'
LIBDAS_COMMIT_HASH = '4f179a584e8b817f1daae0309d72239873f3a458'
SHADERC_COMMIT_HASH = '96b1dd72a827304817470274a470c4d3b2293451'

# Dependency URLs
LUA_TARBALL_URL = f"https://www.lua.org/ftp/{LUA_TARBALL}"
IMGUI_GIT_URL = 'https://github.com/ocornut/imgui'
NEKOWIN_GIT_URL = 'https://git.sadblog.xyz/nekowin'
LIBDAS_GIT_URL = 'https://git.sadblog.xyz/libdas'
SHADERC_GIT_URL = 'https://github.com/google/shaderc'

# Check if target directory is present and if it isn't create new directory
def MakeTargetDir():
    if(not os.path.isdir(TARGET_DIR)):
        os.mkdir(TARGET_DIR)

# Find git executable to use
def FindGitExecutable():
    search_list = ['git', 'git.exe', 'git.bat']

    devnull = open(os.devnull, 'w')
    for git in search_list:
        try:
            subprocess.call([git, '--version'], stdout=devnull)
        except (OSError,):
            continue
        return git

    raise Exception("Could not find git executable")


GIT_EXECUTABLE = FindGitExecutable()
PYTHON_EXECUTABLE = Path(sys.executable).resolve()


# Fetch and extract lua source code
def FetchLua():
    req = requests.get(LUA_TARBALL_URL, allow_redirects=True)

    with open(f"{TARGET_DIR}/{LUA_TARBALL}", 'wb') as fd:
        for chunk in req.iter_content(4096):
            fd.write(chunk)

    print("Fetched lua tarball, extracting ...")
    
    # Extract contents
    with tarfile.open(f"{TARGET_DIR}/{LUA_TARBALL}", 'r:gz') as tarball:
        tarball.extractall(TARGET_DIR)

    print(f"Extracted {TARGET_DIR}/{LUA_TARBALL}")

    os.remove(f"{TARGET_DIR}/{LUA_TARBALL}")


def CloneByCommitId(repo_url, commit_id):
    repo_name = repo_url[repo_url.rfind('/') + 1:]
    devnull = open(os.devnull, 'w')
    print(f"{repo_name} @ {commit_id}")

    subprocess.call([GIT_EXECUTABLE, 'clone', '--quiet', repo_url, f"{TARGET_DIR}/{repo_name}"], stderr=devnull)
    
    if subprocess.call([GIT_EXECUTABLE, 'checkout', '--quiet', commit_id], cwd=f"{TARGET_DIR}/{repo_name}", stderr=devnull) != 0:
        raise Exception(f"Invalid commit id for repo '{repo_name}'")

    devnull.close()


MakeTargetDir()
FetchLua()
CloneByCommitId(IMGUI_GIT_URL, IMGUI_COMMIT_HASH)
CloneByCommitId(NEKOWIN_GIT_URL, NEKOWIN_COMMIT_HASH)
CloneByCommitId(LIBDAS_GIT_URL, LIBDAS_COMMIT_HASH)
CloneByCommitId(SHADERC_GIT_URL, SHADERC_COMMIT_HASH)
subprocess.call([PYTHON_EXECUTABLE, 'utils/git-sync-deps'], cwd=f"{TARGET_DIR}/shaderc")
