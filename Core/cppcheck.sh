#!/bin/bash

cppcheck --xml -I ./src/headers  -i ./test/gtest-1.7.0 -i ./build --enable=all --max-configs=25 --force ./ 2>cppcheck.xml || exit $?
cppcheck-htmlreport --file=cppcheck.xml --title=Susi-Core --report-dir=cppcheck_reports --source-dir=. || exit $?
google-chrome cppcheck_reports/index.html

exit $?

