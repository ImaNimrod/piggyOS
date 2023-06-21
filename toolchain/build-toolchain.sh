#!/bin/bash

set -e

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

export PREFIX="${DIR}/build"
export TARGET=i686-piggyOS

export CFLAGS="-g0 -O2 -mtune=native"
export CXXFLAGS="-g0 -O2 -mtune=native"

BINUTILS_VERSION="2.39"
BINUTILS_MD5SUM="ab6825df57514ec172331e988f55fc10"
BINUTILS_NAME="binutils-$BINUTILS_VERSION"
BINUTILS_PKG="${BINUTILS_NAME}.tar.gz"
BINUTILS_BASE_URL="https://ftp.gnu.org/gnu/binutils"

GCC_VERSION="12.2.0"
GCC_MD5SUM="d7644b494246450468464ffc2c2b19c3"
GCC_NAME="gcc-$GCC_VERSION"
GCC_PKG="${GCC_NAME}.tar.gz"
GCC_BASE_URL="https://ftp.gnu.org/gnu/gcc"

mkdir -p "$DIR/tarballs"

pushd "$DIR/tarballs"
    # download and verify binutils
    md5=""
    if [ -e "$BINUTILS_PKG" ]; then
        md5="$(md5sum $BINUTILS_PKG | cut -f1 -d' ')"
        echo "binutils-${BINUTILS_VERSION} MD5: '$md5'"
    fi
    if [ "$md5" != ${BINUTILS_MD5SUM} ] ; then
        rm -f $BINUTILS_PKG
        curl -LO "$BINUTILS_BASE_URL/$BINUTILS_PKG"
    else
        echo "skipped downloading binutils-${BINUTILS_VERSION}"
    fi

    # download and verify gcc
    md5=""
    if [ -e "$GCC_PKG" ]; then
        md5="$(md5sum ${GCC_PKG} | cut -f1 -d' ')"
        echo "gcc-$GCC_VERSION MD5: '$md5'"
    fi
    if [ "$md5" != ${GCC_MD5SUM} ] ; then
        rm -f $GCC_PKG
        curl -LO "$GCC_BASE_URL/$GCC_NAME/$GCC_PKG"
    else
        echo "skipped downloading gcc-${GCC_VERSION}"
    fi

    # extract and apply patches to binutils
    if [ ! -d "${BINUTILS_NAME}" ]; then
        echo "extracting binutils-${BINUTILS_VERSION}"
        tar -xzf ${BINUTILS_PKG}

        echo "patching binutils"
        pushd ${BINUTILS_NAME}
            for patch in "${DIR}"/patches/binutils/*.patch; do
                patch -p1 < "${patch}" > /dev/null
            done
        popd
    else
        echo "using existing binutils-${BINUTILS_VERSION} source"
    fi

    # extract and apply patches to gcc
    if [ ! -d "${GCC_NAME}" ]; then
        echo "extracting gcc-${GCC_VERSION}"
        tar -xzf ${GCC_PKG}

        echo "patching gcc"
        pushd ${GCC_NAME}
            for patch in "${DIR}"/patches/gcc/*.patch; do
                patch -p1 < "${patch}" > /dev/null
            done
        popd

    else 
        echo "using existing gcc-${GCC_VERSION} source"
    fi
    
    # clean-up
    rm -rf ${BINUTILS_NAME}/build_binutils
    rm -rf ${GCC_NAME}/build_gcc
    mkdir -p ${DIR}/build/

    # compile binutils
    pushd ${DIR}/tarballs/${BINUTILS_NAME}
        mkdir -p build_binutils

        pushd build_binutils
            "$DIR"/tarballs/$BINUTILS_NAME/configure --prefix="$PREFIX" \
                                            --target="$TARGET" \
                                            --with-sysroot \
                                            --disable-nls \
                                            --disable-werror
            make -j "$(nproc)"
            make install
        popd
    popd
    
    # compile gcc
    pushd ${DIR}/tarballs/${GCC_NAME}
        mkdir -p build_gcc

        pushd build_gcc
            "$DIR"/tarballs/$GCC_NAME/configure --prefix="$PREFIX" \
                                       --target="$TARGET" \
                                       --disable-nls \
                                       --enable-shared \
                                       --enable-languages=c \
                                       --without-headers \
                                       --enable-default-pie
            make -j "$(nproc)" all-gcc
            make -j "$(nproc)" all-target-libgcc
            make install-gcc install-target-libgcc
        popd
    popd
popd
