#!/bin/bash
# Passes command line options to configure.
# Useful options:
# CFLAGS=-O2 # optimize and no -g for distribution
# --dist # first to make distribution in gv-#.#.# subdirectory
# --no-dist # first to suppress making distribution [default]
# --no-clean # do not run git clean
# --pull # do git pull, then restart the script
# --with-default-papersize=Letter # change default from A4 to Letter

dir=$(git rev-parse --show-toplevel)
if [ -z "$dir" ] ; then echo "$0: Error: not inside gv git" ; fi
cd "$dir" || exit
if [ ! -d "gv" ] || [ ! -d "scripts" ] ; then echo "$0: Error: gv directories not found in $dir" ; fi
clean=yes
dist=no
opt=yes
debug=no
pull=no
opts=()
while [ -n "$1" ] ; do
  case "$1" in
  --dist) dist=yes ; opt=yes ; debug=no ;;
  --no-dist) dist=no ;;
  --clean) clean=yes ;;
  --no-clean) clean=no ;;
  --opt) opt=yes ;;
  --no-opt) opt=no ;;
  --debug) debug=yes ;;
  --no-debug) debug=no ;;
  --pull) pull=yes ;;
  --no-pull) pull=no ;;
  *) break ;;
  esac
  opts+=("$1")
  shift
done
# bash reads a script as it runs, so restart after a pull that may have changed this file
if [ "$pull" = yes ] ; then
  if git pull --ff-only ; then
    exec "$dir/scripts/makedist.sh" "${opts[@]}" --no-pull "$@"
  fi
  echo "$0: Warning: git pull failed"
fi
if [ "$clean" = yes ] ; then git clean -dfx || echo "$0: Warning: git clean failed" ; fi
if [ "$opt" = yes ] ; then export CFLAGS="$CFLAGS -O2" ; fi
if [ "$debug" = yes ] ; then export CFLAGS="$CFLAGS -g" ; fi
cd gv || { echo "$0: Error: cd gv failed" ; exit 1 ; }
defpap=
if [[ "$LANG" =~ ^en_US ]] && ! [[ "$*" =~ "default-papersize" ]] ; then defpap="--with-default-papersize=Letter" ; fi
PATH=/opt/autotools/bin:${PATH} autoreconf -vi
./configure "$defpap" "$@"
np=$(nproc)
if [ -z "$np" ] ; then np=1 ; fi
make -j "$np"
ls -l src/gv
if [ "$dist" = "yes" ] ; then make dist ; fi
