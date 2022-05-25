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
		DOWNLOAD_NO_EXTRACT false
	)
	FetchContent_MakeAvailable(download) 
	
	# file(ARCHIVE_EXTRACT INPUT ${target}/${file_name} DESTINATION ${target})
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
GitClone(https://github.com/ocornut/imgui eda7792b151d138e15df951578253b821ceed5a3 ${CMAKE_CURRENT_SOURCE_DIR}/deps/imgui imgui)
GitClone(https://git.sadblog.xyz/nekowin 822bfa6fa71271cb29b6e8d3053fc1c3ed87962e ${CMAKE_CURRENT_SOURCE_DIR}/deps/nekowin nekowin)
GitClone(https://git.sadblog.xyz/libdas a0405fcf1e462e9490f1133fce106beb97f93599 ${CMAKE_CURRENT_SOURCE_DIR}/deps/libdas libdas)

# Windows dependencies
if(WIN32)
	set(TRUNK_NAME win32_deps_a0.0.1.zip)
	DownloadAndExtract(
		https://trunk.sadblog.xyz/${TRUNK_NAME}
		b1e6eff83530a37a2500acecea050974a8d756c71fca151119cfefaef2b45bf4
		${TRUNK_NAME}
		${CMAKE_CURRENT_SOURCE_DIR}/deps/trunk
	)
endif()