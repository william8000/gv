#!/bin/bash
# Passes command line options to configure.
# Useful options:
# CFLAGS=-O2 # optimize and no -g for distribution
# --with-default-papersize=Letter # change default from A4 to Letter

cd .. || exit
git clean -dfx
cd gv || exit
PATH=/opt/autotools/bin:${PATH} autoreconf -vi
./configure "$@"
np=$(nproc)
if [ -z "$np" ] ; then np=1 ; fi
make -j "$np"
make dist
