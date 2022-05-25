# DENG: dynamic engine - powerful 3D game engine
# licence: Apache, see LICENCE file
# file: FetchDependencies.py - Python script to fetch all required dependencies for build
# author: Karl-Mihkel Ott

import os
import sys
import subprocess
from pathlib import Path

# Version definitions
TARGET_DIR = 'deps'
IMGUI_COMMIT_HASH = 'eda7792b151d138e15df951578253b821ceed5a3'
NEKOWIN_COMMIT_HASH = 'bb93c0ed5a9e007328ed18c59ec613deb04b5094'
LIBDAS_COMMIT_HASH = 'd2d213f5744db13e1d052eff5d4f7464f0fac5f4'
SHADERC_COMMIT_HASH = '96b1dd72a827304817470274a470c4d3b2293451'

# Dependency URLs
IMGUI_GIT_URL = 'https://github.com/ocornut/imgui'
NEKOWIN_GIT_URL = 'https://git.sadblog.xyz/nekowin'
LIBDAS_GIT_URL = 'https://git.sadblog.xyz/libdas'
SHADERC_GIT_URL = 'https://github.com/google/shaderc'
WIN32_TRUNK_FETCH_URL = 'https://trunk.sadblog.xyz/win32/win32_0.0.1.tar.gz'


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


def CloneByCommitId(repo_url, commit_id):
    repo_name = repo_url[repo_url.rfind('/') + 1:]
    devnull = open(os.devnull, 'w')
    print(f"{repo_name} @ {commit_id}")

    subprocess.call([GIT_EXECUTABLE, 'clone', '--quiet', repo_url, f"{TARGET_DIR}/{repo_name}"], stderr=devnull)
    
    if subprocess.call([GIT_EXECUTABLE, 'checkout', '--quiet', commit_id], cwd=f"{TARGET_DIR}/{repo_name}", stderr=devnull) != 0:
        raise Exception(f"Invalid commit id for repo '{repo_name}'")

    devnull.close()


MakeTargetDir()
CloneByCommitId(IMGUI_GIT_URL, IMGUI_COMMIT_HASH)
CloneByCommitId(NEKOWIN_GIT_URL, NEKOWIN_COMMIT_HASH)
CloneByCommitId(LIBDAS_GIT_URL, LIBDAS_COMMIT_HASH)
CloneByCommitId(SHADERC_GIT_URL, SHADERC_COMMIT_HASH)
subprocess.call([PYTHON_EXECUTABLE, 'utils/git-sync-deps'], cwd=f"{TARGET_DIR}/shaderc")
