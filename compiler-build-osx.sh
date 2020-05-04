#!/bin/bash

MCHP_RESOURCE=A
MCHP_VERSION=2.40

MCHP_RESOURCE="\'${MCHP_RESOURCE}\'"
export MCHP_VERSION
export MCHP_RESOURCE

SUPPORT_HOSTED_LIBSTDCXX="--disable-hosted-libstdcxx"
SUPPORT_SJLJ_EXCEPTIONS="--enable-sjlj-exceptions"

NEWLIB_CONFIGURE_FLAGS="
	--target=pic32mx --enable-target-optspace --disable-threads --enable-static --disable-libmudflap --disable-libssp --disable-libstdcxx-pch --disable-hosted-libstdcxx
	--with-arch=pic32mx --enable-sgxx-sde-multilib --with-gnu-as --with-gnu-ld --enable-languages=c,c++ --disable-shared --disable-nls --with-dwarf2 --disable-bootstrap
	--enable-obsolete --enable-sjlj-exceptions --disable-__cxa_atexit --disable-libfortran --disable-libgomp --disable-libffi --program-prefix=pic32- --with-newlib
	--enable-newlib-io-long-long --disable-newlib-multithread --disable-libgloss --disable-newlib-supplied-syscalls --disable-nls --disable-libunwind-exceptions    
	--enable-libstdcxx-allocator=malloc --disable-newlib-atexit-alloc --disable-libstdcxx-verbose -enable-lto --enable-fixed-point --enable-obsolete --disable-sim
	--disable-checking --disable-debug --with-bugurl=https://github.com/CyberCastle/PIC32-MCU-Compiler"

GCC_CONFIGURE_FLAGS="
	--enable-target-optspace --disable-libunwind-exceptions --enable-sjlj-exceptions --enable-libstdcxx-allocator=malloc --disable-hosted-libstdcxx --target=pic32mx
	--enable-target-optspace --program-prefix=pic32- --disable-threads --disable-libmudflap --disable-libssp --enable-sgxx-sde-multilibs --with-gnu-as --with-gnu-ld
	--enable-languages=c,c++ --disable-shared --enable-static --with-newlib --disable-nls --disable-libgomp --without-headers --disable-libffi --disable-bootstrap
	--disable-decimal-float --disable-libquadmath --disable-__cxa_atexit --disable-libfortran --disable-libstdcxx-pch --with-dwarf2 --disable-libstdcxx-verbose
	--enable-poison-system-directories --enable-lto --enable-fixed-point --enable-obsolete --disable-sim --disable-checking --disable-gofast --disable-debug
	--with-bugurl=https://github.com/CyberCastle/PIC32-MCU-Compiler"

CXX_FLAGS_FOR_TARGET="
	-fno-rtti -fno-exceptions -fomit-frame-pointer -DPREFER_SIZE_OVER_SPEED -D__OPTIMIZE_SIZE__ -Os -fshort-wchar -fno-unroll-loops -fno-enforce-eh-specs
	-ffunction-sections -fdata-sections -DSMALL_MEMORY -D_NO_GETLOGIN -D_NO_GETPWENT -D_NO_GETUT -D_NO_GETPASS -D_NO_SIGSET"

CFLAGS_FOR_TARGET="
	-G 0 -fno-rtti -fno-exceptions -fomit-frame-pointer -DPREFER_SIZE_OVER_SPEED -D__OPTIMIZE_SIZE__ -Os -fshort-wchar -fno-unroll-loops -fno-enforce-eh-specs
	-ffunction-sections -fdata-sections -DSMALL_MEMORY -D_NO_GETLOGIN -D_NO_GETPWENT -D_NO_GETUT -D_NO_GETPASS -D_NO_SIGSET"

CCASFLAGS_FOR_TARGET="
	-G 0 -fno-rtti -fno-exceptions -fomit-frame-pointer -DPREFER_SIZE_OVER_SPEED -D__OPTIMIZE_SIZE__ -Os -fshort-wchar -fno-unroll-loops -fno-enforce-eh-specs
	-ffunction-sections -fdata-sections -DSMALL_MEMORY -D_NO_GETLOGIN -D_NO_GETPWENT -D_NO_GETUT -D_NO_GETPASS -D_NO_SIGSET"

XGCC_FLAGS_FOR_TARGET="
	-G 0 -fno-rtti -fno-exceptions -fomit-frame-pointer -DPREFER_SIZE_OVER_SPEED -D__OPTIMIZE_SIZE__ -Os -fshort-wchar -fno-unroll-loops -fno-enforce-eh-specs
	-ffunction-sections -fdata-sections -DSMALL_MEMORY -D_NO_GETLOGIN -D_NO_GETPWENT -D_NO_GETUT -D_NO_GETPASS -D_NO_SIGSET"

echo "$BASH_SOURCE START BUILD..."

# Does notify-send exist?
if [ "x$NOTIFY_SEND" == "x" ]; then
    WHICH_NOTIFY_SEND=$(which notify-send)
    if [ "x$WHICH_NOTIFY_SEND" == "x" ]; then
        unset NOTIFY_SEND
    else
        NOTIFY_SEND=notify-send
    fi
    unset WHICH_NOTIFY_SEND
fi

# Does growlnotify exist?
if [ "x$GROWL_SEND" == "x" ]; then
    WHICH_GROWLSEND=$(which growlnotify)
    if [ "x$WHICH_GROWLSEND" == "x" ]; then
        unset GROWL_SEND
    else
        GROWL_SEND="growlnotify"
    fi
    unset WHICH_GROWLSEND
