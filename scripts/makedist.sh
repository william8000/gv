#!/bin/bash
# Passes command line options to configure.
# Useful options:
# CFLAGS=-O2 # optimize and no -g for distribution
# --dist # first to make distribution in gv-#.#.# subdirectory
# --no-dist # first to suppress making distribution [default]
# --no-clean # do not run git clean
# --with-default-papersize=Letter # change default from A4 to Letter

dir=$(git rev-parse --show-toplevel)
if [ -z "$dir" ] ; then echo "$0: Error: not inside gv git" ; fi
cd "$dir" || exit
if [ ! -d "gv" ] || [ ! -d "scripts" ] ; then echo "$0: Error: gv directories not found in $dir" ; fi
clean=yes
dist=no
while [ -n "$1" ] ; do
  case "$1" in
  --dist) dist="yes" ;;
  --no-dist) dist="no" ;;
  --clean) clean=yes ;;
  --no-clean) clean=no ;;
  *) break ;;
  esac
  shift
done
if [ "$clean" = yes ] ; then git clean -dfx ; fi
cd gv || exit
defpap=
if [[ "$LANG" =~ ^en_US ]] && ! [[ "$*" =~ "default-papersize" ]] ; then defpap="--with-default-papersize=Letter" ; fi
PATH=/opt/autotools/bin:${PATH} autoreconf -vi
./configure "$defpap" "$@"
np=$(nproc)
if [ -z "$np" ] ; then np=1 ; fi
make -j "$np"
ls -l src/gv
if [ "$dist" = "yes" ] ; then make dist ; fi
