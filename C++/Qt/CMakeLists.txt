cmake_minimum_required(VERSION 3.16)
project(HelloEla)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt5 REQUIRED COMPONENTS Widgets)

add_subdirectory(ElaWidgetTools)

add_executable(HelloEla main.cpp)

target_include_directories(HelloEla PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/ElaWidgetTools/ElaWidgetTools
)

target_link_libraries(HelloEla PRIVATE
    Qt5::Widgets
    ElaWidgetTools
)