fi

DATE=$(date +%Y-%m-%d)
TIME=$(date +%H:%M:%S)
BUILD=Compiler
SKIPLIBS=""
SKIPGRAPHITE="yes"
NATIVEIMAGE=$(uname)
NATIVEIMAGE+="-image"
echo "Native image is $NATIVEIMAGE"

NM_FOR_TARGET="$WORKING_DIR/$NATIVEIMAGE/pic32-tools/bin/pic32-nm"
RANLIB_FOR_TARGET="$WORKING_DIR/$NATIVEIMAGE/pic32-tools/bin/pic32-ranlib"
STRIP_FOR_TARGET="$WORKING_DIR/$NATIVEIMAGE/pic32-tools/bin/pic32-strip"
AR_FOR_TARGET="$WORKING_DIR/$NATIVEIMAGE/pic32-tools/bin/pic32-ar"
AS_FOR_TARGET="$WORKING_DIR/$NATIVEIMAGE/pic32-tools/pic32mx/bin/as"
LD_FOR_TARGET="$WORKING_DIR/$NATIVEIMAGE/pic32-tools/pic32mx/bin/ld"
GCC_FOR_TARGET="$WORKING_DIR/$NATIVEIMAGE/pic32-tools/bin/pic32-gcc"
CC_FOR_TARGET="$WORKING_DIR/$NATIVEIMAGE/pic32-tools/bin/pic32-gcc"
CFLAGS="-Os"

BUILD=$BUILD-$MCHP_VERSION
LOGFILE=$(pwd)/$BUILD.log
WORKING_DIR=$(pwd)
echo "Working folder $WORKING_DIR" | tee -a "$LOGFILE"

####
# assert_success()
#   If the first parameter is non-zero, print the second parameter
#   and exit the script
####
function assert_success() {
    local RESULT=$1
    local MESSAGE=$2
    if [ $RESULT != 0 ]; then
        echo "$MESSAGE ($RESULT)"
        if [ "x$GROWL_SEND" != "x" ]; then
            echo "$GROWL_SEND -s -p1 -t $BASH_SOURCE -m $MESSAGE"
            $GROWL_SEND "-s" "-p1" "-t" "$BASH_SOURCE" "-m" "$MESSAGE"
        elif [ "x$NOTIFY_SEND" != "x" ]; then
            $NOTIFY_SEND "$MESSAGE" "Build Error"
        fi
        echo "$MESSAGE ($RESULT)" 2>&1 | tee -a "$LOGFILE"
        unset GCC_FOR_TARGET
        unset CC_FOR_TARGET
        unset CXX_FOR_TARGET
        unset GXX_FOR_TARGET
        unset CPP_FOR_TARGET
        unset CC_FOR_BUILD
        unset CXX_FOR_BUILD
        unset CC
        unset CPP
        unset CXX
        unset LD
        unset AR
        exit $RESULT
    fi
}

function status_update() {
    local MESSAGE=$1
    if [ "x$GROWL_SEND" != "x" ]; then
        $GROWL_SEND "-t" "$BASH_SOURCE:" "-m" "$MESSAGE"
    elif [ "x$NOTIFY_SEND" != "x" ]; then
        $NOTIFY_SEND "$MESSAGE" "$BASH_SOURCE"
    fi
    echo "$DATE $TIME $MESSAGE" 2>&1 | tee -a "$LOGFILE"
}

### build_xc32_sh()
### $1 name of the directory
### $2 any extra arguments for the make
function build_xc32_sh() {
    mkdir -p "$1/pic32-tools/bin/bin"

    XC32_SH_SRC=${WORKING_DIR}/pic32-sh
    cd "$XC32_SH_SRC/bin" || exit

    # Clean is necessary, since there isn't a separate build directory
    make clean >>/dev/null

    make xc32 MACOS_DEVELOPERDIR="${DEVELOPERDIR}$2" 2>&1 | tee -a "$LOGFILE" >>/dev/null
    assert_success $? "Error building the XC32 shell"

    # Install no need to add the /bin for call below
    export XC32_INSTALL="$1/pic32-tools"
    make pic32-compiler-install $2 2>&1 | tee -a "$LOGFILE" >>/dev/null
    assert_success $? "Error installing the XC32 shell"
    unset XC32_INSTALL

    cd "$WORKING_DIR"
}

function copy_device_files() {
    cp -R ../pic32-part-support/device-info-files/device_files pic32-tools/bin/device_files/ 2>&1 | tee -a "$LOGFILE" >>/dev/null
    cp ../pic32-part-support/device-info-files/xc32_device.info pic32-tools/bin/ 2>&1 | tee -a "$LOGFILE" >>/dev/null
    cp ../pic32-part-support/device-info-files/deviceSupport.xml pic32-tools/bin/ 2>&1 | tee -a "$LOGFILE" >>/dev/null
    cp -R ../pic32-part-support/device-info-files/device_files pic32-tools/pic32mx/device_files/ 2>&1 | tee -a "$LOGFILE" >>/dev/null
    cp ../pic32-part-support/device-info-files/xc32_device.info pic32-tools/pic32mx/ 2>&1 | tee -a "$LOGFILE" >>/dev/null
}

