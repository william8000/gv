#!/bin/bash
# Passes command line options to configure.
# Useful options:
# CFLAGS=-O2 # optimize and no -g for distribution
# --no-dist # first to suppress making distribution
# --with-default-papersize=Letter # change default from A4 to Letter

dir=$(git rev-parse --show-toplevel)
if [ -z "$dir" ] ; then echo "$0: Error: not inside gv git" ; fi
cd "$dir" || exit
if [ ! -d "gv" ] || [ ! -d "scripts" ] ; then echo "$0: Error: gv directories not found in $dir" ; fi
git clean -dfx
cd gv || exit
if [ "$1" = "--no-dist" ] ; then dist="no" ; shift ; fi
PATH=/opt/autotools/bin:${PATH} autoreconf -vi
./configure "$@"
np=$(nproc)
if [ -z "$np" ] ; then np=1 ; fi
make -j "$np"
if [ "$dist" != "no" ] ; then make dist ; fi
