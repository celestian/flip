#!/usr/bin/env bash

clang-format -i -style=file \
    `find ./src -type f -name '*.c'` \
    `find ./src -type f -name '*.h'`