### Main script body
echo "$DATE $TIME START PIC32 build." | tee -a "$LOGFILE"
cd "$WORKING_DIR"

BUILDMACHINE="--build=x86_64-apple-darwin18"
DEVELOPERDIR="/Library/Developer/CommandLineTools"
HOSTMACHINE="--host=x86_64-apple-darwin18"
COMPATIBILITY_FLAGS="-isysroot $DEVELOPERDIR/SDKs/MacOSX.sdk -mmacosx-version-min=10.14 -I$DEVELOPERDIR/SDKs/MacOSX.sdk/usr/include/malloc"
#export CXX_FOR_BUILD="$DEVELOPERDIR/usr/bin/g++ $COMPATIBILITY_FLAGS"
#export CC_FOR_BUILD="$DEVELOPERDIR/usr/bin/gcc $COMPATIBILITY_FLAGS"
#export CC="$DEVELOPERDIR/usr/bin/gcc $COMPATIBILITY_FLAGS"
#export CPP="$DEVELOPERDIR/usr/bin/cpp $COMPATIBILITY_FLAGS"
#export CXX="$DEVELOPERDIR/usr/bin/g++ $COMPATIBILITY_FLAGS"
#export LD="$DEVELOPERDIR/usr/bin/gcc $COMPATIBILITY_FLAGS"
export AR="$DEVELOPERDIR/usr/bin/ar"
# It's unclear what versions of MacOS need this, but probably
# anything after 2011.
export LDFLAGS="-Wl,-search_paths_first"

LIBHOST=""

cd pic32-part-support

# Install headers into cross compiler's install image directory
echo "$DATE $TIME Making library headers for cross compiler's install image..." 2>&1 | tee -a "$LOGFILE"

make DESTROOT="$WORKING_DIR/$NATIVEIMAGE/pic32-tools" install-headers 2>&1 | tee -a "$LOGFILE" >>/dev/null
assert_success $? "ERROR: Making headers into cross compiler's $NATIVEIMAGE install image directory"

# For check installation
make DESTROOT="$WORKING_DIR/export-image/pic32-tools" install-headers 2>&1 | tee -a "$LOGFILE" >>/dev/null
assert_success $? "ERROR: Making headers into cross compiler's export-image install image directory"

# Install pic32-fdlibm headers
cd "$WORKING_DIR"/pic32-fdlibm/src/xc32

echo "Making pic32-fdlibm headers for cross-compiler" | tee -a "$LOGFILE"
echo "$DATE $TIME Making pic32-fdlibm library headers for cross compiler's install image..." 2>&1 | tee -a "$LOGFILE"

make DESTROOT="$WORKING_DIR/$NATIVEIMAGE/pic32-tools" install-headers 2>&1 | tee -a "$LOGFILE" >>/dev/null
assert_success $? "ERROR: Making headers into cross compiler's $NATIVEIMAGE install image directory"

# For check installation
make DESTROOT="$WORKING_DIR/export-image/pic32-tools" install-headers 2>&1 | tee -a "$LOGFILE" >>/dev/null
assert_success $? "ERROR: Making headers into cross compiler's export-image install image directory"

cd "$WORKING_DIR"

build_xc32_sh "$WORKING_DIR/$NATIVEIMAGE"

# Build native cross compiler
echo "$DATE $TIME Creating cross build in $WORKING_DIR/native-build..." 2>&1 | tee -a "$LOGFILE"

status_update "Beginning native pic32 build"

if [ -e native-build ]; then
    rm -rf native-build
fi
mkdir native-build
assert_success $? "ERROR: creating directory $WORKING_DIR/native-build"

cd native-build

if [ -e binutils ]; then
    rm -rf binutils
fi
mkdir binutils
assert_success $? "ERROR: creating directory $WORKING_DIR/native-build/binutils"

cd binutils

# Configure cross binutils
echo "$DATE $TIME Configuring cross binutils build in $WORKING_DIR/native-build..." 2>&1 | tee -a "$LOGFILE"
../../pic32-compiler/src48x/binutils/configure \
    $HOSTMACHINE --target=pic32mx --prefix="$WORKING_DIR/$NATIVEIMAGE/pic32-tools" --bindir="$WORKING_DIR/$NATIVEIMAGE/pic32-tools/bin/bin" \
    --libexecdir="$WORKING_DIR/$NATIVEIMAGE/pic32-tools/bin/bin" --disable-nls --disable-tui --disable-gdbtk --disable-shared --enable-static \
    --disable-threads --disable-bootstrap --with-dwarf2 --enable-multilib --without-newlib --disable-sim --with-lib-path=: --enable-poison-system-directories \
    --program-prefix=pic32- --disable-werror --disable-debug \
    --with-bugurl=https://github.com/CyberCastle/PIC32-MCU-Compiler 2>&1 | tee -a "$LOGFILE" >>/dev/null

assert_success $? "ERROR: configuring cross binutils build"

# Make cross binutils and install it
echo "$DATE $TIME Making all in $WORKING_DIR/native-build/binutils and installing..." 2>&1 | tee -a "$LOGFILE"
make CFLAGS="-O2 -DMCHP_VERSION=${MCHP_VERSION}" all -j4 2>&1 | tee -a "$LOGFILE" >>/dev/null
assert_success $? "ERROR: making/installing cross binutils build"
make install 2>&1 | tee -a "$LOGFILE" >>/dev/null
assert_success $? "ERROR: making/installing cross binutils build"

cd ..

