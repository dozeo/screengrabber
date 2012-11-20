#!/bin/sh
export DIR=`dirname $0`
cd $DIR
export ABSDIR=`cd .;pwd`

# Fail on errors
set -e

# define directories
export SRC_DIR=$ABSDIR/dependencies_source
export INSTALL_DIR=$ABSDIR/dependencies

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

export PATH=$PATH:$INSTALL_DIR/bin
export PATH=$PATH:"$VSDIR/VC/bin":"$VSDIR/Common7/IDE"
export PATH=$PATH:/C/MinGW/bin
export PKG_CONFIG_PATH=$PKG_CONFIG_PATH:$INSTALL_DIR/lib/pkgconfig

# create target directory where to compile tos
echo "creating install dir"
mkdir -p $INSTALL_DIR

echo "VSDIR:       $VSDIR"
echo "SRC_DIR:     $SRC_DIR"
echo "INSTALL_DIR: $INSTALL_DIR"

cd $SRC_DIR
mkdir -p win32

# yasm
# download by hand
# git version doesn't work, as no autoconf is installed by default
if [ -e $INSTALL_DIR/bin/yasm ]; then
    echo "yasm seems to already exist"
else
    cd win32

    if [ -d yasm-1.2.0 ]; then
        echo "Yasm already downloaded"
    else
        curl -fL http://www.tortall.net/projects/yasm/releases/yasm-1.2.0.tar.gz > yasm-1.2.0.tar.gz
        tar -xzf yasm-1.2.0.tar.gz
    fi
    echo "Compiling yasm ..."

    cd yasm-1.2.0
    ./configure --prefix=$INSTALL_DIR
    make -j2
    make install
    cd ../../
fi



if [ -e $INSTALL_DIR/bin/pkg-config.exe ]; then
    echo "pkgconfig seems to already exist"
else
    echo "Fetching pkgconfig"
    cd win32
    curl -fL http://sflx.net/files/pkg-config-lite/pkg-config-lite-0.26-1_bin-win32.zip > pkg-config-lite-0.26-1_bin-win32.zip
    unzip pkg-config-lite-0.26-1_bin-win32.zip
    cp -rf pkg-config-lite-0.26-1/* $INSTALL_DIR/
    rm -r pkg-config-lite-0.26-1
    rm pkg-config-lite-0.26-1_bin-win32.zip
    cd ..
fi


# compile polarssl
if [ -e $INSTALL_DIR/lib/polarssl.dll ]; then
    echo "polarssl seems to already exist"
else

    cd polarssl

#    make clean
    make SYS=posix DESTDIR=$INSTALL_DIR lib
    make SYS=posix DESTDIR=$INSTALL_DIR install

    cd ../
fi


# rtmpdump
if [ -e $INSTALL_DIR/bin/rtmpdump ]; then
    echo "rtmpdump seems to already exist"
else
    echo "Compiling rtmpdump ..."
    cd rtmpdump

    LIBZ="-lssl -lcrypto -ldl" make CRYPTO=POLARSSL SYS=mingw prefix=$INSTALL_DIR \
        XCFLAGS=-I$INSTALL_DIR/include XLDFLAGS=-L$INSTALL_DIR/lib -j2 install
    cd ..
fi



# lib x264 needs to get build!
if [ -e $INSTALL_DIR/bin/x264 ]; then
	echo "libx264 seems to already exist"
else
	echo "Compiling x264 ..."

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
	echo "Compiling ffmpeg ..."
	
	cd ffmpeg

    export PKG_CONFIG_PATH="$PKG_CONFIG_PATH:$INSTALL_DIR/lib/pkgconfig"
#    export PKG_CONFIG="$INSTALL_DIR/bin/pkg-config"

    # ./configure --prefix=$INSTALL_DIR --enable-shared --enable-libx264 --enable-gpl --enable-librtmp --enable-memalign-hack --pkg-config=$INSTALL_DIR/bin/pkg-config --extra-cflags=-I$INSTALL_DIR/include --extra-cxxflags=-I$INSTALL_DIR/include --extra-ldflags=-L$INSTALL_DIR/lib
    ./configure --prefix=$INSTALL_DIR \
        --enable-shared --enable-libx264 --enable-gpl --enable-librtmp \
        --disable-everything --enable-encoder=libx264 --enable-muxer=flv \
        --enable-protocol=rtmps --enable-protocol=tcp --enable-protocol=rtp \
        --enable-protocol=rtmp --enable-protocol=file --enable-memalign-hack \
        --pkg-config=$INSTALL_DIR/bin/pkg-config \
        --extra-cflags=-I$INSTALL_DIR/include --extra-cxxflags=-I$INSTALL_DIR/include \
        --extra-ldflags="-L$INSTALL_DIR/lib -L$INSTALL_DIR/bin" --extra-libs="-lrtmp"

	make -j4
	make install -k
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

# Checks if a given .DLL file is already from $PATH, if not, then copy (from Mingw, which is in $PATH)
# Param $1: dll Name
# Param $2: Human readable name
function CheckDLL {
	if [ -e $INSTALL_DIR/bin/$1 ]; then
		echo "$2 ($1) already exists."
	else
		echo "Copy $2 ($1)"
		export DLL_PLACE=`which $1`
		cp $DLL_PLACE $INSTALL_DIR/bin
	fi
}

CheckDLL libz-1.dll "Zlib DLL"
CheckDLL libgcc_s_dw2-1.dll "GCC Support Library"
CheckDLL pthreadGC2.dll "Pthread"
