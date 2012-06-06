#!/bin/bash
export DIR=`dirname $0`
cd $DIR                       # Go to scripts living directory
export ABSDIR=`cd .;pwd`
echo "ABSDIR=$ABSDIR"

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

# pkgconfig
# Download it by hand
# Note: 0.25 is the last version which included GObject
# Git version doesn't work, as no autoconf is installed by default
if [ -e $INSTALL_DIR/bin/pkg-config ]; then
  echo "pkg-config seems to already exist"
else
  mkdir -p osx
  cd osx
  if [ -d pkg-config-0.25 ]; then
    echo "Pkg-Config already downloaded"
  else
    curl -fL http://pkgconfig.freedesktop.org/releases/pkg-config-0.25.tar.gz > pkg-config-0.25.tar.gz
    tar -xzf pkg-config-0.25.tar.gz
  fi
  echo "Compiling pkg-config"

  cd pkg-config-0.25
  ./configure --prefix=$INSTALL_DIR
  make -j2
  make install
  cd ../..
fi
export PATH=$PATH:$INSTALL_DIR/bin
export PKG_CONFIG_PATH=$INSTALL_DIR/lib/pkgconfig:/usr/lib/pkgconfig

# yasm
# download by hand
# git version doesn't work, as no autoconf is installed by default
if [ -e $INSTALL_DIR/bin/yasm ]; then
    echo "yasm seems to already exist"
else
    cd osx
    if [ -d yasm-1.2.0 ]; then
     echo "Yasm already downloaded"
    else
     curl -fL http://www.tortall.net/projects/yasm/releases/yasm-1.2.0.tar.gz > yasm-1.2.0.tar.gz
     tar -xzf yasm-1.2.0.tar.gz
    fi

    echo "Compiling yasm"
    cd yasm-1.2.0
    ./configure --prefix=$INSTALL_DIR
    make -j2
    make install
    cd ../../
fi

# rtmpdump
if [ -e $INSTALL_DIR/bin/rtmpdump ]; then
    echo "rtmpdump seems to already exist"
else
    echo "Compiling rtmpdump"
    cd rtmpdump
    make SYS=darwin prefix=$INSTALL_DIR install
    cd ..
fi

# libx264
if [ -e $INSTALL_DIR/bin/x264 ]; then   
    echo "x264 seems to already exist"
else
    echo "Compiling x264"
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
    echo "Compiling ffmpeg"
    cd ffmpeg
    #./configure --enable-shared --enable-gpl --enable-libx264 --prefix=$INSTALL_DIR --extra-ldflags=-L$INSTALL_DIR/lib --extra-cflags=-I$INSTALL_DIR/include --extra-cxxflags=-I$INSTALL_DIR/include --enable-librtmp --cc=clang
    ./configure --enable-shared --enable-gpl --enable-libx264 --disable-everything --enable-encoder=libx264 --enable-muxer=flv --enable-protocol=rtmps --enable-protocol=tcp --enable-protocol=rtp --enable-protocol=rtmp --enable-protocol=file --prefix=$INSTALL_DIR --extra-ldflags=-L$INSTALL_DIR/lib --extra-cflags=-I$INSTALL_DIR/include --extra-cxxflags=-I$INSTALL_DIR/include --enable-librtmp --cc=clang
    make -j2
    make install
    cd ..
fi

# gtest
if [ -d gtest-1.6.0 ]; then
    echo "gtest seems already downloaded"
else
    curl -fL http://googletest.googlecode.com/files/gtest-1.6.0.zip > gtest-1.6.0.zip
    unzip gtest-1.6.0.zip
    rm gtest-1.6.0.zip
fi