if [ -e gmp ]; then
    rm -rf gmp
fi
mkdir gmp
assert_success $? "ERROR: creating directory $WORKING_DIR/native-build/gmp"

cd gmp

echo "$DATE $TIME Configuring native gmp build in $WORKING_DIR/native-build/gmp..." 2>&1 | tee -a "$LOGFILE"
../../pic32-compiler/src48x/gmp/configure \
    $HOSTMACHINE $BUILDMACHINE --enable-cxx --prefix=$WORKING_DIR/native-build/host-libs --disable-shared --enable-static --disable-nls --with-gnu-ld \
    --disable-debug --disable-rpath --enable-fft --enable-hash-synchronization 2>&1 | tee -a "$LOGFILE" >>/dev/null

# Make native gmp and install it
echo "$DATE $TIME Making all in $WORKING_DIR/native-build/gmp and installing..." 2>&1 | tee -a "$LOGFILE" >>/dev/null
make all -j4 2>&1 | tee -a "$LOGFILE" >>/dev/null
assert_success $? "ERROR: making/installing gmp build"
make install 2>&1 | tee -a "$LOGFILE" >>/dev/null
assert_success $? "ERROR: making/installing gmp build"

cd ..

if [ "x$SKIPGRAPHITE" == "x" ]; then
    if [ -e ppl ]; then
        rm -rf ppl
    fi
    mkdir ppl
    assert_success $? "ERROR: creating directory $WORKING_DIR/linux32-build/ppl"

    cd ppl
    echo "$DATE $TIME Configuring native ppl build in $WORKING_DIR/native-build/ppl..." 2>&1 | tee -a "$LOGFILE"
    ../../pic32-compiler/src48x/ppl/configure \
        --prefix=$WORKING_DIR/native-build/host-libs --disable-shared --enable-static --with-gnu-ld $HOSTMACHINE --target=pic32mx --disable-nls \
        --with-libgmp-prefix=$WORKING_DIR/native-build/host-libs --with-gmp=$WORKING_DIR/native-build/host-libs 2>&1 | tee -a "$LOGFILE" >>/dev/null

    # Make native ppl and install it
    echo "$DATE $TIME Making all in $WORKING_DIR/native-build/ppl and installing..." 2>&1 | tee -a "$LOGFILE"
    make all -j4 2>&1 | tee -a "$LOGFILE" >>/dev/null
    assert_success $? "ERROR: making/installing ppl build"
    make install 2>&1 | tee -a "$LOGFILE" >>/dev/null
    assert_success $? "ERROR: making/installing ppl build"

    cd ..

    USE_PPL="--with-ppl=$WORKING_DIR/native-build/host-libs --with-isl=$WORKING_DIR/native-build/host-libs"

    if [ -e cloog ]; then
        rm -rf cloog
    fi
    mkdir cloog
    assert_success $? "ERROR: creating directory $WORKING_DIR/native-build/cloog"

    cd cloog

    echo "$DATE $TIME Configuring native cloog build in $WORKING_DIR/native-build/cloog..." 2>&1 | tee -a "$LOGFILE"
    ../../pic32-compiler/src48x/cloog/configure \
        $BUILDMACHINE --enable-optimization=speed --with-gnu-ld '--with-host-libstdcxx=-static-libgcc -Wl,-Bstatic,-lstdc++,-Bdynamic -lm' \
        --prefix=$WORKING_DIR/native-build/host-libs--with-gmp=$WORKING_DIR/native-build/host-libs --with-ppl=$WORKING_DIR/native-build/host-libs --target=pic32mx \
        --disable-shared --enable-static --disable-shared 2>&1 | tee -a "$LOGFILE" >>/dev/null

    # Make native cloog and install it
    echo "$DATE $TIME Making all in $WORKING_DIR/native-build/cloog and installing..." 2>&1 | tee -a "$LOGFILE"
    make all -j4 2>&1 | tee -a "$LOGFILE" >>/dev/null
    assert_success $? "ERROR: making/installing cloog build"
    make install 2>&1 | tee -a "$LOGFILE" >>/dev/null
    assert_success $? "ERROR: making/installing cloog build"

    cd ..
    USE_CLOOG="--with-cloog=$WORKING_DIR/native-build/host-libs"
else
    USE_CLOOG="--without-cloog"
    USE_PPL="--without-isl"
fi

if [ -e libelf ]; then
    rm -rf libelf
fi
mkdir libelf
assert_success $? "ERROR: creating directory $WORKING_DIR/native-build/libelf"

cd libelf
echo "$DATE $TIME Configuring native libelf build in $WORKING_DIR/native-build/libelf..." 2>&1 | tee -a "$LOGFILE"
../../pic32-compiler/src48x/libelf/configure \
    --prefix=$WORKING_DIR/native-build/host-libs $HOSTMACHINE --target=pic32mx --disable-shared --disable-debug --disable-nls 2>&1 | tee -a "$LOGFILE" >>/dev/null

# Make native libelf and install it
echo "$DATE $TIME Making all in $WORKING_DIR/native-build/libelf and installing..." 2>&1 | tee -a "$LOGFILE"
make all -j4 2>&1 | tee -a "$LOGFILE" >>/dev/null
assert_success $? "ERROR: making/installing libelf build"
make install 2>&1 | tee -a "$LOGFILE" >>/dev/null
assert_success $? "ERROR: making/installing libelf build"
cd ..

