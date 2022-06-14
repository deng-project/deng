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
GitClone(https://github.com/ocornut/imgui e900ca355e3c7be7672e25800af346f1179e91d8 ${CMAKE_CURRENT_SOURCE_DIR}/deps/imgui imgui)
GitClone(https://git.sadblog.xyz/nekowin ad035e81ef8dc90540ffb38e9350b401fbefdcd9 ${CMAKE_CURRENT_SOURCE_DIR}/deps/nekowin nekowin)
GitClone(https://git.sadblog.xyz/libdas f5929b8d2b4ecb1e4bf8b1cf168745c49ea22092 ${CMAKE_CURRENT_SOURCE_DIR}/deps/libdas libdas)

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
        49ec949f67f00988e95ff46def6a049718327de210b335183598c376a24855d8
        ${TRUNK_NAME}
        ${CMAKE_CURRENT_SOURCE_DIR}/deps/trunk
    )
endif()
