#!/bin/bash

set -e

ABSDIR=`(cd $(dirname $0); pwd)`
DEPSRC=$ABSDIR/dependencies_source

echo "Dependencies will be checkout in '$DEPSRC'"

# $1 = path name
# $2 = git url
# $3 = git commit sha1
checkout() {
	echo "...git -> $1 ($2:$3)"
	if [ ! -d $DEPSRC/$1 ]; then
		git clone $2 --no-checkout $DEPSRC/$1
	fi
	
	# check out the correct SHA but also get rid of all build artifacts that the project might have
	# because we might want to rebuild it
	(cd $DEPSRC/$1 && git checkout $3 && git clean -dffx && git checkout -- .) || return 1;
	
	return 0;
}

if [ "$1" == "rebuild" ]; then
	rm -rf $DEPSRC
fi

mkdir -p $DEPSRC

(cd $ABSDIR && git submodule update --init)

checkout zlib git@github.com:dozeo/zlib.git b1827f54df6c61b8c0c21ce3fe7994e7ca0ae8a1
checkout polarssl git@github.com:polarssl/polarssl.git 1492633e54afdb4cffa5294319b1707cf631e74a
checkout rtmpdump git@github.com:justahero/rtmpdump.git 2df708d7a1156e8258c83558e81c4ee50cd26ac5
checkout x264 git@github.com:dozeo/x264.git 1ca7bb943d15165a4b9631002a655587d923be63
checkout ffmpeg git@github.com:dozeo/ffmpeg.git e3f384b428351b62c83a19a8b77d93ffd415a124
