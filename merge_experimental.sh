#!/bin/bash

FEATURE_BRANCHES="feature/commandline-debugger feature/core feature/cpp-engine feature/php-engine feature/watchdog"

git checkout experimental || exit $?

for feature in $FEATURE_BRANCHES; do
	git merge $feature || exit $?
done

for feature in $FEATURE_BRANCHES; do
        git checkout $feature || exit $?
	git merge experimental || exit $?
done

git push --all

git checkout experimental

exit $?

