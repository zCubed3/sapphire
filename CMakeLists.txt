cmake_minimum_required(VERSION 3.15)
project(Sapphire)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/${CMAKE_CXX_COMPILER_ID}-${CMAKE_SYSTEM_PROCESSOR}-${CMAKE_BUILD_TYPE}/")
set(CMAKE_PDB_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY})

set(ROOT_DIR ${CMAKE_SOURCE_DIR})

set(BRANDING_DIR ${ROOT_DIR}/branding)
set(SCRIPT_DIR ${ROOT_DIR}/scripts)
set(SOURCE_DIR ${ROOT_DIR}/source)
set(THIRD_PARTY_DIR ${ROOT_DIR}/third_party)

set(SHADER_DIR ${SOURCE_DIR}/shaders)
set(ENGINE_DIR ${SOURCE_DIR}/engine)
set(SHADER_LIB_DIR ${SHADER_DIR})

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
add_subdirectory(${THIRD_PARTY_DIR}/glm)
add_subdirectory(${THIRD_PARTY_DIR}/SDL)

#
# Projects
#
add_subdirectory(${SOURCE_DIR})
