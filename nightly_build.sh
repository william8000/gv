#!/bin/sh
# nightly_build.sh -- build gv from git

cd "$(dirname "$0")" || { echo "$0: Error: Initial cd $(dirname "$0") failed" ; exit 1 ; }

rm -f nightly_build.log

# handle clean here to avoid removing the log

if [ "$1" != no-clean ] && [ "$1" != "--no-clean" ]
then
	echo "Cleaning..."
	git clean -dfx
fi

(
 echo "Starting nightly_build $* at $(date) in $(pwd)"
 ./scripts/makedist.sh --no-clean "$@"
) 2>&1 | tee nightly_build.log
