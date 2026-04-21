cmake_minimum_required(VERSION 3.20)
project(JFLX_TextureHandler LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ── Module path ───────────────────────────────────────────────────────────────
if(WIN32 OR ANDROID)
    list(APPEND CMAKE_MODULE_PATH "F:/Dropbox/Dropbox/CompilerUndScripts/CMake/modules")
elseif(UNIX)
    list(APPEND CMAKE_MODULE_PATH "/home/lr6549/Dropbox/CompilerUndScripts/CMake/Modules")
endif()

include(osSpecificIncludes)
include(osSpecificLibs)

# ── MSVC runtime ─────────────────────────────────────────────────────────────
if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

# ── Static library ────────────────────────────────────────────────────────────
add_library(JFLX_TextureHandler STATIC
        TextureHandler.hpp
        TextureHandler.cpp
)

target_include_directories(JFLX_TextureHandler
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/subLibs
        ${CMAKE_CURRENT_SOURCE_DIR}/classes
)

target_compile_definitions(JFLX_TextureHandler PRIVATE SDL_STATIC)

# use your abstraction instead of hardcoding .lib
target_add_os_includes(JFLX_TextureHandler)
target_add_os_libs(JFLX_TextureHandler)

target_link_platform_deps(JFLX_TextureHandler "SDL3")
target_link_platform_deps(JFLX_TextureHandler "SDL3_image")
target_link_platform_deps(JFLX_TextureHandler "SDL3_mixer")
target_link_platform_deps(JFLX_TextureHandler "SDL3_net")
target_link_platform_deps(JFLX_TextureHandler "SDL3_ttf")
target_link_platform_deps(JFLX_TextureHandler "SDL3_rtf")

message(STATUS "Building static library: JFLX_TextureHandler")