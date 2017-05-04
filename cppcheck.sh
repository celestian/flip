#!/usr/bin/env bash

RELATIVE_FLIP_SOURCE_PATH=`dirname ${BASH_SOURCE[@]}`
FLIP_SOURCE_PATH=`realpath ${RELATIVE_FLIP_SOURCE_PATH}`

cppcheck \
    --language=c \
    --std=c11 \
    --enable=all \
    --suppress=missingIncludeSystem \
    --check-config \
    -I${FLIP_SOURCE_PATH} \
    -I${FLIP_SOURCE_PATH}/$(uname -m) \
    ${FLIP_SOURCE_PATH}/src
