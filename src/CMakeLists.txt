# include_directories(
#   ${CMAKE_CURRENT_SOURCE_DIR}/../taglib
#   ${CMAKE_CURRENT_SOURCE_DIR}/../taglib/toolkit
#   ${CMAKE_CURRENT_SOURCE_DIR}/../taglib/ape
#   ${CMAKE_CURRENT_SOURCE_DIR}/../taglib/mpeg
#   ${CMAKE_CURRENT_SOURCE_DIR}/../taglib/mpeg/id3v1
#   ${CMAKE_CURRENT_SOURCE_DIR}/../taglib/mpeg/id3v2
#   ${CMAKE_CURRENT_SOURCE_DIR}/../bindings/c/
# )

if(NOT BUILD_SHARED_LIBS)
  add_definitions(-DTAGLIB_STATIC)
endif()

add_executable(tagreader tagreader.cpp)
target_link_libraries(tagreader tag)
