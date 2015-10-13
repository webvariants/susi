#!/bin/bash

for s in /etc/init.d/susi-*; do sudo $s start; done

exit 0;

