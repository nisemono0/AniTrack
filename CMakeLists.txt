cmake_minimum_required(VERSION 3.22)

project(AniTrack
    VERSION 2.1.0
    LANGUAGES CXX
)

# Set tabler-icons version used
set(TABLER_ICONS_VERSION "3.49.0")

# Set anitomy version to git tag or fallback to short commit
execute_process(
    COMMAND git -C "${CMAKE_SOURCE_DIR}/deps/anitomy" describe --tags --exact-match
    OUTPUT_VARIABLE ANITOMY_VERSION
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET
)

if (NOT ANITOMY_VERSION)
    execute_process(
        COMMAND git -C "${CMAKE_SOURCE_DIR}/deps/anitomy" rev-parse --short HEAD
        OUTPUT_VARIABLE ANITOMY_VERSION
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
    )
endif()

if (NOT ANITOMY_VERSION)
    set(ANITOMY_VERSION "unknown")
endif()

# Set anime-relations version to short commit
execute_process(
    COMMAND git -C "${CMAKE_SOURCE_DIR}/deps/anime-relations" rev-parse --short HEAD
    OUTPUT_VARIABLE ANIME_RELATIONS_VERSION
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET
)

if (NOT ANIME_RELATIONS_VERSION)
    set(ANIME_RELATIONS_VERSION "unknown")
endif()

# Set utf8proc version to git tag or fallback to short commit
execute_process(
    COMMAND git -C "${CMAKE_SOURCE_DIR}/deps/utf8proc" describe --tags --exact-match
    OUTPUT_VARIABLE UTF8PROC_VERSION
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET
)

if (NOT UTF8PROC_VERSION)
    execute_process(
        COMMAND git -C "${CMAKE_SOURCE_DIR}/deps/utf8proc" rev-parse --short HEAD
        OUTPUT_VARIABLE UTF8PROC_VERSION
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
    )
endif()

if (NOT UTF8PROC_VERSION)
    set(UTF8PROC_VERSION "unknown")
endif()

# Option to enable WIP features
option(ENABLE_WIP_FEATURES "Enable WIP features" OFF)

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
