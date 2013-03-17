#!/bin/bash

cd ..
git clean -dfx
cd gv
PATH=/opt/autotools/bin:${PATH} autoreconf -vi
./configure
make
make dist
