cmake_minimum_required(VERSION 3.13)
project(downloadTool)

set(CMAKE_CXX_STANDARD 11)
link_directories(/usr/local/lib)
link_libraries(curl pthread)
add_executable(downloadTool main.cpp Download.h HttpDownload.h Download.cpp HttpDownload.cpp)