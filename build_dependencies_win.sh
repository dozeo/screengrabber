#!/bin/bash

ABSDIR=`(cd $(dirname $0); pwd)`

# Fail on errors
set -e

# define directories
SRC_DIR=$ABSDIR/dependencies_source
INSTALL_DIR=$ABSDIR/dependencies

export PATH=/C/MinGW/bin:$PATH
export PATH=$PATH:$INSTALL_DIR/bin
export PKG_CONFIG_PATH=$PKG_CONFIG_PATH:$INSTALL_DIR/lib/pkgconfig

echo "Fetching all dependencies"
./get_dependencies.sh

# create target directory where to compile tos
echo "creating install dir"
mkdir -p $INSTALL_DIR/bin
mkdir -p $SRC_DIR/win32


echo "SRC_DIR:     $SRC_DIR"
echo "INSTALL_DIR: $INSTALL_DIR"


# yasm
# download by hand
# git version doesn't work, as no autoconf is installed by default
if [ -e $INSTALL_DIR/bin/yasm ]; then
	echo "yasm seems to already exist"
else
	if [ -d $SRC_DIR/yasm-1.2.0 ]; then
		echo "Yasm already downloaded"
	else
		(cd $SRC_DIR && wget --no-check-certificate https://github.com/downloads/dozeo/screengrabber/yasm-1.2.0.tar.gz -O yasm-1.2.0.tar.gz && tar zxvf yasm-1.2.0.tar.gz && rm yasm-1.2.0.tar.gz)
	fi
	
	echo "Compiling yasm ..."

	(
		cd $SRC_DIR/yasm-1.2.0
		./configure --prefix=$INSTALL_DIR
		make install
	)
fi


# ffmpeg requires pkg-config
# =================================================================
if [ -e $INSTALL_DIR/bin/pkg-config.exe ]; then
	echo "pkgconfig seems to already exist"
else
	echo "Fetching pkgconfig"
	
	(
		PKG_FILENAME=pkg-config_0.28-1_bin-win32.zip
		PKG_URL=http://kent.dl.sourceforge.net/project/pkgconfiglite/0.28-1/pkg-config-lite-0.28-1_bin-win32.zip

		cd $INSTALL_DIR/bin/

		wget $PKG_URL -O $PKG_FILENAME
		unzip -o $PKG_FILENAME
		cp -rf pkg-config-lite-0.28-1/* .
		rm -rf pkg-config-lite-0.28-1/ $PKG_FILENAME
	)
fi
# =================================================================


# compile polarssl
# =================================================================
if [ -e $INSTALL_DIR/lib/polarssl.dll ]; then
	echo "polarssl seems to already exist"
else

	(
		cd $SRC_DIR/polarssl
		make SYS=posix DESTDIR=$INSTALL_DIR lib
		make SYS=posix DESTDIR=$INSTALL_DIR install
	)
		
fi
# =================================================================


# rtmpdump
# =================================================================
if [ -e $INSTALL_DIR/bin/rtmpdump ]; then
	echo "rtmpdump seems to already exist"
else
	echo "Compiling rtmpdump ..."
	
	(
		cd $SRC_DIR/rtmpdump
		export LIBZ="-lssl -lcrypto -ldl" 
		make CRYPTO=POLARSSL SYS=mingw prefix=$INSTALL_DIR XCFLAGS=-I$INSTALL_DIR/include XLDFLAGS=-L$INSTALL_DIR/lib install
	)
fi
# =================================================================


# zlib
# =================================================================
ZLIBFILE=libz-1.dll
ZLIBPATH=$INSTALL_DIR/bin/$ZLIBFILE

if [ -e $ZLIBPATH ]; then
	echo "$ZLIBFILE exists"
else
	echo "Compiling $ZLIBFILE"
	
	cd $SRC_DIR/zlib
	
	make -f win32/Makefile.gcc
	cp -rf zlib1.dll $ZLIBPATH
fi
# =================================================================


# gtest (just fetching, will be build via CMake)
# =================================================================
GTESTNAME=gtest-1.6.0
GTESTPATH=$SRC_DIR
GTESTURL=http://googletest.googlecode.com/files/gtest-1.6.0.zip
if [ -d $SRC_DIR/$GTESTNAME ]; then
	echo "gtest seems already downloaded"
else
	(
		cd $SRC_DIR
		wget $GTESTURL -O $GTESTNAME.zip
		unzip $GTESTNAME.zip
		rm $GTESTNAME.zip
	)
fi
# =================================================================


# lib x264 needs to get build!
# =================================================================
if [ -e $INSTALL_DIR/bin/x264 ]; then
	echo "libx264 seems to already exist"
else
	echo "Compiling x264 ..."

	(
		cd $SRC_DIR/x264
		./configure --enable-shared --prefix=$INSTALL_DIR
		#--enable-debug --disable-asm --enable-win32thread
		make install
	)
	#make install
fi
# =================================================================


# enable VSDIR here
# =================================================================
# HACK determine installed Visual Studio, can differ for OS, 32 / 64 Bit
if [ -d "/C/Program Files/Microsoft Visual Studio 9.0/VC" ]; then
	echo "Using Visual Studio 2008"
	export VSDIR="/C/Program Files/Microsoft Visual Studio 9.0"
elif [ -d "/C/Program Files/Microsoft Visual Studio 10.0/VC" ]; then
	echo "Using Visual Studio 2010"
	export VSDIR="/C/Program Files/Microsoft Visual Studio 10.0"
elif [ -d "/C/Program Files (x86)/Microsoft Visual Studio 10.0/VC" ]; then
	echo "Using Visual Studio 2010"
	export VSDIR="/C/Program Files (x86)/Microsoft Visual Studio 10.0"
elif [ -d "/C/Programme/Microsoft Visual Studio 10.0/VC" ]; then
	echo "Using Visual Studio 2010"
	export VSDIR="/C/Programme/Microsoft Visual Studio 10.0"
else
	echo "Fatal: No visual studio found, ffmpeg won't link correctly without lib.exe"
	exit 1
fi
export PATH=$PATH:$PATH:"$VSDIR/VC/bin":"$VSDIR/Common7/IDE"
echo "VSDIR: $VSDIR"
# =================================================================


# ffmpeg
# =================================================================
if [ -e $INSTALL_DIR/bin/ffmpeg.exe ]; then
	echo "ffmpeg seems to already exist"
else
	echo "Compiling ffmpeg ..."
	
	(
		cd $SRC_DIR/ffmpeg

		./configure --prefix=$INSTALL_DIR --enable-shared --enable-libx264 --enable-gpl --enable-librtmp --enable-encoder=libx264 --enable-protocol=rtmp --enable-protocol=rtmps --enable-protocol=rtmpts --enable-protocol=rtmpte --enable-protocol=tcp --enable-protocol=file
		
		make
		make install -k
		git clean -df
		git checkout -- .
	)
	
fi
# =================================================================


# Checks if a given .DLL file is already from $PATH, if not, then copy (from Mingw, which is in $PATH)
# Param $1: dll Name
# Param $2: Human readable name
#function 
CheckDLL() {
	if [ -e $INSTALL_DIR/bin/$1 ]; then
		echo "$2 ($1) already exists."
	else
		echo "Copy $2 ($1)"
		export DLL_PLACE=`which $1`
		cp $DLL_PLACE $INSTALL_DIR/bin
	fi
}

#CheckDLL libz-1.dll "Zlib DLL"
CheckDLL libgcc_s_dw2-1.dll "GCC Support Library"
CheckDLL pthreadGC2.dll "Pthread"
