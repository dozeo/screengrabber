#!/bin/bash

set -e

export DIR=`dirname $0`
cd $DIR
export ABSDIR=`cd .;pwd`

# $1 = path name
# $2 = git url
# $3 = git commit sha1
checkout() {
	echo "...git -> $1 ($2:$3)"
	if [ ! -d $DEPSRC/$1 ]; then
		git clone $2 --no-checkout $DEPSRC/$1
	fi
	
	(cd $DEPSRC/$1 && git checkout $3 > /dev/null)
	
	return 0;
}

DEPSRC=$ABSDIR/dependencies_source

if [ ! -d $DEPSRC ]; then
	mkdir $DEPSRC
fi

ZLIB_COMMIT=b1827f54df6c61b8c0c21ce3fe7994e7ca0ae8a1
ZLIB_GITURL=git@github.com:dozeo/zlib.git

FFMPEG_COMMIT=e3f384b428351b62c83a19a8b77d93ffd415a124
FFMPEG_GITURL=git@github.com:dozeo/ffmpeg.git

POLARSSL_COMMIT=1492633e54afdb4cffa5294319b1707cf631e74a
POLARSSL_GITURL=git@github.com:polarssl/polarssl.git

RTMPDUMP_COMMIT=a9f353c7ccf29d6305e13fedb77653b8681e9fc2
RTMPDUMP_GITURL=git://git.ffmpeg.org/rtmpdump

X264_COMMIT=1ca7bb943d15165a4b9631002a655587d923be63
X264_GITURL=git@github.com:dozeo/x264.git



checkout zlib $ZLIB_GITURL $ZLIB_COMMIT
checkout ffmpeg $FFMPEG_GITURL $FFMPEG_COMMIT
checkout polarssl $POLARSSL_GITURL $POLARSSL_COMMIT
checkout rtmpdump $RTMPDUMP_GITURL $RTMPDUMP_COMMIT
checkout x264 $X264_GITURL $X264_COMMIT
