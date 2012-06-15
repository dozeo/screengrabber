#!/bin/bash
export DIR=`dirname $0`
cd $DIR                       # Go to scripts living directory
export ABSDIR=`cd .;pwd`
echo "ABSDIR=$ABSDIR"

# If you want to debug
# export FFMPEG_EXTRA="--enable-debug=2 --disable-stripping --disable-optimizations"
# If you want to get some extra testcases running
# export FFMPEG_EXTRA="$FFMPEG_EXTRA --enable-decoder=h264 --enable-decoder=flv --enable-demuxer=flv"

# Fail on errors
set -e

# Directories
export SRC_DIR=$ABSDIR/dependencies_source
export INSTALL_DIR=$ABSDIR/dependencies
 
# Create target directory
mkdir -p $INSTALL_DIR  # where to compile to

echo "SRC_DIR:     $SRC_DIR"
echo "INSTALL_DIR: $INSTALL_DIR"

cd $SRC_DIR

# rtmpdump
if [ -e $INSTALL_DIR/bin/rtmpdump ]; then
    echo "rtmpdump seems to already exist"
else
    cd rtmpdump
    make prefix=$INSTALL_DIR install
    cd ..
fi
export PKG_CONFIG_PATH=$INSTALL_DIR/lib/pkgconfig

# libx264
if [ -e $INSTALL_DIR/bin/x264 ]; then
    echo "x264 seems to already exist"
else
    cd x264
    ./configure --enable-shared --prefix=$INSTALL_DIR
    make -j2
    make install
    cd ..
fi

# ffmpeg
if [ -e $INSTALL_DIR/bin/ffmpeg ]; then
    echo "ffmpeg seems to already exist"
else
    cd ffmpeg
    if [ "$(uname -m)" = "x86_64" ]; then
    echo "This is an x86_64 Linux"
    export ADDPIC="--enable-pic"
    else
    echo "This seems 32bit Linux"
    export ADDPIC=""
    fi

    ./configure $ADDPIC --enable-shared --enable-gpl --enable-libx264 --disable-everything --enable-encoder=libx264 --enable-muxer=flv --enable-protocol=rtmps --enable-protocol=rtmp --enable-protocol=file --enable-protocol=tcp --enable-protocol=rtp --prefix=$INSTALL_DIR --extra-ldflags=-L$INSTALL_DIR/lib --extra-cflags=-I$INSTALL_DIR/include --extra-cxxflags=-I$INSTALL_DIR/include --enable-librtmp $FFMPEG_EXTRA
    make -j2
    make install
    cd ..
fi

# gtest (just fetching, will be build via CMake)
if [ -d gtest-1.6.0 ]; then
    echo "gtest seems already downloaded"
else
    curl -fL http://googletest.googlecode.com/files/gtest-1.6.0.zip > gtest-1.6.0.zip
    unzip gtest-1.6.0.zip
	rm gtest-1.6.0.zip
fi

