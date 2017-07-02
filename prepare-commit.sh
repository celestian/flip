#!/usr/bin/env bash

RELATIVE_FLIP_SOURCE_PATH=`dirname ${BASH_SOURCE[@]}`
FLIP_SOURCE_PATH=`realpath ${RELATIVE_FLIP_SOURCE_PATH}`

${FLIP_SOURCE_PATH}/clang-format.sh
git submodule foreach git reset --hard