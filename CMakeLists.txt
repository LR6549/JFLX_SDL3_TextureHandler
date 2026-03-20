cmake_minimum_required(VERSION 3.20)
project(JFLX_SDL3_TextureHandler LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(WIN32)
    message(STATUS "Running on Windows")
    list(APPEND CMAKE_MODULE_PATH "F:/Dropbox/Dropbox/CompilerUndScripts/CMake/modules")
    include(osSpecificLibs)

elseif(UNIX)
    message(STATUS "Running on Linux / Unix")
    list(APPEND CMAKE_MODULE_PATH "/home/lr6549/Dropbox/CompilerUndScripts/CMake/Modules")
    include(osSpecificLibs)
endif()

if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

add_library(${PROJECT_NAME} STATIC
        TextureHandler.hpp
        TextureHandler.cpp
)

target_include_directories(${PROJECT_NAME}
        PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/subLibs
            ${CMAKE_CURRENT_SOURCE_DIR}/classes
)

target_compile_definitions(${PROJECT_NAME} PRIVATE SDL_STATIC)

# ---- Link libraries ----
if(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE
            osSpecificLibs
            SDL3.lib
            SDL3_image.lib
            SDL3_mixer.lib
            SDL3_net.lib
            SDL3_ttf.lib
            SDL3_rtf.lib
            user32
            winmm
            setupapi
            cfgmgr32
            version
            imm32
    )
elseif(UNIX)
    # Add Target Link Libraries
endif()