if [ -e zlib ]; then
    rm -rf zlib
fi
cp -r ../pic32-compiler/src48x/zlib .

assert_success $? "ERROR: copy src48x/zlib directory to $WORKING_DIR/native-build/zlib"

cd zlib
echo "$DATE $TIME Configuring native zlib build in $WORKING_DIR/native-build/zlib..." 2>&1 | tee -a "$LOGFILE"
./configure --prefix=$WORKING_DIR/native-build/host-libs 2>&1 | tee -a "$LOGFILE" >>/dev/null

# Make native zlib and install it
echo "$DATE $TIME Making all in $WORKING_DIR/native-build/zlib and installing..." 2>&1 | tee -a "$LOGFILE"
make all -j4 2>&1 | tee -a "$LOGFILE" >>/dev/null
assert_success $? "ERROR: making/installing zlib build"
make install 2>&1 | tee -a "$LOGFILE" >>/dev/null
assert_success $? "ERROR: making/installing zlib build"

cd ..

if [ -e gcc ]; then
    rm -rf gcc
fi
mkdir gcc
assert_success $? "ERROR: creating directory $WORKING_DIR/native-build/gcc"

cd gcc

# Configure cross compiler
echo "$DATE $TIME Configuring cross compiler build in $WORKING_DIR/native-build..." 2>&1 | tee -a "$LOGFILE"

AR_FOR_TARGET="$WORKING_DIR/$NATIVEIMAGE/pic32-tools/pic32mx/bin/ar" \
    AS_FOR_TARGET="$WORKING_DIR/$NATIVEIMAGE/pic32-tools/pic32mx/bin/as" \
    LD_FOR_TARGET="$WORKING_DIR/$NATIVEIMAGE/pic32-tools/pic32mx/bin/ld" \
    ../../pic32-compiler/src48x/gcc/configure \
    --target=pic32mx --program-prefix=pic32- --disable-threads --disable-libmudflap --disable-libssp --enable-sgxx-sde-multilibs --with-gnu-as --with-gnu-ld \
    --enable-languages=c,c++ --disable-shared --enable-static --with-newlib --disable-nls --disable-libgomp --without-headers --disable-libffi --disable-bootstrap \
    --disable-decimal-float --disable-libquadmath --disable-__cxa_atexit --disable-libfortran --disable-libstdcxx-pch --prefix="$WORKING_DIR/$NATIVEIMAGE/pic32-tools" \
    --bindir="$WORKING_DIR/$NATIVEIMAGE/pic32-tools/bin/bin" --libexecdir="$WORKING_DIR/$NATIVEIMAGE/pic32-tools/bin/bin" --with-dwarf2 \
    --with-gmp="$WORKING_DIR/native-build/host-libs" $USE_CLOOG $USE_PPL "$LIBHOST" --enable-lto --enable-fixed-point --disable-debug \
    XGCC_FLAGS_FOR_TARGET="-frtti -fexceptions -fno-enforce-eh-specs" CXXFLAGS="-g3" $SUPPORT_SJLJ_EXCEPTIONS --enable-obsolete --disable-sim \
    --disable-checking $SUPPORT_HOSTED_LIBSTDCXX --with-bugurl=https://github.com/CyberCastle/PIC32-MCU-Compiler 2>&1 | tee -a "$LOGFILE" >>/dev/null

assert_success $? "ERROR: configuring cross build"

# Make cross compiler and install it
echo "$DATE $TIME Making all in $WORKING_DIR/native-build/gcc and installing..." 2>&1 | tee -a "$LOGFILE"
make all-gcc CFLAGS="-O2" CXXFLAGS="-g3 -DTARGET_IS_PIC32MX" \
    NM_FOR_TARGET="$WORKING_DIR/$NATIVEIMAGE/pic32-tools/bin/pic32-nm" \
    RANLIB_FOR_TARGET="$WORKING_DIR/$NATIVEIMAGE/pic32-tools/bin/pic32-ranlib" \
    STRIP_FOR_TARGET="$WORKING_DIR/$NATIVEIMAGE/pic32-tools/bin/pic32-strip" \
    AR_FOR_TARGET="$WORKING_DIR/$NATIVEIMAGE/pic32-tools/bin/pic32-ar" \
    AS_FOR_TARGET="$WORKING_DIR/$NATIVEIMAGE/pic32-tools/pic32mx/bin/as" \
    LD_FOR_TARGET="$WORKING_DIR/$NATIVEIMAGE/pic32-tools/pic32mx/bin/ld" -j2 2>&1 | tee -a "$LOGFILE" >>/dev/null
assert_success $? "ERROR: making/installing cross build all-gcc"

make CFLAGS="-O2 -DTARGET_IS_PIC32MX" CXXFLAGS="-O2 -DTARGET_IS_PIC32MX" install-gcc 2>&1 | tee -a "$LOGFILE" >>/dev/null
assert_success $? "ERROR: making/installing cross build install-gcc"

cd ..

if [ -e newlib ]; then
    rm -rf newlib
fi
mkdir newlib
assert_success $? "ERROR: creating directory $WORKING_DIR/native-build/gcc"

cd newlib
status_update "Building newlib"

