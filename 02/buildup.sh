#!/bin/bash
git fetch --all
git reset --hard origin/master
rm -r build
mkdir build
cd build
cmake ..
make ..
