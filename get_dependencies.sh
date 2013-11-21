#!/bin/bash

set -e

DIR=`dirname $0`
ABSDIR=`cd $DIR;pwd`
DEPSRC=$ABSDIR/dependencies_source

# $1 = path name
# $2 = git url
# $3 = git commit sha1
checkout() {
	echo "...git -> $1 ($2:$3)"
	if [ ! -d $DEPSRC/$1 ]; then
		git clone $2 --no-checkout $DEPSRC/$1 > /dev/null
	fi
	
	(cd $DEPSRC/$1 && git checkout $3)
	
	return 0;
}

if [ "$1" == "rebuild" ]; then
	rm -rf $DEPSRC
fi

mkdir -p $DEPSRC

checkout	zlib		git@github.com:dozeo/zlib.git			b1827f54df6c61b8c0c21ce3fe7994e7ca0ae8a1
checkout	ffmpeg 		git@github.com:dozeo/ffmpeg.git			e3f384b428351b62c83a19a8b77d93ffd415a124
checkout	polarssl 	git@github.com:polarssl/polarssl.git	1492633e54afdb4cffa5294319b1707cf631e74a
checkout	rtmpdump 	git://git.ffmpeg.org/rtmpdump			a9f353c7ccf29d6305e13fedb77653b8681e9fc2
checkout	x264 		git@github.com:dozeo/x264.git			1ca7bb943d15165a4b9631002a655587d923be63
