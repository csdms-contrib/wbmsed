#! /usr/bin/env bash
# Build and install WBMsed on top of an existing GHAAS installation.

if [ ! -d "$GHAASDIR" ]; then
    echo "Error: GHAASDIR environment variable is not set or does not point to a directory."
    exit 1
fi

build_dir=_build

cmake -B $build_dir --fresh -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=$GHAASDIR
cmake --build $build_dir
ctest -V --test-dir $build_dir
cmake --install $build_dir --prefix $GHAASDIR
rm -rf $build_dir

exit 0
