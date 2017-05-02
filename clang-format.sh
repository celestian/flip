#!/usr/bin/env bash

RELATIVE_FLIP_SOURCE_PATH=`dirname ${BASH_SOURCE[@]}`
FLIP_SOURCE_PATH=`realpath ${RELATIVE_FLIP_SOURCE_PATH}`

clang-format -i -style=file \
    `find ${FLIP_SOURCE_PATH}/src -type f -name '*.c'` \
    `find ${FLIP_SOURCE_PATH}/src -type f -name '*.h'`