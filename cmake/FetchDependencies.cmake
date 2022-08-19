# DENG: dynamic engine - powerful 3D game engine
# licence: Apache, see LICENCE file
# file: FetchDependencies.cmake - CMake configuration file to fetch all required dependencies
# author: Karl-Mihkel Ott

include(FetchContent)
set(CMAKE_TLS_VERIFY true)

# Generic archive download function
function(DownloadAndExtract download_url hash file_name target)
	message("Downloading file from ${download_url} to ${target}")
    set(DOWNLOAD_TARGET "download_${file_name}")
    FetchContent_Declare(${DOWNLOAD_TARGET}
		DOWNLOAD_DIR ${target}
		SOURCE_DIR ${target}
		URL ${download_url}
		URL_HASH SHA256=${hash}
        DOWNLOAD_NO_EXTRACT FALSE
	)
    FetchContent_MakeAvailable(${DOWNLOAD_TARGET}) 
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
GitClone(https://github.com/ocornut/imgui 8cbd391f096b9314a08670052cc0025cbcadb249 ${CMAKE_CURRENT_SOURCE_DIR}/deps/imgui imgui)
GitClone(https://git.dengproject.org/deng/trs-headers 898e58a49f8f8f76dc3b07407b879cb6836ddc93 ${CMAKE_CURRENT_SOURCE_DIR}/deps/trs trs)
GitClone(https://git.dengproject.org/deng/minimal-ascii-reader f5d9b5d0d04da1063e2fab2fd124f6558c37de2c ${CMAKE_CURRENT_SOURCE_DIR}/deps/mar mar)
GitClone(https://git.dengproject.org/deng/nekowin 36d99bf5ece549f95a3051bd658223f65973df5d ${CMAKE_CURRENT_SOURCE_DIR}/deps/nekowin nekowin)
GitClone(https://git.dengproject.org/deng/libdas a6b675fb5ccb7a22ea6f73de155d7df379306748 ${CMAKE_CURRENT_SOURCE_DIR}/deps/libdas libdas)
target_include_directories(das-static
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/deps/mar/include
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/deps/trs/include)

if(WIN32)
    target_include_directories(nwin-static
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/deps/VulkanSDK-${VULKAN_SDK_VERSION}/Include)

    target_link_directories(nwin-static
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/deps/VulkanSDK-${VULKAN_SDK_VERSION}/Lib)
endif()



# Windows dependencies
if(WIN32)
    if(CMAKE_BUILD_TYPE MATCHES Debug)
        DownloadAndExtract(
            https://sdks.dengproject.org/win32/cpython-${PYTHON_VERSION}-db_win32-x86_64.zip
            47907D05E38E57F3DD1DE3C9815AF15B1FA91C79F63B737BD396ABE6D3CD4C0E
            cpython-${PYTHON_VERSION}-db_win32-x86_64.zip
            ${CMAKE_CURRENT_SOURCE_DIR}/deps/cpython-${PYTHON_VERSION}
        )
    else()
        DownloadAndExtract(
            https://sdks.dengproject.org/win32/cpython-${PYTHON_VERSION}_win32-x86_64.zip
            782DDF7147507FA67C4B2A441ACA9E610A43E204981C58B46E1BEAF35F37A844
            cpython-${PYTHON_VERSION}_win32-x86_64.zip
            ${CMAKE_CURRENT_SOURCE_DIR}/deps/cpython-${PYTHON_VERSION}
        )
    endif()

    # google/shaderc
    DownloadAndExtract(
        https://sdks.dengproject.org/win32/shaderc-${SHADERC_VERSION}_win32-x86_64.zip
        F0D8F40B15A781CD48DD88561AE2C8682FD90D5A80443E7A3BC8E1180D1EF87B
        shaderc-${SHADERC_VERSION}_win32-x86_64.zip
        ${CMAKE_CURRENT_SOURCE_DIR}/deps/shaderc-${SHADERC_VERSION}
    )

    DownloadAndExtract(
        https://sdks.dengproject.org/win32/VulkanSDK-${VULKAN_SDK_VERSION}_win32-x86_64.zip
        0B2515FCC8407262D75C882254DD218E79C255C7DEE4E715F2B7BD8D3AA1C235
        VulkanSDK-${VULKAN_SDK_VERSION}_win32-x86_64.zip
        ${CMAKE_CURRENT_SOURCE_DIR}/deps/VulkanSDK-${VULKAN_SDK_VERSION}
    )

    #DownloadAndExtract(
        #https://sdks.dengproject.org/win32/ode-${ODE_VERSION}_win32-x86_64.zip
        #F16A85523615A08B0D4897D9BE30846DC55862506B88935796EF37942AD04D60
        #ode-${ODE_VERSION}_win32-x86_64.zip
        #${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}/ode-${ODE_VERSION}
    #)

    DownloadAndExtract(
        https://sdks.dengproject.org/win32/wxWidgets-3.2.0_win32-x86_64.zip
        DC0E69A6E58001CD5B7A094AFDABEFBE918BF4D732368B99481195D1880A624B
        wxWidgets-${WXWIDGETS_VERSION}_win32-x86_64.zip
        ${CMAKE_CURRENT_SOURCE_DIR}/deps/wxWidgets-${WXWIDGETS_VERSION}
    )
elseif(UNIX AND NOT APPLE)
    # CPython
    if(CMAKE_BUILD_TYPE MATCHES Debug)
        DownloadAndExtract(
            https://sdks.dengproject.org/linux/cpython-${PYTHON_VERSION}-db_gnu-linux-x86_64.tar.gz
            48c1e77d98104da8f5c5f3bd5e2b733a7da3e9acd88ac5d026e3dc167f1ffb1c
            cpython-${PYTHON_VERSION}-db_gnu-linux-x86_64.tar.gz
            ${CMAKE_CURRENT_SOURCE_DIR}/deps/cpython-${PYTHON_VERSION}
        )
    else()
        DownloadAndExtract(
            https://sdks.dengproject.org/linux/cpython-${PYTHON_VERSION}_gnu-linux-x86_64.tar.gz
            eb9ba00ac1e5cbe43eefeb392c5fae6a2f3ef5b535ddb737d47911162a58c6c8
            cpython-${PYTHON_VERSION}_gnu-linux-x86_64.tar.gz
            ${CMAKE_CURRENT_SOURCE_DIR}/deps/cpython-${PYTHON_VERSION}
        )
    endif()

    # ODE
    #DownloadAndExtract(
        #https://sdks.dengproject.org/linux/ode-${ODE_VERSION}_gnu-linux-x86_64.tar.gz
        #e0fd88c3b7ea3ffcc4481c392405ad3d280963aa8190ac7deabecd2e5b8e37d6
        #ode-${ODE_VERSION}_gnu-linux-x86_64.tar.gz
        #${CMAKE_CURRENT_SOURCE_DIR}/ode-${ODE_VERSION}
    #)

    # wxWidgets
    DownloadAndExtract(
        https://sdks.dengproject.org/linux/wxWidgets-${WXWIDGETS_VERSION}_gnu-linux-x86_64.tar.gz
        061eb7d447421ec30a6910887d65a479d70e0de7d9a69b324728d37c6de01e10
        wxWidgets-${WXWIDGETS_VERSION}_gnu-linux-x86_64.tar.gz
        ${CMAKE_CURRENT_SOURCE_DIR}/deps/wxWidgets-${WXWIDGETS_VERSION}
    )
endif()