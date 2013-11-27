#!/bin/bash

set -e

ABSDIR=`(cd $(dirname $0); pwd)`

if [ "$1" == "clean" ] || [ "$1" == "rebuild" ]; then
	(
		cd $ABSDIR
		echo "Cleaning build/ and install/"
		rm -rf build/
		rm -rf install/
	)
fi

if [ "$1" == "clean" ]; then
	echo "Clean only build... exiting after clean process"
	exit 0
fi

(
	mkdir -p $ABSDIR/build/
	cd $ABSDIR/build/
	../vs10env 'cmake .. -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Release'
	../vs10env 'nmake install'
)