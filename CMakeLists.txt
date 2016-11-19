cmake_minimum_required(VERSION 2.6)
cmake_policy(SET CMP0011 OLD)

include(CheckCXXCompilerFlag)
include(FindPkgConfig)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

if (CMAKE_CXX_COMPILER MATCHES ".*clang")
  set(CMAKE_COMPILER_IS_CLANGXX 1)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-uninitialized")
endif ()


set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

find_program(CCACHE_EXECUTABLE NAMES ccache)
if (CCACHE_EXECUTABLE)
  message(STATUS "ccache found: will be used for compilation and linkage")
  SET_PROPERTY(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ${CCACHE_EXECUTABLE})
  SET_PROPERTY(GLOBAL PROPERTY RULE_LAUNCH_LINK ${CCACHE_EXECUTABLE})
endif ()

if (UNIX AND NOT APPLE)
  set(LINUX 1)
endif (UNIX AND NOT APPLE)

pkg_check_modules(TAGLIB REQUIRED taglib>=1.6)
set(CMAKE_REQUIRED_INCLUDES "${TAGLIB_INCLUDE_DIRS}")
set(CMAKE_REQUIRED_LIBRARIES "${TAGLIB_LIBRARIES}")
check_cxx_source_compiles("#include <opusfile.h>
    int main() { char *s; TagLib::Ogg::Opus::File opusfile(s); return 0;}" TAGLIB_HAS_OPUS)
set(CMAKE_REQUIRED_INCLUDES)
set(CMAKE_REQUIRED_LIBRARIES)

# Set up definitions and paths
include_directories(${TAGLIB_INCLUDE_DIRS})
link_directories(${TAGLIB_LIBRARY_DIRS})

add_subdirectory(src)
