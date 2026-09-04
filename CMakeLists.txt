cmake_minimum_required(VERSION 3.22)

project(AniTrack
    VERSION 0.8.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

find_package(Qt6 REQUIRED COMPONENTS
    Core
    Gui
    Widgets
    Sql
    Network
    DBus
    Concurrent
)
qt_standard_project_setup()

add_subdirectory(deps)
add_subdirectory(src)
