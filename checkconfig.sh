#!/bin/bash
#
# Checks to see if the buildroot config file exists and copies a new one across if it doesn't
#

if [ ! -e buildroot/.config ]; then
	cp -Rf --backup=none conf/buildroot.config buildroot/.config
fi
