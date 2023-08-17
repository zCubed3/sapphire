cmake_minimum_required(VERSION 3.14)
project(Sapphire)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/${CMAKE_CXX_COMPILER_ID}-${CMAKE_SYSTEM_PROCESSOR}-${CMAKE_BUILD_TYPE}/")
set(CMAKE_PDB_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY})

set(ROOT_DIR ${CMAKE_SOURCE_DIR})
set(SCRIPT_DIR ${CMAKE_SOURCE_DIR}/scripts)
set(CODE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/sapphire)
set(THIRD_PARTY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/third_party)

#
# Variables
#
if (NOT DEFINED PY_CMD)
    set(PY_CMD "py")
endif()

#
# Options
#

#
# Compiler things
#
if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    message("Debug build!")
    list(APPEND SAPPHIRE_DEFINITIONS "DEBUG")
endif()

#
# Projects
#

# Core: Depends on nothing but the OS
add_subdirectory(${CODE_DIR}/core)

# TODO: Null driver to remove SDL dependency?
add_subdirectory(${THIRD_PARTY_DIR}/SDL)
list(APPEND SAPPHIRE_DEFINITIONS "SDL_SUPPORT" "GLM_FORCE_DEPTH_ZERO_TO_ONE")

if (IMGUI_SUPPORT)
    list(APPEND SAPPHIRE_DEFINITIONS IMGUI_SUPPORT)
endif()

if (RS_VULKAN_SUPPORT)
    message("Compiling Sapphire with Vulkan Render Server support!")

    list(APPEND ENGINE_LINK_LIBRARIES SapphireVulkanRS)
    list(APPEND ENGINE_DEFINITIONS RS_VULKAN_SUPPORT)

    add_subdirectory(${CODE_DIR}/rs_vulkan)
endif()

add_subdirectory(${CODE_DIR}/engine)