#build newlib here
GCC_FOR_TARGET=$WORKING_DIR/$NATIVEIMAGE/pic32-tools/bin/bin/pic32-gcc \
    CC_FOR_TARGET="$WORKING_DIR/$NATIVEIMAGE/pic32-tools/bin/bin/pic32-gcc -I$WORKING_DIR/pic32-compiler/src48x/gcc/gcc/ginclude" \
    CXX_FOR_TARGET=$WORKING_DIR/$NATIVEIMAGE/pic32-tools/bin/bin/pic32-g++ \
    CPP_FOR_TARGET=$WORKING_DIR/$NATIVEIMAGE/pic32-tools/bin/bin/pic32-g++ \
    AR_FOR_TARGET=$WORKING_DIR/$NATIVEIMAGE/pic32-tools/bin/bin/pic32-ar \
    RANLIB_FOR_TARGET=$WORKING_DIR/$NATIVEIMAGE/pic32-tools/bin/bin/pic32-ranlib \
    READELF_FOR_TARGET=$WORKING_DIR/$NATIVEIMAGE/pic32-tools/bin/bin/pic32-readelf \
    STRIP_FOR_TARGET=$WORKING_DIR/$NATIVEIMAGE/pic32-tools/bin/bin/pic32-strip \
    AS_FOR_TARGET=$WORKING_DIR/$NATIVEIMAGE/pic32-tools/pic32mx/bin/as \
    LD_FOR_TARGET=$WORKING_DIR/$NATIVEIMAGE/pic32-tools/pic32mx/bin/ld \
    ../../pic32-newlib/configure \
    $NEWLIB_CONFIGURE_FLAGS --prefix=$WORKING_DIR/$NATIVEIMAGE/pic32-tools --bindir="$WORKING_DIR/$NATIVEIMAGE/pic32-tools/bin/bin" \
    --libexecdir="$WORKING_DIR/$NATIVEIMAGE/pic32-tools/bin/bin" CFLAGS_FOR_TARGET="-fno-short-double -fno-rtti -fno-exceptions -fomit-frame-pointer \
    	-DPREFER_SIZE_OVER_SPEED -D__OPTIMIZE_SIZE__ -Os -fshort-wchar -fno-unroll-loops -fno-enforce-eh-specs -ffunction-sections -fdata-sections -DSMALL_MEMORY \
    	-D_NO_GETLOGIN -D_NO_GETPWENT -D_NO_GETUT -D_NO_GETPASS -D_NO_SIGSET" CCASFLAGS_FOR_TARGET="-fno-short-double -fno-rtti -fno-exceptions -fomit-frame-pointer \
    	-DPREFER_SIZE_OVER_SPEED -D__OPTIMIZE_SIZE__ -Os -fshort-wchar -fno-unroll-loops -fno-enforce-eh-specs -ffunction-sections -fdata-sections -DSMALL_MEMORY \
    	-D_NO_GETLOGIN -D_NO_GETPWENT -D_NO_GETUT -D_NO_GETPASS -D_NO_SIGSET" XGCC_FLAGS_FOR_TARGET="-fno-short-double -fno-rtti -fno-exceptions -fomit-frame-pointer \
    	-DPREFER_SIZE_OVER_SPEED -D__OPTIMIZE_SIZE__ -Os -fshort-wchar -fno-unroll-loops -fno-enforce-eh-specs -ffunction-sections -fdata-sections" \
    --enable-cxx-flags="-fno-short-double -fno-rtti -fno-exceptions -fomit-frame-pointer -DPREFER_SIZE_OVER_SPEED -D__OPTIMIZE_SIZE__ -Os -fshort-wchar -fno-unroll-loops \
    	-fno-enforce-eh-specs -ffunction-sections -fdata-sections" 2>&1 | tee -a "$LOGFILE" >>/dev/null
assert_success $? "ERROR: Configure Newlib for native build"

make all -j2 CFLAGS_FOR_TARGET="-fno-short-double -fno-rtti -fno-exceptions -fomit-frame-pointer -DPREFER_SIZE_OVER_SPEED -D__OPTIMIZE_SIZE__ -Os -fshort-wchar \
	    -fno-unroll-loops -fno-enforce-eh-specs -ffunction-sections -fdata-sections -DSMALL_MEMORY -D_NO_GETLOGIN -D_NO_GETPWENT -D_NO_GETUT -D_NO_GETPASS -D_NO_SIGSET" \
    CCASFLAGS_FOR_TARGET="-fno-short-double -fno-rtti -fno-exceptions -fomit-frame-pointer -DPREFER_SIZE_OVER_SPEED -D__OPTIMIZE_SIZE__ -Os -fshort-wchar -fno-unroll-loops \
	    -fno-enforce-eh-specs -ffunction-sections -fdata-sections -DSMALL_MEMORY -D_NO_GETLOGIN -D_NO_GETPWENT -D_NO_GETUT -D_NO_GETPASS -D_NO_SIGSET" \
    XGCC_FLAGS_FOR_TARGET="-fno-short-double -fno-rtti -fno-exceptions -fomit-frame-pointer -DPREFER_SIZE_OVER_SPEED -D__OPTIMIZE_SIZE__ -Os -fshort-wchar -fno-unroll-loops \
	    -fno-enforce-eh-specs -ffunction-sections -fdata-sections" 2>&1 | tee -a "$LOGFILE" >>/dev/null
assert_success $? "ERROR: Make newlib for native build"

make install 2>&1 | tee -a "$LOGFILE" >>/dev/null
assert_success $? "ERROR: Install newlib for native build"

