cmake_minimum_required(VERSION 3.15)
project(Sapphire)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/${CMAKE_CXX_COMPILER_ID}-${CMAKE_SYSTEM_PROCESSOR}-${CMAKE_BUILD_TYPE}/")
set(CMAKE_PDB_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY})

set(ROOT_DIR ${CMAKE_SOURCE_DIR})
set(SCRIPT_DIR ${ROOT_DIR}/scripts)
set(CODE_DIR ${ROOT_DIR})
set(THIRD_PARTY_DIR ${ROOT_DIR}/third_party)

#
# Variables
#
if (NOT DEFINED PY_CMD)
    set(PY_CMD "py")
endif()

#
# Options
#

# TODO

#
# Compile definitions
#
if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    message("Debug build!")
    list(APPEND SAPPHIRE_DEFINITIONS "DEBUG")
endif()

if (IMGUI_SUPPORT)
    list(APPEND SAPPHIRE_DEFINITIONS IMGUI_SUPPORT)
endif()

#
# Dependencies
#
add_subdirectory(${THIRD_PARTY_DIR}/SDL)
add_subdirectory(${THIRD_PARTY_DIR}/VulkanMemoryAllocator)

#
# Projects
#
add_subdirectory(${CODE_DIR}/engine)
