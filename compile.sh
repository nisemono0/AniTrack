#!/bin/bash

BUILD_DIR="build"
BUILD_TYPE=""
CLEAN_BUILD=false
WIP_FEATURES=OFF
CLAZY_COMPILE=false
BUILD_ONLY=false

print_help_and_exit() {
    if [ -n "$1" ]; then
        echo "Error: $1"
        echo
        exit 1
    fi

    echo "Usage: $(basename "$0") {--debug, -d | --release, -r} [--clean, -c] [--wip, -w] [--clazy, -l] [--build, -b]"
    echo "Options:"
    echo "  -d, --debug         Build a Debug build"
    echo "  -r, --release       Build a Release build"
    echo "  -c, --clean         Remove the build directory before building"
    echo "  -w, --wip           Enable WIP features"
    echo "  -l, --clazy         Compile with clazy"
    echo "  -b, --build         Build the existing configuration, ignoring all other options"
    echo "  -h, --help          Show this help"
    exit 0
}

[ "$#" -eq 0 ] && print_help_and_exit

while [ "$#" -gt 0 ]; do
    case "$1" in
        "-d" | "--debug")
            if [ -n "$BUILD_TYPE" ]; then
                print_help_and_exit "debug/release can only be specified once"
            fi
            BUILD_TYPE="Debug"
            shift
            ;;
        "-r" | "--release")
            if [ -n "$BUILD_TYPE" ]; then
                print_help_and_exit "debug/release can only be specified once"
            fi
            shift
            BUILD_TYPE="Release"
            ;;
        "-c" | "--clean")
            CLEAN_BUILD=true
            shift
            ;;
        "-w" | "--wip")
            WIP_FEATURES=ON
            shift
            ;;
        "-b" | "--build")
            BUILD_ONLY=true
            shift
            ;;
        "-l" | "--clazy")
            CLAZY_COMPILE=true
            shift
            ;;
        "-h" | "--help") print_help_and_exit ;;
        -*|--*) print_help_and_exit "unknown option: $1" ;;
        *) print_help_and_exit "unexpected argument: $1" ;;
    esac
done

if [ "$BUILD_ONLY" = true ]; then
    [ ! -d "$BUILD_DIR" ] && print_help_and_exit "build directory does not exist"

    echo
    echo "========================================"
    echo " -> Building existing configuration"
    echo "========================================"
    echo

    cmake --build "$BUILD_DIR" --parallel "$(nproc)"
    exit 0
fi

if [ -z "$BUILD_TYPE" ]; then
    print_help_and_exit "--debug or --release must be specified"
fi

if [ "$CLEAN_BUILD" = true ]; then
    echo "Cleaning build directory..."
    rm -rf "$BUILD_DIR"
fi

CMAKE_ARGS=(
    -S .
    -B "$BUILD_DIR"
    -DCMAKE_BUILD_TYPE="$BUILD_TYPE"
    -DENABLE_WIP_FEATURES="$WIP_FEATURES"
)

if [ "$CLAZY_COMPILE" = true ]; then
    CMAKE_ARGS+=(-DCMAKE_CXX_COMPILER=clazy)
fi

echo
echo "========================================"
echo " -> Build type:   $BUILD_TYPE"
echo " -> Clean build:  $CLEAN_BUILD"
echo " -> WIP features: $WIP_FEATURES"
echo " -> Clazy:        $CLAZY_COMPILE"
echo "========================================"
echo

cmake "${CMAKE_ARGS[@]}"

cmake --build "$BUILD_DIR" --parallel "$(nproc)"

