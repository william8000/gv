#!/bin/bash

cd ..
git clean -dfx
cd gv
PPATH=/opt/autotools/bin:${PATH} autoreconf -vi
./configure
make
make dist
