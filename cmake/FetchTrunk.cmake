# DENG: dynamic engine - powerful 3D game engine
# licence: Apache, see LICENCE file
# file: CMakeFiles.txt - main CMake configuration file
# author: Karl-Mihkel Ott

include(FetchContent)
set(CMAKE_TLS_VERIFY true)

# Generic archive download function
function(DownloadAndExtract download_url hash file_name target)
	message("Downloading file from ${download_url}")
	FetchContent_Declare(download
		DOWNLOAD_DIR ${target}
		SOURCE_DIR ${target}
		URL ${download_url}
		URL_HASH SHA256=${hash}
        DOWNLOAD_NO_EXTRACT FALSE
	)
	FetchContent_MakeAvailable(download) 
endfunction(DownloadAndExtract)


# Git repo cloning function
function(GitClone git_url git_tag target_dir repo_name)
	set(GIT_REPO "git_repo_${repo_name}")
	message("Cloning git repository from ${git_url} to ${target_dir}")
	FetchContent_Declare(${GIT_REPO}
		SOURCE_DIR ${target_dir}
		GIT_REPOSITORY ${git_url}
		GIT_TAG ${git_tag}
	)
	FetchContent_MakeAvailable(${GIT_REPO})
endfunction(GitClone)

# Git clone embedded dependencies
GitClone(https://github.com/ocornut/imgui 250333d895b1067533533dcfab137512745b9689 ${CMAKE_CURRENT_SOURCE_DIR}/deps/imgui imgui)
GitClone(https://git.sadblog.xyz/nekowin 6ed31a06b534777cb67fe099e866b3a3f2821e81 ${CMAKE_CURRENT_SOURCE_DIR}/deps/nekowin nekowin)
GitClone(https://git.sadblog.xyz/libdas 5a9da4704eb7bd3f7416482c0b7aa4748e6aca4f ${CMAKE_CURRENT_SOURCE_DIR}/deps/libdas libdas)

# Windows dependencies
if(WIN32)
	set(TRUNK_NAME win32_deps_a0.0.1.zip)
	DownloadAndExtract(
		https://trunk.sadblog.xyz/${TRUNK_NAME}
		b1e6eff83530a37a2500acecea050974a8d756c71fca151119cfefaef2b45bf4
		${TRUNK_NAME}
		${CMAKE_CURRENT_SOURCE_DIR}/deps/trunk
	)
elseif(UNIX AND NOT APPLE)
    set(TRUNK_NAME linux_deps_a0.0.1.tar.gz)
    DownloadAndExtract(
        https://trunk.sadblog.xyz/${TRUNK_NAME}
        c1a72786111769f4124c2b076423511d8ffe819037e1e8fc71b1c1b22a6850d3
        ${TRUNK_NAME}
        ${CMAKE_CURRENT_SOURCE_DIR}/deps/trunk
    )
endif()
