#!/bin/bash

set -e

DIR=`dirname $0`
CURDIR=`cd $DIR; pwd`

cd $CURDIR

if [ "$1" == "clean" ] || [ "$1" == "rebuild" ]; then
	echo "Cleaning build/ and install/"
	rm -rf build/
	rm -rf install/
fi

if [ "$1" == "clean" ]; then
	echo "Clean only build... exiting after clean process"
	exit 0
fi

mkdir -p build/
cd build/
cmake .. -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=$CURDIR/install
nmake install