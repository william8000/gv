#!/bin/bash
# Passes command line options to configure.
# Useful options:
# CFLAGS=-O2 # optimize and no -g for distribution
# --with-default-papersize=Letter # change default from A4 to Letter

cd ..
git clean -dfx
cd gv
PATH=/opt/autotools/bin:${PATH} autoreconf -vi
./configure "$@"
make
make dist
