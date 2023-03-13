cmake_minimum_required(VERSION 3.17)
project(Sapphire)

# TODO: What about platforms that don't support C++20?
set(CMAKE_CXX_STANDARD 20)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/${CMAKE_CXX_COMPILER_ID}-${CMAKE_SYSTEM_PROCESSOR}-${CMAKE_BUILD_TYPE}/")
set(CMAKE_PDB_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY})

set(ROOT_DIR ${CMAKE_SOURCE_DIR})
set(SCRIPT_DIR ${CMAKE_SOURCE_DIR}/scripts)
set(CODE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/sapphire)
set(THIRD_PARTY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/third_party)

#
# Options
#

# TODO: Make Vulkan non-optional
set(RS_VULKAN_SUPPORT ON)

option(SAPPHIRE_AS_EDITOR "Compiles the engine as the editor and not standalone" ON)
option(SAPPHIRE_BUILD_TOOLS "Compiles tool applications" ON)

#
# Compiler things
#
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    message("Compiling with MSVC")

    set(SAPPHIRE_COMPILE_OPTIONS)
    set(SAPPHIRE_DEFINITIONS "_CRT_SECURE_NO_WARNINGS")
endif()

if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    message("Debug build!")
    list(APPEND SAPPHIRE_DEFINITIONS "DEBUG" "SAPPHIRE_DEBUG")
endif()

# Forces GLM depth to be correct
list(APPEND SAPPHIRE_DEFINITIONS "GLM_FORCE_DEPTH_ZERO_TO_ONE")

#
# Projects
#
if (SAPPHIRE_AS_EDITOR)
    list(APPEND SAPPHIRE_DEFINITIONS "SAPPHIRE_EDITOR")
endif()

# Core: Depends on nothing but the OS
add_subdirectory(${CODE_DIR}/core)

# TODO: Null driver to remove SDL dependency?
add_subdirectory(${THIRD_PARTY_DIR}/SDL)

set(IMGUI_SUPPORT ON)
list(APPEND SAPPHIRE_DEFINITIONS "IMGUI_SUPPORT")

if (RS_VULKAN_SUPPORT)
    message("Compiling Sapphire with Vulkan Render Server support!")

    list(APPEND ENGINE_LINK_LIBRARIES SapphireVulkanRS)
    list(APPEND ENGINE_DEFINITIONS RS_VULKAN_SUPPORT)

    add_subdirectory(${CODE_DIR}/rs_vulkan)
endif()

if (COMPILE_TOOLS)
    message("Compile Sapphire tools!")

    set(OLD_BIN_DIR ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/tools)

    add_subdirectory(${CODE_DIR}/tools/obj2sebm)

    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OLD_BIN_DIR})
endif()

add_subdirectory(${CODE_DIR}/engine)
