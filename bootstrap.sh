#!/bin/bash

# Make all necessary dirs
if [ ! -d build-aux ]; then
    mkdir build-aux
fi

if [ ! -d build-aux/m4 ]; then
    mkdir build-aux/m4
fi

# Make all necessary files
if [ ! -e README ]; then
    ln -s README.md README
fi

if [ ! -e COPYING ]; then
    ln -s LICENSE COPYING
fi

# Generate build scripts
autoreconf --install --force --verbose
