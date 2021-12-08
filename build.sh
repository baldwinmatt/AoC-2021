#!/bin/bash

set -eu

ROOT_DIR=$(cd $(dirname $0); pwd)
BUILD_DIR="${ROOT_DIR}/build"
BUILDS_ROOT_DIR="${BUILD_DIR}/private/builds"

BUILD_TYPE=RelWithDebInfo

if [[ $# > 0 ]]; then
    case "${1}" in
        clean)
            echo "Cleaning build output..."
            [ -e "${BUILD_DIR}" ] && rm -rf "${BUILD_DIR}"
            exit 0
            ;;
        debug)
            echo "Runnig Debug build..."
	    BUILD_TYPE=Debug
            ;;
        release)
            echo "Running Release build..."
	    BUILD_TYPE=RelWithDebInfo
            ;;
	    new)
            shift
            if [[ $# = 0 ]]; then
                echo "Missing day suffix"
                exit 1
            fi
            new_day="${ROOT_DIR}/Day$1"
            [ -e "${new_day}" ] && echo "ERROR: Day $1 exists" && exit -1
            echo "Setting up new day in ${new_day}"
            mkdir -p "${new_day}"
            cp "${ROOT_DIR}/template"/* "${new_day}/"
            touch "${ROOT_DIR}/inputs/Day${1}.txt" "${ROOT_DIR}/inputs/Day${1}Sample.txt"
            exit 0
            ;;
        run)
            shift
            if [[ $# = 0 ]]; then
                for f in ${BUILD_DIR}/bin/Day*; do
                    day=$(basename "${f}")
                    echo ${day}
                    ${f} ${ROOT_DIR}/inputs/${day}.txt
                    echo
                done
            else
                ${BUILD_DIR}/bin/Day${1} ${ROOT_DIR}/inputs/Day${1}.txt
            fi
            exit 0
            ;;
        *)
            echo "Build type must be one of:"
            echo "  clean     - Clean build output"
            echo "  release   - builds release, coverage and asan targets"
            echo "  debug     - (default) Disable optimizations and enable debug options"
            echo "  new [num] - Prepare for a new day from an empty template"
            echo "  run (day) - Run the executables, optionally run specific day"
            exit 1
    esac
fi

BUILD_CMD="cmake --build ."
THIS_CMAKE_ARGS=""
THIS_BUILD_DIR="${BUILDS_ROOT_DIR}/${BUILD_TYPE}"
if [[ "$(command -v ninja)" ]]; then
    THIS_CMAKE_ARGS=-GNinja
    BUILD_CMD=ninja
fi

mkdir -p "${THIS_BUILD_DIR}"
cd "${THIS_BUILD_DIR}"

cmake ${THIS_CMAKE_ARGS} "${ROOT_DIR}" -DCMAKE_BUILD_TYPE="${BUILD_TYPE}"
${BUILD_CMD}
cmake --install . --prefix "${BUILD_DIR}"

