#!/usr/bin/env bash

clang-format -i -style=file \
    ${PWD}/src/listener/*.c \
    ${PWD}/src/nbus/*.{h,c} \
    ${PWD}/src/url/*.{h,c} \
    ${PWD}/src/utils/*.{h,c}
