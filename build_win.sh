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
	
	cmake .. -G "Visual Studio 10" -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=install/
	cmake --build . --config Release --target INSTALL

	echo "compressing binaries to screengrabber.tar.gz"
	cd install && tar -c * | gzip - > ../screengrabber.tar.gz
)