cd "$WORKING_DIR"
rsync -qavzC --include "*/" --include "*" export-image/pic32-tools/ $NATIVEIMAGE/pic32-tools/ 2>&1 | tee -a "$LOGFILE"
assert_success $? "ERROR: Install newlib in $NATIVEIMAGE"

# Cleanup post newlib Installation
rm -rf export-image/

cd native-build

if [ -e gcc ]; then
    rm -rf gcc
fi
mkdir gcc
assert_success $? "ERROR: creating directory $WORKING_DIR/native-build/gcc"

cd gcc

GCC_FOR_TARGET=$WORKING_DIR/$NATIVEIMAGE/pic32-tools/bin/pic32-gcc \
    CC_FOR_TARGET=$WORKING_DIR/$NATIVEIMAGE/pic32-tools/bin/pic32-gcc \
    CPP_FOR_TARGET=$WORKING_DIR/$NATIVEIMAGE/pic32-tools/bin/pic32-g++ \
    AR_FOR_TARGET=$WORKING_DIR/$NATIVEIMAGE/pic32-tools/bin/pic32-ar \
    AS_FOR_TARGET=$WORKING_DIR/$NATIVEIMAGE/pic32-tools/pic32mx/bin/as \
    LD_FOR_TARGET=$WORKING_DIR/$NATIVEIMAGE/pic32-tools/pic32mx/bin/ld \
    ../../pic32-compiler/src48x/gcc/configure \
    $GCC_CONFIGURE_FLAGS --prefix=$WORKING_DIR/$NATIVEIMAGE/pic32-tools --bindir="$WORKING_DIR/$NATIVEIMAGE/pic32-tools/bin/bin" \
    --libexecdir="$WORKING_DIR/$NATIVEIMAGE/pic32-tools/bin/bin" --with-gmp="$WORKING_DIR/native-build/host-libs" $USE_CLOOG $USE_PPL "$LIBHOST" \
    CFLAGS_FOR_TARGET="-fno-rtti -fno-exceptions -fomit-frame-pointer -DPREFER_SIZE_OVER_SPEED -D__OPTIMIZE_SIZE__ -Os -fshort-wchar -fno-unroll-loops \
    	-fno-enforce-eh-specs -ffunction-sections -fdata-sections" XGCC_FLAGS_FOR_TARGET="-fno-rtti -fno-exceptions -fomit-frame-pointer -DPREFER_SIZE_OVER_SPEED \
    	-D__OPTIMIZE_SIZE__ -Os -fshort-wchar -fno-unroll-loops -fno-enforce-eh-specs -ffunction-sections -fdata-sections" --enable-cxx-flags="-fno-rtti -fno-exceptions \
    	-fomit-frame-pointer -DPREFER_SIZE_OVER_SPEED -D__OPTIMIZE_SIZE__ -Os -fshort-wchar -fno-unroll-loops -fno-enforce-eh-specs -ffunction-sections -fdata-sections" \
    CFLAGS_FOR_BUILD="-Os" 2>&1 | tee -a "$LOGFILE" >>/dev/null
assert_success $? "ERROR: Configure gcc after Newlib for native build"

make all \
    CXXFLAGS="$CXXFLAGS_FOR_TARGET -Os" \
    NM_FOR_TARGET="$WORKING_DIR/$NATIVEIMAGE/pic32-tools/bin/pic32-nm" \
    RANLIB_FOR_TARGET="$WORKING_DIR/$NATIVEIMAGE/pic32-tools/bin/pic32-ranlib" \
    STRIP_FOR_TARGET="$WORKING_DIR/$NATIVEIMAGE/pic32-tools/bin/pic32-strip" \
    AR_FOR_TARGET="$WORKING_DIR/$NATIVEIMAGE/pic32-tools/bin/pic32-ar" \
    AS_FOR_TARGET="$WORKING_DIR/$NATIVEIMAGE/pic32-tools/pic32mx/bin/as" \
    LD_FOR_TARGET="$WORKING_DIR/$NATIVEIMAGE/pic32-tools/pic32mx/bin/ld" -j2 2>&1 | tee -a "$LOGFILE" >>/dev/null
assert_success $? "ERROR: making/installing cross build all"

make install 2>&1 | tee -a "$LOGFILE" >>/dev/null
assert_success $? "ERROR: making/installing cross build install"

cd ..
if [ -e gdb ]; then
    rm -rf gdb
fi
mkdir gdb
assert_success $? "ERROR: creating directory $WORKING_DIR/native-build/gdb"

cd gdb
echo "$DATE $TIME Configuring gdb in $WORKING_DIR/native-build..." 2>&1 | tee -a "$LOGFILE"
../../pic32-compiler/src48x/gdb/configure \
    --prefix=$WORKING_DIR/$NATIVEIMAGE/pic32-tools \
    --bindir="$WORKING_DIR/$NATIVEIMAGE/pic32-tools/bin/bin" \
    "$HOSTMACHINE" \
    --target=pic32mx \
    --program-prefix=pic32- \
    --disable-binutils \
    --disable-gas \
    --disable-ld \
    --disable-gprof \
    --disable-sim \
    --disable-tui \
    --disable-gdbtk 2>&1 | tee -a "$LOGFILE" >>/dev/null
assert_success $? "ERROR: Configure gdb for native build"

echo "$DATE $TIME Building gdb in $WORKING_DIR/native-build..." 2>&1 | tee -a "$LOGFILE"
make 2>&1 | tee -a "$LOGFILE" >>/dev/null
assert_success $? "ERROR: making gdb for native build"

