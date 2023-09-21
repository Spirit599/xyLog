cmake_minimum_required(VERSION 2.6)

project("xyLog")

set(CMAKE_CXX_STANDARD 17)

set(CXX_FLAGS
 -g
 -ggdb
 -Wall
 -Wextra
 -Werror
 -std=c++17
)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

include_directories(${PROJECT_SOURCE_DIR})


set(SRCS
  Logger.cc
  LogStream.cc
  AsyncLogger.cc
  LogFile.cc
  AppendFile.cc
)

add_library(xyLog ${SRCS})

add_subdirectory(test)