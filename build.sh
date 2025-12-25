#!/bin/bash

set -o errexit -o pipefail -o noclobber -o nounset

getopt --test > /dev/null && true
if [[ $? -ne 4 ]]; then
    echo 'I’m sorry, `getopt --test` failed in this environment.'
    exit 1
fi

LONGOPTS=release,test
OPTIONS=rt

PARSED=$(getopt --options=$OPTIONS --longoptions=$LONGOPTS --name "$0" -- "$@") || exit 2

eval set -- "$PARSED"

d=y r=n t=n outFile=-

while true; do
    case "$1" in
        -r|--release)
            r=y
            shift
            ;;
        -t|--test)
            t=y
            shift
            ;;
        --)
            shift
            break
            ;;
        *)
            echo "Programming error"
            exit 3
            ;;
    esac
done

# handle non-option arguments
if [[ $# -ne 0 ]]; then
    echo "Build takes no arguments"
    exit 4
fi

FLAGS=""

if [[ "$r" == y ]]; then
    mkdir -p cmake-build-release
    cd cmake-build-release
    FLAGS="-O2 -DNDEBUG -Wall -Wextra -pedantic -Werror"
else
    mkdir -p cmake-build-debug
    cd cmake-build-debug
    FLAGS="-Og -g -Wall -Wextra -pedantic"
fi

if [[ "$t" == y ]]; then
    cmake -DCMAKE_C_FLAGS="$FLAGS" -DINI_TEST=ON ..
else
    cmake -DCMAKE_C_FLAGS="$FLAGS" ..
fi

make
