#!/bin/sh
# nightly_build.sh -- build gv from git

cd "$(dirname "$0")" || { echo "$0: Error: Initial cd $(dirname "$0") failed" ; exit 1 ; }

rm -f nightly_build.log

(
 echo "Starting nightly_build $* at $(date) in $(pwd)"
 ./scripts/makedist.sh "$@"
) 2>&1 | tee nightly_build.log
