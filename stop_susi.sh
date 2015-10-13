#!/bin/bash

for s in /etc/init.d/susi-*; do sudo $s stop; done

exit 0;

