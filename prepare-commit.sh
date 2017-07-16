#!/usr/bin/env bash

RELATIVE_FLIP_SOURCE_PATH=`dirname ${BASH_SOURCE[@]}`
FLIP_SOURCE_PATH=`realpath ${RELATIVE_FLIP_SOURCE_PATH}`

echo '::: format all source files'
${FLIP_SOURCE_PATH}/clang-format.sh

# Removing of trailing spaces
echo '::: removing trailing white spaces from all markdown files'
sed --in-place 's/[[:space:]]\+$//' `find ${FLIP_SOURCE_PATH} -type f -name '*.md'`

# Reformat all markdown files to markdown_github
echo '::: reformat all markdown files to markdown_github'
for filename in `find ${FLIP_SOURCE_PATH} -type f -name '*.md'`; do

    pandoc \
        -s \
        --smart \
        -f markdown_github \
        -t markdown_github \
        -o ${filename}.tmp \
        ${filename}

    mv -f ${filename}.tmp ${filename}

    echo ${filename}

done

echo '::: fix all git submodules reformatting'
git submodule foreach git reset --hard
