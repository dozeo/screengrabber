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

$ABSDIR/build_dependencies_win.sh

(
	mkdir -p $ABSDIR/build/
	cd $ABSDIR/build/
	rm -f screengrabber.tar.gz 
	rm -rf install/
	../vs10env 'cmake .. -G "Visual Studio 10" -DCMAKE_BUILD_TYPE=Release'
	../vs10env 'msbuild INSTALL.vcxproj /p:Configuration=Release'
	cd install && tar -c * | gzip - > ../screengrabber.tar.gz
)