make install 2>&1 | tee -a "$LOGFILE" >>/dev/null
assert_success $? "ERROR: installing gdb for native build"

status_update "Cross build complete"
cd ../..

echo "$DATE $TIME strip native-image"
cd $NATIVEIMAGE/pic32-tools
find . -type f -perm -g+x -follow | xargs file | grep Mach-O | cut -d: -f1 | xargs $DEVELOPERDIR/usr/bin/strip -x

cd "$WORKING_DIR"
# end strip native-image

unset CC
unset CPP
unset CXX
unset LD
unset AR

# Set up path so that we can build the libraries using the cross compiler we just built
OLDPATH=$PATH
PATH=$WORKING_DIR/$NATIVEIMAGE/pic32-tools/bin:$PATH

if [ "x$SKIPLIBS" == "x" ]; then

    cd pic32-part-support

    # Build cross compiler libraries
    echo "$DATE $TIME Making and installing cross-compiler libraries to $WORKING_DIR/$NATIVEIMAGE/pic32-tools..." 2>&1 | tee -a "$LOGFILE"

    make DESTROOT=$WORKING_DIR/$NATIVEIMAGE/pic32-tools all 2>&1 | tee -a "$LOGFILE" >>/dev/null
    assert_success $? "ERROR: making libraries for cross build"

    make DESTROOT=$WORKING_DIR/$NATIVEIMAGE/pic32-tools install -j2 2>&1 | tee -a "$LOGFILE" >>/dev/null
    assert_success $? "ERROR: making libraries for $NATIVEIMAGE cross build"

    status_update "cross-compiler library build complete"

    cd ..

    # Build and install pic32-fdlibm
    cd "$WORKING_DIR"/pic32-fdlibm/src/xc32

    # Build pic32-fdlibm once
    echo "$DATE $TIME Making and installing cross-compiler pic32-fdlibm libraries to $WORKING_DIR/$NATIVEIMAGE..." 2>&1 | tee -a "$LOGFILE"
    make DESTROOT=$WORKING_DIR/$NATIVEIMAGE/pic32-tools all 2>&1 | tee -a "$LOGFILE" >>/dev/null
    assert_success $? "ERROR: making pic32-fdlibm  libraries for cross build"

    # Then install
    make DESTROOT=$WORKING_DIR/$NATIVEIMAGE/pic32-tools install -j2 2>&1 | tee -a "$LOGFILE" >>/dev/null
    assert_success $? "ERROR: making installing pic32-fdlibm for $NATIVEIMAGE cross build"

    status_update "cross-compiler pic32-fdlibm library build complete"
fi # skip library build

cd "$WORKING_DIR"

###
# In the resultant install directory, there are a few extra directories
# that we don"t want for our build.
###
echo "$DATE $TIME Removing unecessary directories from build..." | tee -a "$LOGFILE"
echo "Directory clean up for pic-tools" | tee -a "$LOGFILE"

cp -R "$WORKING_DIR/$NATIVEIMAGE/pic32-tools/include/gdb" "$WORKING_DIR/$NATIVEIMAGE/pic32-tools/pic32mx/include/"
rm -rf $WORKING_DIR/$NATIVEIMAGE/pic32-tools/include
rm -rf $WORKING_DIR/$NATIVEIMAGE/pic32-tools/man
rm -rf $WORKING_DIR/$NATIVEIMAGE/pic32-tools/info
rm -rf $WORKING_DIR/$NATIVEIMAGE/pic32-tools/share
rm -rf $WORKING_DIR/$NATIVEIMAGE/pic32-tools/pic32mx/share
rm -rf $WORKING_DIR/$NATIVEIMAGE/pic32-tools/libsrc

echo "$DATE $TIME Copying resource files..." | tee -a "$LOGFILE"
cd $NATIVEIMAGE
copy_device_files
cd ..

#ZIP installation directory.
echo "$DATE $TIME Making zip files" | tee -a "$LOGFILE"
echo "Tar components to $WORKING_DIR/zips directory..." | tee -a "$LOGFILE"
echo "Tar installation directory..." | tee -a "$LOGFILE"

if [[ ! -e zips ]]; then
    mkdir zips
fi

REV=${BUILD##pic32-}
cd $NATIVEIMAGE
# delete *.py  and *.o files
find . -type f -name *.py -delete
find . -name *-gdb.py -delete
find . -name *_pic.o -delete
find . -name *-pic.o -delete
tar -czvf $WORKING_DIR/zips/pic32-tools-$REV-$NATIVEIMAGE.tar.gz pic32-tools 2>&1 | tee -a "$LOGFILE" >>/dev/null

cd ..
shasum -a 256 "$WORKING_DIR/zips/pic32-tools-$REV-$NATIVEIMAGE.tar.gz" >"$WORKING_DIR/zips/pic32-tools-$REV-$NATIVEIMAGE.sha256"

unset GCC_FOR_TARGET
unset CC_FOR_TARGET
unset CXX_FOR_TARGET
unset GXX_FOR_TARGET
unset CPP_FOR_TARGET
unset CC_FOR_BUILD
unset CXX_FOR_BUILD
unset CC
unset CPP
unset CXX
unset LD
unset AR

PATH=$OLDPATH
echo "$DATE $TIME DONE..." 2>&1 | tee -a "$LOGFILE"
status_update "DONE"

exit 0
