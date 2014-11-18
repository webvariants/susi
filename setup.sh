#!/bin/bash

echo get npm dependencies
npm install
echo got npm dependencies

echo get bower dependencies
bower install
echo got bower dependencies

exit $?
