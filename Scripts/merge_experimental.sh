#!/bin/bash

FEATURE_BRANCHES="feature/commandline-debugger feature/core feature/cpp-engine feature/php-engine feature/watchdog feature/ddhcp feature/scripts"

git fetch --all

git checkout experimental || exit $?
git merge || exit $?

for feature in $FEATURE_BRANCHES; do
    git merge $feature || exit $?
done

for feature in $FEATURE_BRANCHES; do
    git checkout $feature || exit $?
    git merge || exit $?
    git merge experimental || exit $?
done

git push --all

git checkout experimental

exit $?

