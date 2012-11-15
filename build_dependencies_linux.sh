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
mkdir -p linux



# download and build gmp
if [ -e $INSTALL_DIR/lib/libgmp.a ]; then
    echo "gmp seems to already exist"
else
    cd linux

    GMP_VER=gmp-5.0.5
    if [ -d ${GMP_VER} ]; then
        echo "gmp already downloaded"
    else
        curl -fL ftp://ftp.gmplib.org/pub/${GMP_VER}/${GMP_VER}.tar.bz2 > ${GMP_VER}.tar.bz2
        tar -xf ${GMP_VER}.tar.bz2
    fi

    echo "Compiling gmp"
    cd ${GMP_VER}

    ./configure --enable-cxx --enable-shared --prefix=$INSTALL_DIR
    make -j2
    make install

    cd ../../
fi


# download and build nettle
if [ -e $INSTALL_DIR/lib/libnettle.a ]; then
    echo "nettle seems to already exist"
else
    cd linux

    if [ -d nettle-2.5.tar.gz ]; then
        echo "nettle already downloaded"
    else
        curl -fL ftp://ftp.gnu.org/gnu/nettle/nettle-2.5.tar.gz > nettle-2.5.tar.gz
        tar -xf nettle-2.5.tar.gz
    fi

    echo "Compiling nettle"
    cd nettle-2.5

    LIBS="-lgmp" ./configure --enable-shared --prefix=$INSTALL_DIR --disable-openssl --with-include-path=$INSTALL_DIR/include --with-lib-path=$INSTALL_DIR/lib --disable-assembler
    make -j2
    make install

    cd ../../
fi


# download and build gnu tls
if [ -e $INSTALL_DIR/lib/libgnutls.a ]; then
    echo "gnutls seems to already exist"
else
    cd linux

    GNUTLS_VER=gnutls-3.1.3
    if [ -d ${GNUTLS_VER} ]; then
        echo "gnutls already downloaded"
    else
        curl -fL ftp://ftp.gnu.org/gnu/gnutls/${GNUTLS_VER}.tar.xz > ${GNUTLS_VER}.tar.xz
        tar -xf ${GNUTLS_VER}.tar.xz
    fi

    echo "Compiling gnutls"
    cd ${GNUTLS_VER}

    ./configure --prefix=$INSTALL_DIR LIBS=-lnettle LDFLAGS="-L$INSTALL_DIR/lib"
    make -j2
    make install

    cd ../../
fi




# rtmpdump
if [ -e $INSTALL_DIR/bin/rtmpdump ]; then
    echo "rtmpdump seems to already exist"
else
    cd rtmpdump
    make clean
    LIB_GNUTLS="-lssl -lcrypto -ldl" make CRYPTO=GNUTLS prefix=$INSTALL_DIR XCFLAGS=-I$INSTALL_DIR/include XLDFLAGS=-L$INSTALL_DIR/lib install
    cd ..
fi



# libx264
if [ -e $INSTALL_DIR/bin/x264 ]; then
    echo "x264 seems to already exist"
else
    cd x264
    ./configure --enable-shared --prefix=$INSTALL_DIR --extra-ldflags="-L$INSTALL_DIR/lib" --extra-clfags="-I$INSTALL_DIR/include"
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

    export PKG_CONFIG_PATH=$PKG_CONFIG_PATH:$INSTALL_DIR/lib/pkgconfig

    echo "Compiling ffmpeg"
    ./configure $ADDPIC --enable-shared --enable-gpl --enable-libx264 --disable-everything \
        --enable-encoder=libx264 --enable-muxer=flv --enable-protocol=rtmps --enable-protocol=rtmp \
        --enable-protocol=file --enable-protocol=tcp --enable-protocol=rtp --enable-librtmp \
        --pkg-config=$INSTALL_DIR/bin/pkg-config \
        --prefix=$INSTALL_DIR --extra-ldflags=-L$INSTALL_DIR/lib --extra-libs=-lrtmp --extra-cflags=-I$INSTALL_DIR/include --extra-cxxflags=-I$INSTALL_DIR/include $FFMPEG_EXTRA
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

