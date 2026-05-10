cmake_minimum_required(VERSION 3.10)
project(FTPServer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${CMAKE_SOURCE_DIR}/include)

find_package(Threads REQUIRED)

add_executable(ftp_server 
    src/main.cpp
    src/ftp_server.cpp
)

target_link_libraries(ftp_server 
    Threads::Threads
    stdc++fs
)