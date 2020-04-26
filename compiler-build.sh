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
	--disable-checking --with-bugurl=https://github.com/CyberCastle/PIC32-MCU-Compiler"

GCC_CONFIGURE_FLAGS="
	--enable-target-optspace --disable-libunwind-exceptions --enable-sjlj-exceptions --enable-libstdcxx-allocator=malloc --disable-hosted-libstdcxx --target=pic32mx
	--enable-target-optspace --program-prefix=pic32- --disable-threads --disable-libmudflap --disable-libssp --enable-sgxx-sde-multilibs --with-gnu-as --with-gnu-ld
	--enable-languages=c,c++ --disable-shared --enable-static --with-newlib --disable-nls --disable-libgomp --without-headers --disable-libffi --disable-bootstrap
	--disable-decimal-float --disable-libquadmath --disable-__cxa_atexit --disable-libfortran --disable-libstdcxx-pch --with-dwarf2 --disable-libstdcxx-verbose
	--enable-poison-system-directories --enable-lto --enable-fixed-point --enable-obsolete --disable-sim --disable-checking --disable-gofast
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
# Figure out which MinGW compiler we have. Candidates are:
# i586-mingw32msvc-gcc   (Ubuntu)
# i386-mingw32-gcc       (Fedora)
if [ "x$MINGW32_HOST_PREFIX" == "x" ]; then
 MINGW_GCC=`which i686-w64-mingw32-gcc`
 if [ "x$MINGW_GCC" != "x" ] ; then
  MINGW32_HOST_PREFIX=i686-w64-mingw32
 else
  MINGW32_HOST_PREFIX=i586-mingw32msvc
 fi
 unset MINGW_GCC
fi

if [ "x$ARMLINUX32_HOST_PREFIX" == "x" ]; then
unset ARMLINUX32_HOST_PREFIX
 # ARMLINUX32_HOST_PREFIX=arm-none-linux-gnueabi
 ARMLINUX32_HOST_PREFIX=arm-linux-gnueabi
fi

# Does notify-send exist?
if [ "x$NOTIFY_SEND" == "x" ] ; then
    WHICH_NOTIFY_SEND=`which notify-send`
    if [ "x$WHICH_NOTIFY_SEND" == "x" ] ; then
        unset NOTIFY_SEND
    else
        NOTIFY_SEND=notify-send
    fi
    unset WHICH_NOTIFY_SEND
fi

# Does growlnotify exist?
if [ "x$GROWL_SEND" == "x" ] ; then
    WHICH_GROWLSEND=`which growlnotify`
    if [ "x$WHICH_GROWLSEND" == "x" ] ; then
        unset GROWL_SEND
    else
        GROWL_SEND="growlnotify"
    fi
    unset WHICH_GROWLSEND
fi

DATE=`date +%Y%m%d`
TIME=`date +%H%M`
BUILD=Compiler
FULL_ONLY=no
SKIPLIBS=""
SKIPNATIVE=""
SKIPLINUX32="yes"
SKIPWIN32="yes"
SKIPARMLINUX="yes"
SKIPGRAPHITE="yes"
SKIPMULTIPLENEWLIB="yes"
SKIPPLIBIMAGE="yes"
NATIVEIMAGE=`uname`
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

show_usage()
    {
        # Shows the usage screen
        echo "USAGE:  $0 [-t <tag> | -?]"
        echo "  -b <tag>      Specify the branch for which you would like to build"
        echo "  -t <tag>      Specify the tag for which you would like to build"
        echo "  -M            OS X build only"
        echo "  -? | -h       Show this usage message"
        exit 1
    }

# Process the arguments
while getopts b:FMNt:Q:h\? opt
do
    case "$opt" in
      t)
        TVAL=$OPTARG
        TAG="$TVAL"
        ;;
      b)
        TVAL=$OPTARG
        TAG="$TVAL"
        BUILD=pic32-$TVAL-$DATE
        ;;
      M)
        echo "OS X build only"
        SKIPLINUX32="yes"
        SKIPWIN32="yes"
        SKIPARMLINUX="yes"
        ;;
      h | \?) show_usage ;;
    esac
done

if [ "x$NATIVEIMAGE" != "xDarwin-image" ]; then
  if [ "x$OSXONLY" == "xyes" ]; then
    echo "Error: Cannot specify -M option on non-OSX host for now"
    exit 1
  fi 
fi

BUILD=$BUILD-$DATE-$TIME
LOGFILE=`pwd`/$BUILD.log
WORKING_DIR=`pwd`
echo WORKING DIR $WORKING_DIR


####
# assert_success()
#   If the first parameter is non-zero, print the second parameter
#   and exit the script
####
function assert_success ()
    {
        local RESULT=$1
        local MESSAGE=$2
        if [ $RESULT != 0 ]
        then
            echo "$MESSAGE ($RESULT)"
            if [ "x$GROWL_SEND" != "x" ] ; then
                echo "$GROWL_SEND -s -p1 -t $BASH_SOURCE -m $MESSAGE"
                $GROWL_SEND "-s" "-p1" "-t" "$BASH_SOURCE" "-m" "$MESSAGE"
            elif [ "x$NOTIFY_SEND" != "x" ] ; then
                $NOTIFY_SEND "$MESSAGE" "Build Error"
            fi
            echo "$MESSAGE ($RESULT)" 2>&1 | tee $LOGFILE >> /dev/null
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

function status_update ()
    {
        local MESSAGE=$1
        if [ "x$GROWL_SEND" != "x" ] ; then
            $GROWL_SEND "-t" "$BASH_SOURCE:" "-m" "$MESSAGE"
        elif [ "x$NOTIFY_SEND" != "x" ] ; then
            $NOTIFY_SEND "$MESSAGE" "$BASH_SOURCE"
        fi
        echo `date` $MESSAGE 2>&1 | tee $LOGFILE >> /dev/null

    }

### build_xc32_sh()
### $1 name of the directory
### $2 any extra arguments for the make
function build_xc32_sh()
    {
        mkdir -p "$1/pic32-tools/bin/bin"

        XC32_SH_SRC=${WORKING_DIR}/pic32-sh
        cd $XC32_SH_SRC/bin

        # Clean is necessary, since there isn't a separate build directory
        make clean >> /dev/null

        make xc32 MACOS_DEVELOPERDIR="${DEVELOPERDIR}" $2 2>&1 | tee $LOGFILE >> /dev/null
        assert_success $? "Error building the XC32 shell"

        # Install no need to add the /bin for call below
        export XC32_INSTALL="$1/pic32-tools"
        make pic32-compiler-install $2 2>&1 | tee $LOGFILE >> /dev/null
        assert_success $? "Error installing the XC32 shell"
        unset XC32_INSTALL

        cd $WORKING_DIR
    }

function copy_device_files()
{
    cp -R ../pic32-part-support/device-info-files/device_files pic32-tools/bin/device_files/
    cp ../pic32-part-support/device-info-files/xc32_device.info pic32-tools/bin/
    cp ../pic32-part-support/device-info-files/deviceSupport.xml pic32-tools/bin/
    cp -R ../pic32-part-support/device-info-files/device_files pic32-tools/pic32mx/device_files/
    cp ../pic32-part-support/device-info-files/xc32_device.info pic32-tools/pic32mx/
}

### Main script body


# Create the working directory

echo `date` " START PIC32 build." > $LOGFILE
echo `date` " Creating build in $WORKING_DIR..." 2>&1 | tee $LOGFILE >> /dev/null
if [ -e $WORKING_DIR ]
then
    echo `date` " $WORKING_DIR already exists..." 2>&1 | tee $LOGFILE >> /dev/null
else
    mkdir $WORKING_DIR
    assert_success $? "ERROR: creating directory $WORKING_DIR"
fi

cd $WORKING_DIR

if [ "x$NATIVEIMAGE" == "xDarwin-image" ]
then
    LINUX32IMAGE="Linux32-image"

    # Figure out which Linux compiler we have.
    if [ "x$LINUX32_HOST_PREFIX" == "x" ]; then
        LINUX_GCC=`which i586-pc-linux-gcc`
        if [ "x$LINUX_GCC" == "x" ] ; then
            LINUX32_HOST_PREFIX=i386-linux
        else
            LINUX32_HOST_PREFIX=i586-pc-linux
        fi
        unset LINUX_GCC
    fi
    BUILDMACHINE="--build=x86_64-apple-darwin18"
    DEVELOPERDIR="/Library/Developer/CommandLineTools"

    if [ "x$SKIPNATIVE" == "x" ] ; then

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
    fi  # SKIPNATIVE
    LIBHOST=""
else
    LINUX32IMAGE=""
    LINUX32_HOST_PREFIX=""
    HOSTMACHINE=""
    BUILDMACHINE="--build=x86_64-pc-linux-gnu"
    LIBHOST="--with-host-libstdcxx=-static-libgcc -Wl,-Bstatic,-lstdc++,-Bdynamic -lm"
 export CC_FOR_BUILD="/usr/bin/gcc -m32 -march=i386"
 export CXX_FOR_BUILD="/usr/bin/g++ -m32 -march=i386"
 export CPP_FOR_BUILD="/usr/bin/cpp -m32 -march=i386"
 export CC="/usr/bin/gcc -m32 -march=i386"
 export CPP="/usr/bin/cpp -m32 -march=i386"
 export CPPCXX="/usr/bin/cpp -m32 -march=i386"
 export CXX="/usr/bin/g++ -m32 -march=i386"
 export LD="/usr/bin/g++ -m32 -march=i386"
 export AR="/usr/bin/ar"


fi
cd pic32-part-support

# Install headers into cross compiler's install image directory
echo "Making library headers for cross-compiler"
echo `date` "Making library headers for cross compiler's install image..." 2>&1 | tee $LOGFILE >> /dev/null

echo "make DESTROOT=\"$WORKING_DIR/$NATIVEIMAGE/pic32-tools\" install-headers"
make DESTROOT="$WORKING_DIR/$NATIVEIMAGE/pic32-tools" install-headers 2>&1 | tee $LOGFILE >> /dev/null
assert_success $? "ERROR: Making headers into cross compiler's $NATIVEIMAGE install image directory"

if [ "x$LINUX32IMAGE" != "x" ] ; then
    echo "make DESTROOT=\"$WORKING_DIR/$LINUX32IMAGE/pic32-tools\" install-headers"
    make DESTROOT="$WORKING_DIR/$LINUX32IMAGE/pic32-tools" install-headers 2>&1 | tee $LOGFILE >> /dev/null
    assert_success $? "ERROR: Making headers into cross compiler's $LINUXIMAGE install image directory"
fi

echo "make DESTROOT=\"$WORKING_DIR/export-image/pic32-tools\" install-headers"
make DESTROOT="$WORKING_DIR/export-image/pic32-tools" install-headers 2>&1 | tee $LOGFILE >> /dev/null
assert_success $? "ERROR: Making headers into cross compiler's export-image install image directory"

echo "make DESTROOT=\"$WORKING_DIR/win32-image/pic32-tools\" install-headers"
make DESTROOT="$WORKING_DIR/win32-image/pic32-tools" install-headers 2>&1 | tee $LOGFILE >> /dev/null
assert_success $? "ERROR: Making headers into cross compiler's win32-image install image directory"

echo "make DESTROOT=\"$WORKING_DIR/arm-linux-image/pic32-tools\" install-headers"
make DESTROOT="$WORKING_DIR/arm-linux-image/pic32-tools" install-headers 2>&1 | tee $LOGFILE >> /dev/null
assert_success $? "ERROR: Making headers into cross compiler's arm-linux-image install image directory"

# Install pic32-fdlibm headers
cd $WORKING_DIR/pic32-fdlibm/src/xc32

echo "Making pic32-fdlibm headers for cross-compiler"
echo `date` "Making pic32-fdlibm library headers for cross compiler's install image..." 2>&1 | tee $LOGFILE >> /dev/null

echo "make DESTROOT=\"$WORKING_DIR/$NATIVEIMAGE/pic32-tools\" install-headers"
make DESTROOT="$WORKING_DIR/$NATIVEIMAGE/pic32-tools" install-headers 2>&1 | tee $LOGFILE >> /dev/null
assert_success $? "ERROR: Making headers into cross compiler's $NATIVEIMAGE install image directory"

if [ "x$LINUX32IMAGE" != "x" ] ; then
    echo "make DESTROOT=\"$WORKING_DIR/$LINUX32IMAGE/pic32-tools\" install-headers"
    make DESTROOT="$WORKING_DIR/$LINUX32IMAGE/pic32-tools" install-headers 2>&1 | tee $LOGFILE >> /dev/null
    assert_success $? "ERROR: Making headers into cross compiler's $LINUXIMAGE install image directory"
fi

echo "make DESTROOT=\"$WORKING_DIR/export-image/pic32-tools\" install-headers"
make DESTROOT="$WORKING_DIR/export-image/pic32-tools" install-headers 2>&1 | tee $LOGFILE >> /dev/null
assert_success $? "ERROR: Making headers into cross compiler's export-image install image directory"

echo "make DESTROOT=\"$WORKING_DIR/win32-image/pic32-tools\" install-headers"
make DESTROOT="$WORKING_DIR/win32-image/pic32-tools" install-headers 2>&1 | tee $LOGFILE >> /dev/null
assert_success $? "ERROR: Making headers into cross compiler's win32-image install image directory"

echo "make DESTROOT=\"$WORKING_DIR/arm-linux-image/pic32-tools\" install-headers"
make DESTROOT="$WORKING_DIR/arm-linux-image/pic32-tools" install-headers 2>&1 | tee $LOGFILE >> /dev/null
assert_success $? "ERROR: Making headers into cross compiler's arm-linux-image install image directory"

cd $WORKING_DIR

if [ "x$SKIPNATIVE" == "x" ] ; then

    build_xc32_sh "$WORKING_DIR/$NATIVEIMAGE"

    # Build native cross compiler
    echo `date` " Creating cross build in $WORKING_DIR/native-build..." 2>&1 | tee $LOGFILE >> /dev/null

    status_update "Beginning native pic32 build"

    if [ -e native-build ]
    then
        rm -rf native-build
    fi
    mkdir native-build
    assert_success $? "ERROR: creating directory $WORKING_DIR/native-build"

    cd native-build

    if [ -e binutils ]
    then
        rm -rf binutils
    fi
    mkdir binutils
    assert_success $? "ERROR: creating directory $WORKING_DIR/native-build/binutils"

    cd binutils

    # Configure cross binutils
    echo `date` " Configuring cross binutils build in $WORKING_DIR/native-build..." 2>&1 | tee $LOGFILE >> /dev/null
    ../../pic32-compiler/src48x/binutils/configure \
    	$HOSTMACHINE --target=pic32mx --prefix="$WORKING_DIR/$NATIVEIMAGE/pic32-tools" --bindir="$WORKING_DIR/$NATIVEIMAGE/pic32-tools/bin/bin" \
    	--libexecdir="$WORKING_DIR/$NATIVEIMAGE/pic32-tools/bin/bin" --disable-nls --disable-tui --disable-gdbtk --disable-shared --enable-static \
    	--disable-threads --disable-bootstrap --with-dwarf2 --enable-multilib --without-newlib --disable-sim --with-lib-path=: --enable-poison-system-directories \
    	--program-prefix=pic32- --disable-werror --with-bugurl=https://github.com/CyberCastle/PIC32-MCU-Compiler 2>&1 | tee $LOGFILE >> /dev/null

    assert_success $? "ERROR: configuring cross binutils build"

    # Make cross binutils and install it
    echo `date` " Making all in $WORKING_DIR/native-build/binutils and installing..." 2>&1 | tee $LOGFILE >> /dev/null
    make CFLAGS="-O2 -DMCHP_VERSION=${MCHP_VERSION}" all -j4 2>&1 | tee $LOGFILE >> /dev/null
    assert_success $? "ERROR: making/installing cross binutils build"
    make install 2>&1 | tee $LOGFILE >> /dev/null
    assert_success $? "ERROR: making/installing cross binutils build"

    NM_FOR_TARGET="$WORKING_DIR/$NATIVEIMAGE/pic32-tools/bin/pic32-nm"
    RANLIB_FOR_TARGET="$WORKING_DIR/$NATIVEIMAGE/pic32-tools/bin/pic32-ranlib"
    STRIP_FOR_TARGET="$WORKING_DIR/$NATIVEIMAGE/pic32-tools/bin/pic32-strip"
    AR_FOR_TARGET="$WORKING_DIR/$NATIVEIMAGE/pic32-tools/bin/pic32-ar"
    AS_FOR_TARGET="$WORKING_DIR/$NATIVEIMAGE/pic32-tools/pic32mx/bin/as"
    LD_FOR_TARGET="$WORKING_DIR/$NATIVEIMAGE/pic32-tools/pic32mx/bin/ld"

    cd ..

    if [ -e gmp ]
    then
        rm -rf gmp
    fi
    mkdir gmp
    assert_success $? "ERROR: creating directory $WORKING_DIR/native-build/gmp"

    cd gmp
    
    echo `date` " Configuring native gmp build in $WORKING_DIR/native-build/gmp..." 2>&1 | tee $LOGFILE >> /dev/null
    ../../pic32-compiler/src48x/gmp/configure \
    	$HOSTMACHINE $BUILDMACHINE --enable-cxx --prefix=$WORKING_DIR/native-build/host-libs --disable-shared --enable-static --disable-nls --with-gnu-ld \
    	--disable-debug --disable-rpath --enable-fft --enable-hash-synchronization  2>&1 | tee $LOGFILE >> /dev/null

    # Make native gmp and install it
    echo `date` " Making all in $WORKING_DIR/native-build/gmp and installing..." 2>&1 | tee $LOGFILE >> /dev/null
    make all -j4  2>&1 | tee $LOGFILE >> /dev/null
    assert_success $? "ERROR: making/installing gmp build"
    make install  2>&1 | tee $LOGFILE >> /dev/null
    assert_success $? "ERROR: making/installing gmp build"

    cd ..

    if [ "x$SKIPGRAPHITE" == "x" ]; then
        if [ -e ppl ]
        then
            rm -rf ppl
        fi
        mkdir ppl
        assert_success $? "ERROR: creating directory $WORKING_DIR/linux32-build/ppl"

        cd ppl
        echo `date` " Configuring native ppl build in $WORKING_DIR/native-build/ppl..." 2>&1 | tee $LOGFILE >> /dev/null
        ../../pic32-compiler/src48x/ppl/configure \
        	--prefix=$WORKING_DIR/native-build/host-libs --disable-shared --enable-static --with-gnu-ld $HOSTMACHINE --target=pic32mx --disable-nls \
        	--with-libgmp-prefix=$WORKING_DIR/native-build/host-libs --with-gmp=$WORKING_DIR/native-build/host-libs 2>&1 | tee $LOGFILE >> /dev/null

        # Make native ppl and install it
        echo `date` " Making all in $WORKING_DIR/native-build/ppl and installing..." 2>&1 | tee $LOGFILE >> /dev/null
        make all -j4 2>&1 | tee $LOGFILE >> /dev/null
        assert_success $? "ERROR: making/installing ppl build"
        make install 2>&1 | tee $LOGFILE >> /dev/null
        assert_success $? "ERROR: making/installing ppl build"

        cd ..

        USE_PPL="--with-ppl=$WORKING_DIR/native-build/host-libs --with-isl=$WORKING_DIR/native-build/host-libs"

        if [ -e cloog ]
        then
            rm -rf cloog
        fi
        mkdir cloog
        assert_success $? "ERROR: creating directory $WORKING_DIR/native-build/cloog"

        cd cloog

        echo `date` " Configuring native cloog build in $WORKING_DIR/native-build/cloog..." 2>&1 | tee $LOGFILE >> /dev/null
        ../../pic32-compiler/src48x/cloog/configure \
        	$BUILDMACHINE --enable-optimization=speed --with-gnu-ld '--with-host-libstdcxx=-static-libgcc -Wl,-Bstatic,-lstdc++,-Bdynamic -lm' \
        	--prefix=$WORKING_DIR/native-build/host-libs--with-gmp=$WORKING_DIR/native-build/host-libs --with-ppl=$WORKING_DIR/native-build/host-libs --target=pic32mx \
        	--disable-shared --enable-static --disable-shared 2>&1 | tee $LOGFILE >> /dev/null

        # Make native cloog and install it
        echo `date` " Making all in $WORKING_DIR/native-build/cloog and installing..." 2>&1 | tee $LOGFILE >> /dev/null
        make all -j4  2>&1 | tee $LOGFILE >> /dev/null
        assert_success $? "ERROR: making/installing cloog build"
        make install  2>&1 | tee $LOGFILE >> /dev/null
        assert_success $? "ERROR: making/installing cloog build"

        cd ..
        USE_CLOOG="--with-cloog=$WORKING_DIR/native-build/host-libs"
    else
        USE_CLOOG="--without-cloog"
        USE_PPL="--without-isl"
    fi


    if [ -e libelf ]
    then
        rm -rf libelf
    fi
    mkdir libelf
    assert_success $? "ERROR: creating directory $WORKING_DIR/native-build/libelf"

    cd libelf
    echo `date` " Configuring native libelf build in $WORKING_DIR/native-build/libelf..." 2>&1 | tee $LOGFILE >> /dev/null
    ../../pic32-compiler/src48x/libelf/configure \
        --prefix=$WORKING_DIR/native-build/host-libs $HOSTMACHINE --target=pic32mx --disable-shared --disable-debug --disable-nls 2>&1 | tee $LOGFILE >> /dev/null

    # Make native libelf and install it
    echo `date` " Making all in $WORKING_DIR/native-build/libelf and installing..." 2>&1 | tee $LOGFILE >> /dev/null
    make all -j4 2>&1 | tee $LOGFILE >> /dev/null
    assert_success $? "ERROR: making/installing libelf build"
    make install 2>&1 | tee $LOGFILE >> /dev/null
    assert_success $? "ERROR: making/installing libelf build"
    cd ..

    if [ -e zlib ]
    then
        rm -rf zlib
    fi
    cp -r ../pic32-compiler/src48x/zlib .

    assert_success $? "ERROR: copy src48x/zlib directory to $WORKING_DIR/native-build/zlib"

    cd zlib
    echo `date` " Configuring native zlib build in $WORKING_DIR/native-build/zlib..." 2>&1 | tee $LOGFILE >> /dev/null
    ./configure --prefix=$WORKING_DIR/native-build/host-libs 2>&1 | tee $LOGFILE >> /dev/null

    # Make native zlib and install it
    echo `date` " Making all in $WORKING_DIR/native-build/zlib and installing..." 2>&1 | tee $LOGFILE >> /dev/null
    make all -j4 2>&1 | tee $LOGFILE >> /dev/null
    assert_success $? "ERROR: making/installing zlib build"
    make install 2>&1 | tee $LOGFILE >> /dev/null
    assert_success $? "ERROR: making/installing zlib build"

    cd ..

    if [ -e gcc ]
    then
        rm -rf gcc
    fi
    mkdir gcc
    assert_success $? "ERROR: creating directory $WORKING_DIR/native-build/gcc"

    cd gcc

    # Configure cross compiler
    echo `date` " Configuring cross compiler build in $WORKING_DIR/native-build..." 2>&1 | tee $LOGFILE >> /dev/null
    echo AR_FOR_TARGET="$WORKING_DIR/$NATIVEIMAGE/pic32-tools/pic32mx/bin/ar" \
	AS_FOR_TARGET="$WORKING_DIR/$NATIVEIMAGE/pic32-tools/pic32mx/bin/as" \
	LD_FOR_TARGET="$WORKING_DIR/$NATIVEIMAGE/pic32-tools/pic32mx/bin/ld" \
	../../pic32-compiler/src48x/gcc/configure \
    	--target=pic32mx --program-prefix=pic32- --disable-threads --disable-libmudflap --disable-libssp --enable-sgxx-sde-multilibs --with-gnu-as --with-gnu-ld \
    	--enable-languages=c,c++ --disable-shared --enable-static --with-newlib --disable-nls --disable-libgomp --without-headers --disable-libffi --disable-bootstrap \
    	--disable-decimal-float --disable-libquadmath --disable-__cxa_atexit --disable-libfortran --disable-libstdcxx-pch --prefix="$WORKING_DIR/$NATIVEIMAGE/pic32-tools" \
    	--bindir="$WORKING_DIR/$NATIVEIMAGE/pic32-tools/bin/bin" --libexecdir="$WORKING_DIR/$NATIVEIMAGE/pic32-tools/bin/bin" --with-dwarf2 \
    	--with-gmp="$WORKING_DIR/native-build/host-libs" $USE_CLOOG $USE_PPL "$LIBHOST" --enable-lto --enable-fixed-point \
    	XGCC_FLAGS_FOR_TARGET="-fno-rtti \-fno-enforce-eh-specs" --enable-cxx-flags="-fno-exceptions -ffunction-sections" $SUPPORT_SJLJ_EXCEPTIONS --enable-obsolete \
    	--disable-sim --disable-checking $SUPPORT_HOSTED_LIBSTDCXX --with-bugurl=https://github.com/CyberCastle/PIC32-MCU-Compiler 2>&1 | tee $LOGFILE >> /dev/null

    AR_FOR_TARGET="$WORKING_DIR/$NATIVEIMAGE/pic32-tools/pic32mx/bin/ar" \
    AS_FOR_TARGET="$WORKING_DIR/$NATIVEIMAGE/pic32-tools/pic32mx/bin/as" \
    LD_FOR_TARGET="$WORKING_DIR/$NATIVEIMAGE/pic32-tools/pic32mx/bin/ld" \
    ../../pic32-compiler/src48x/gcc/configure \
		--target=pic32mx --program-prefix=pic32- --disable-threads --disable-libmudflap --disable-libssp --enable-sgxx-sde-multilibs --with-gnu-as --with-gnu-ld \
		--enable-languages=c,c++ --disable-shared --enable-static --with-newlib --disable-nls --disable-libgomp --without-headers --disable-libffi --disable-bootstrap \
		--disable-decimal-float --disable-libquadmath --disable-__cxa_atexit --disable-libfortran --disable-libstdcxx-pch --prefix="$WORKING_DIR/$NATIVEIMAGE/pic32-tools" \
		--bindir="$WORKING_DIR/$NATIVEIMAGE/pic32-tools/bin/bin" --libexecdir="$WORKING_DIR/$NATIVEIMAGE/pic32-tools/bin/bin" --with-dwarf2 \
		--with-gmp="$WORKING_DIR/native-build/host-libs" $USE_CLOOG $USE_PPL "$LIBHOST" --enable-lto --enable-fixed-point  \
		XGCC_FLAGS_FOR_TARGET="-frtti -fexceptions -fno-enforce-eh-specs" CXXFLAGS="-g3" $SUPPORT_SJLJ_EXCEPTIONS --enable-obsolete --disable-sim \
		--disable-checking $SUPPORT_HOSTED_LIBSTDCXX --with-bugurl=https://github.com/CyberCastle/PIC32-MCU-Compiler 2>&1 | tee $LOGFILE >> /dev/null

    assert_success $? "ERROR: configuring cross build"

    # Make cross compiler and install it
    echo `date` " Making all in $WORKING_DIR/native-build/gcc and installing..." 2>&1 | tee $LOGFILE >> /dev/null
    make all-gcc CFLAGS="-O2" CXXFLAGS="-g3 -DTARGET_IS_PIC32MX" \
	    NM_FOR_TARGET="$WORKING_DIR/$NATIVEIMAGE/pic32-tools/bin/pic32-nm" \
	    RANLIB_FOR_TARGET="$WORKING_DIR/$NATIVEIMAGE/pic32-tools/bin/pic32-ranlib" \
	    STRIP_FOR_TARGET="$WORKING_DIR/$NATIVEIMAGE/pic32-tools/bin/pic32-strip"  \
	    AR_FOR_TARGET="$WORKING_DIR/$NATIVEIMAGE/pic32-tools/bin/pic32-ar"  \
	    AS_FOR_TARGET="$WORKING_DIR/$NATIVEIMAGE/pic32-tools/pic32mx/bin/as" \
	    LD_FOR_TARGET="$WORKING_DIR/$NATIVEIMAGE/pic32-tools/pic32mx/bin/ld" -j2 2>&1 | tee $LOGFILE >> /dev/null
    assert_success $? "ERROR: making/installing cross build all-gcc"

    make CFLAGS="-O2 -DTARGET_IS_PIC32MX" CXXFLAGS="-O2 -DTARGET_IS_PIC32MX" install-gcc 2>&1 | tee $LOGFILE >> /dev/null
    assert_success $? "ERROR: making/installing cross build install-gcc"

    cd ..

    if [ -e newlib ]
    then
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
    	$NEWLIB_CONFIGURE_FLAGS  --prefix=$WORKING_DIR/$NATIVEIMAGE/pic32-tools --bindir="$WORKING_DIR/$NATIVEIMAGE/pic32-tools/bin/bin" \
    	--libexecdir="$WORKING_DIR/$NATIVEIMAGE/pic32-tools/bin/bin" CFLAGS_FOR_TARGET="-fno-short-double -fno-rtti -fno-exceptions -fomit-frame-pointer \
    	-DPREFER_SIZE_OVER_SPEED -D__OPTIMIZE_SIZE__ -Os -fshort-wchar -fno-unroll-loops -fno-enforce-eh-specs -ffunction-sections -fdata-sections -DSMALL_MEMORY \
    	-D_NO_GETLOGIN -D_NO_GETPWENT -D_NO_GETUT -D_NO_GETPASS -D_NO_SIGSET" CCASFLAGS_FOR_TARGET="-fno-short-double -fno-rtti -fno-exceptions -fomit-frame-pointer \
    	-DPREFER_SIZE_OVER_SPEED -D__OPTIMIZE_SIZE__ -Os -fshort-wchar -fno-unroll-loops -fno-enforce-eh-specs -ffunction-sections -fdata-sections -DSMALL_MEMORY \
    	-D_NO_GETLOGIN -D_NO_GETPWENT -D_NO_GETUT -D_NO_GETPASS -D_NO_SIGSET" XGCC_FLAGS_FOR_TARGET="-fno-short-double -fno-rtti -fno-exceptions -fomit-frame-pointer \
    	-DPREFER_SIZE_OVER_SPEED -D__OPTIMIZE_SIZE__ -Os -fshort-wchar -fno-unroll-loops -fno-enforce-eh-specs -ffunction-sections -fdata-sections" \
    	--enable-cxx-flags="-fno-short-double -fno-rtti -fno-exceptions -fomit-frame-pointer -DPREFER_SIZE_OVER_SPEED -D__OPTIMIZE_SIZE__ -Os -fshort-wchar -fno-unroll-loops \
    	-fno-enforce-eh-specs -ffunction-sections -fdata-sections" 2>&1 | tee $LOGFILE >> /dev/null
    assert_success $? "ERROR: Configure Newlib for native build"

    make all -j2 CFLAGS_FOR_TARGET="-fno-short-double -fno-rtti -fno-exceptions -fomit-frame-pointer -DPREFER_SIZE_OVER_SPEED -D__OPTIMIZE_SIZE__ -Os -fshort-wchar \
	    -fno-unroll-loops -fno-enforce-eh-specs -ffunction-sections -fdata-sections -DSMALL_MEMORY -D_NO_GETLOGIN -D_NO_GETPWENT -D_NO_GETUT -D_NO_GETPASS -D_NO_SIGSET" \
	    CCASFLAGS_FOR_TARGET="-fno-short-double -fno-rtti -fno-exceptions -fomit-frame-pointer -DPREFER_SIZE_OVER_SPEED -D__OPTIMIZE_SIZE__ -Os -fshort-wchar -fno-unroll-loops \
	    -fno-enforce-eh-specs -ffunction-sections -fdata-sections -DSMALL_MEMORY -D_NO_GETLOGIN -D_NO_GETPWENT -D_NO_GETUT -D_NO_GETPASS -D_NO_SIGSET" \
	    XGCC_FLAGS_FOR_TARGET="-fno-short-double -fno-rtti -fno-exceptions -fomit-frame-pointer -DPREFER_SIZE_OVER_SPEED -D__OPTIMIZE_SIZE__ -Os -fshort-wchar -fno-unroll-loops \
	    -fno-enforce-eh-specs -ffunction-sections -fdata-sections" 2>&1 | tee $LOGFILE >> /dev/null
    assert_success $? "ERROR: Make newlib for native build"

    make install 2>&1 | tee $LOGFILE >> /dev/null
    assert_success $? "ERROR: Install newlib for native build"

    cd $WORKING_DIR
    if [ "x$NATIVEIMAGE" != "x" ]
    then
      rsync -qavzC --include "*/" --include "*" export-image/pic32-tools/ $NATIVEIMAGE/pic32-tools/
      assert_success $? "ERROR: Install newlib in $NATIVEIMAGE"
    fi
    if [ "x$INUX32IMAGE" != "x" ]
    then
      rsync -qavzC --include "*/" --include "*" export-image/pic32-tools/ $LINUX32IMAGE/pic32-tools/
      assert_success $? "ERROR: Install newlib in $LINUX32IMAGE"
    fi
    if [ -e win32-image ]
    then
      rsync -qavzC --include "*/" --include "*" export-image/pic32-tools/ win32-image/pic32-tools/
      assert_success $? "ERROR: Install newlib in win32-image"
    fi
    if [ -e arm-linux-image ]
    then
      rsync -qavzC --include "*/" --include "*" export-image/pic32-tools/ arm-linux-image/pic32-tools/
      assert_success $? "ERROR: Install newlib in arm-linux-image"
    fi
    cd native-build

    if [ -e gcc ]
    then
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
    	CFLAGS_FOR_BUILD="-Os" 2>&1 | tee $LOGFILE >> /dev/null
    assert_success $? "ERROR: Configure gcc after Newlib for native build"

    make all \
    CXXFLAGS="$CXXFLAGS_FOR_TARGET -Os" \
    NM_FOR_TARGET="$WORKING_DIR/$NATIVEIMAGE/pic32-tools/bin/pic32-nm" \
    RANLIB_FOR_TARGET="$WORKING_DIR/$NATIVEIMAGE/pic32-tools/bin/pic32-ranlib" \
    STRIP_FOR_TARGET="$WORKING_DIR/$NATIVEIMAGE/pic32-tools/bin/pic32-strip"  \
    AR_FOR_TARGET="$WORKING_DIR/$NATIVEIMAGE/pic32-tools/bin/pic32-ar"  \
    AS_FOR_TARGET="$WORKING_DIR/$NATIVEIMAGE/pic32-tools/pic32mx/bin/as" \
    LD_FOR_TARGET="$WORKING_DIR/$NATIVEIMAGE/pic32-tools/pic32mx/bin/ld" -j2 2>&1 | tee $LOGFILE >> /dev/null
    assert_success $? "ERROR: making/installing cross build all"

    make install 2>&1 | tee $LOGFILE >> /dev/null
    assert_success $? "ERROR: making/installing cross build install"

    cd ..
    if [ -e gdb ]
    then
        rm -rf gdb
    fi
    mkdir gdb
    assert_success $? "ERROR: creating directory $WORKING_DIR/native-build/gdb"

    cd gdb
    echo `date` " Configuring gdb in $WORKING_DIR/native-build..." 2>&1 | tee $LOGFILE >> /dev/null
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
        --disable-gdbtk 2>&1 | tee $LOGFILE >> /dev/null
    assert_success $? "ERROR: Configure gdb for native build"

    echo `date` " Building gdb in $WORKING_DIR/native-build..." 2>&1 | tee $LOGFILE >> /dev/null
    make  2>&1 | tee $LOGFILE >> /dev/null
    assert_success $? "ERROR: making gdb for native build"
    make install  2>&1 | tee $LOGFILE >> /dev/null
    assert_success $? "ERROR: installing gdb for native build"

    status_update "Cross build complete"

    cd ../..

    # strip native-image
    cd $NATIVEIMAGE/pic32-tools
    if [ "x$NATIVEIMAGE" == "xDarwin-image" ] ; then
        find . -type f -perm -g+x -follow | xargs file | grep Mach-O | cut -d: -f1 | xargs $DEVELOPERDIR/usr/bin/strip
    elif [ "x$LINUX32_HOST_PREFIX" != "x" ] ; then
        find . -type f -perm -g+x -follow | xargs file | grep ELF | cut -d: -f1 | xargs $LINUX32_HOST_PREFIX-strip
    fi
    cd $WORKING_DIR
    # end strip native-image

fi # skipping native


# end build native toolchain

unset  CC
unset  CPP
unset  CXX
unset  LD
unset  AR

# Set up path so that we can build the libraries and the win32 cross
# compiler using the cross compiler we just built
OLDPATH=$PATH
PATH=$WORKING_DIR/$NATIVEIMAGE/pic32-tools/bin:$PATH

if [ "x$SKIPLIBS" == "x" ] ; then

    cd pic32-part-support

    # Build cross compiler libraries
    echo `date` " Making and installing cross-compiler libraries to $WORKING_DIR/$NATIVEIMAGE/pic32-tools..." 2>&1 | tee $LOGFILE >> /dev/null

    make DESTROOT=$WORKING_DIR/$NATIVEIMAGE/pic32-tools all  2>&1 | tee $LOGFILE >> /dev/null
    assert_success $? "ERROR: making libraries for cross build"

    make DESTROOT=$WORKING_DIR/$NATIVEIMAGE/pic32-tools install -j2  2>&1 | tee $LOGFILE >> /dev/null
    assert_success $? "ERROR: making libraries for $NATIVEIMAGE cross build"

    if [ "x$LINUX32IMAGE" != "x" ] ; then
        make DESTROOT="$WORKING_DIR/$LINUX32IMAGE/pic32-tools" install -j2  2>&1 | tee $LOGFILE >> /dev/null
        assert_success $? "ERROR: making libraries for linux32-image cross build"
    fi

    make DESTROOT="$WORKING_DIR/export-image/pic32-tools" install -j2  2>&1 | tee $LOGFILE >> /dev/null
    assert_success $? "ERROR: making libraries for export-image cross build"
    make DESTROOT="$WORKING_DIR/win32-image/pic32-tools" install -j2  2>&1 | tee $LOGFILE >> /dev/null
    assert_success $? "ERROR: making libraries for win32-image cross build"
    make DESTROOT="$WORKING_DIR/arm-linux-image/pic32-tools" install -j2  2>&1 | tee $LOGFILE >> /dev/null
    assert_success $? "ERROR: making libraries for arm-linux-image cross build"

    status_update "cross-compiler library build complete"

    cd ..

    # Build and install pic32-fdlibm
    cd $WORKING_DIR/pic32-fdlibm/src/xc32

    # Build pic32-fdlibm once
    echo `date` " Making and installing cross-compiler pic32-fdlibm libraries to $WORKING_DIR/$NATIVEIMAGE..." 2>&1 | tee $LOGFILE >> /dev/null
    make DESTROOT=$WORKING_DIR/$NATIVEIMAGE/pic32-tools all 2>&1 | tee $LOGFILE >> /dev/null
    assert_success $? "ERROR: making pic32-fdlibm  libraries for cross build"

    # Then install
    make DESTROOT=$WORKING_DIR/$NATIVEIMAGE/pic32-tools install -j2 2>&1 | tee $LOGFILE >> /dev/null
    assert_success $? "ERROR: making installing pic32-fdlibm for $NATIVEIMAGE cross build"

    if [ "x$LINUX32IMAGE" != "x" ] ; then
        make DESTROOT="$WORKING_DIR/$LINUX32IMAGE/pic32-tools" install -j2 2>&1 | tee $LOGFILE >> /dev/null
        assert_success $? "ERROR: making pic32-fdlibm libraries for Linux32-image cross build"
    fi
    make DESTROOT="$WORKING_DIR/export-image/pic32-tools" install -j2 2>&1 | tee $LOGFILE >> /dev/null
    assert_success $? "ERROR: making pic32-fdlibm libraries for export-image cross build"

    make DESTROOT="$WORKING_DIR/win32-image/pic32-tools" install -j2 2>&1 | tee $LOGFILE >> /dev/null
    assert_success $? "ERROR: making pic32-fdlibm libraries for win32-image cross build"

    make DESTROOT="$WORKING_DIR/arm-linux-image/pic32-tools" install -j2 2>&1 | tee $LOGFILE >> /dev/null
    assert_success $? "ERROR: making pic32-fdlibm libraries for arm-linux-image cross build"

    status_update "cross-compiler pic32-fdlibm library build complete"

fi # skip library build

# Build linux compiler

if [ "x$SKIPLINUX32" == "x" ] ; then

    if [ "x$LINUX32IMAGE" != "x" ] ; then
        
        unset CC
        unset CPP
        unset CXX

        build_xc32_sh "$WORKING_DIR/$LINUX32IMAGE" "TARGET=linux"

        echo `date` " Creating linux cross build in $WORKING_DIR/linux32-build..." 2>&1 | tee $LOGFILE >> /dev/null
        cd $WORKING_DIR
        if [ -e linux32-build ]
        then
            rm -rf linux32-build
        fi
        mkdir linux32-build
        assert_success $? "ERROR: creating directory $WORKING_DIR/linux32-build"

        cd linux32-build

        if [ -e binutils ]
        then
            rm -rf binutils
        fi
        mkdir binutils
        assert_success $? "ERROR: creating directory $WORKING_DIR/linux32-build/binutils"

        cd binutils

        # Configure linux-cross binutils
        echo `date` " Configuring linux32 binutils build in $WORKING_DIR/linux32-build..." 2>&1 | tee $LOGFILE >> /dev/null
        ../../pic32-compiler/src48x/binutils/configure \
        	$BUILDMACHINE --target=pic32mx --prefix="$WORKING_DIR/$LINUX32IMAGE/pic32-tools" --bindir="$WORKING_DIR/$LINUX32IMAGE/pic32-tools/bin/bin" \
        	--libexecdir="$WORKING_DIR/$LINUX32IMAGE/pic32-tools/bin/bin" --host=$LINUX32_HOST_PREFIX --disable-nls --disable-tui --disable-gdbtk --disable-shared \
        	--enable-static --disable-threads --disable-bootstrap  --with-dwarf2 --enable-multilib --without-newlib --disable-sim --with-lib-path=: \
        	--enable-poison-system-directories --program-prefix=pic32- --with-bugurl=https://github.com/CyberCastle/PIC32-MCU-Compiler --disable-werror 2>&1 | tee $LOGFILE >> /dev/null
        assert_success $? "ERROR: configuring linux32 binutils build"

        # Make linux-cross binutils and install it
        echo `date` " Making all in $WORKING_DIR/linux32-build/binutils and installing..." 2>&1 | tee $LOGFILE >> /dev/null
        make all CFLAGS="-O2 -DMCHP_VERSION=${MCHP_VERSION}" -j4 2>&1 | tee $LOGFILE >> /dev/null
        assert_success $? "ERROR: making/installing linux32 Canadian-cross binutils build"
        make CFLAGS="-O2" install 2>&1 | tee $LOGFILE >> /dev/null
        assert_success $? "ERROR: making/installing linux32 Canadian-cross binutils build"

        cd ..

        if [ -e gmp ]
        then
            rm -rf gmp
        fi
        mkdir gmp
        assert_success $? "ERROR: creating directory $WORKING_DIR/linux32-build/gmp"

        cd gmp

        echo `date` " Configuring linux gmp build in $WORKING_DIR/linux32-build/gmp..." 2>&1 | tee $LOGFILE >> /dev/null
        CFLAGS="-fexceptions" ../../pic32-compiler/src48x/gmp/configure \
        	--enable-cxx  --prefix=$WORKING_DIR/linux32-build/linux-libs --disable-shared --target=$LINUX32_HOST_PREFIX --host=$LINUX32_HOST_PREFIX --disable-nls \
        	--with-gnu-ld --disable-debug --disable-rpath --enable-fft \
            --enable-hash-synchronization "--with-host-libstdcxx=-static-libgcc -Wl,-Bstatic,-lstdc++,-Bdynamic -lm" 2>&1 | tee $LOGFILE >> /dev/null

        # Make linux gmp and install it
        echo `date` " Making all in $WORKING_DIR/linux32-build/gmp and installing..." 2>&1 | tee $LOGFILE >> /dev/null
        make all -j2 2>&1 | tee $LOGFILE >> /dev/null
        assert_success $? "ERROR: making/installing gmp build"
        make install 2>&1 | tee $LOGFILE >> /dev/null
        assert_success $? "ERROR: making/installing gmp build"

        cd ..

        if [ "x$SKIPGRAPHITE" == "x" ]; then
            if [ -e ppl ]
            then
                rm -rf ppl
            fi
            mkdir ppl
            assert_success $? "ERROR: creating directory $WORKING_DIR/linux32-build/ppl"

            cd ppl
            echo `date` " Configuring linux32 ppl build in $WORKING_DIR/linux32-build/ppl..." 2>&1 | tee $LOGFILE >> /dev/null
            ../../pic32-compiler/src48x/ppl/configure \
            	--prefix=$WORKING_DIR/linux32-build/linux-libs --disable-shared --enable-static --with-gnu-ld --host=$LINUX32_HOST_PREFIX --target=pic32mx --disable-nls \
            	--with-libgmp-prefix=$WORKING_DIR/linux32-build/linux-libs --with-gmp=$WORKING_DIR/linux32-build/linux-libs 2>&1 | tee $LOGFILE >> /dev/null

            # Make native ppl and install it
            echo `date` " Making all in $WORKING_DIR/linux32-build/ppl and installing..." 2>&1 | tee $LOGFILE >> /dev/null
            make all -j2 2>&1 | tee $LOGFILE >> /dev/null
            assert_success $? "ERROR: making/installing ppl build"
            make install 2>&1 | tee $LOGFILE >> /dev/null
            assert_success $? "ERROR: making/installing ppl build"

            cd ..

            if [ -e cloog ]
            then
                rm -rf cloog
            fi
            mkdir cloog
            assert_success $? "ERROR: creating directory $WORKING_DIR/linux32-build/cloog"

            cd cloog

            echo `date` " Configuring linux cloog build in $WORKING_DIR/linux32-build/cloog..." 2>&1 | tee $LOGFILE >> /dev/null
            ../../pic32-compiler/src48x/cloog/configure \
            	$BUILDMACHINE --enable-optimization=speed --with-gnu-ld '--with-host-libstdcxx=-static-libgcc -Wl,-Bstatic,-lstdc++,-Bdynamic -lm' \
            	--prefix=$WORKING_DIR/linux32-build/linux-libs --host=$LINUX32_HOST_PREFIX --with-gmp=$WORKING_DIR/linux32-build/linux-libs \
            	--with-ppl=$WORKING_DIR/linux32-build/linux-libs --target=pic32mx --disable-shared --enable-static --disable-shared 2>&1 | tee $LOGFILE >> /dev/null

            # Make native cloog and install it
            echo `date` " Making all in $WORKING_DIR/linux32-build/cloog and installing..." 2>&1 | tee $LOGFILE >> /dev/null
            make all -j2 2>&1 | tee $LOGFILE >> /dev/null
            assert_success $? "ERROR: making/installing cloog build"
            make install 2>&1 | tee $LOGFILE >> /dev/null
            assert_success $? "ERROR: making/installing cloog build"

            cd ..
        else
            USE_CLOOG="--without-cloog"
            USE_PPL="--without-isl"
        fi

        if [ -e libelf ]
        then
            rm -rf libelf
        fi
        mkdir libelf
        assert_success $? "ERROR: creating directory $WORKING_DIR/linux32-build/libelf"

        cd libelf
        echo `date` " Configuring native libelf build in $WORKING_DIR/linux32-build/libelf..." 2>&1 | tee $LOGFILE >> /dev/null
        ../../pic32-compiler/src48x/libelf/configure  \
        	--prefix=$WORKING_DIR/linux32-build/linux-libs --host=$LINUX32_HOST_PREFIX --target=pic32mx --disable-shared --disable-debug --disable-nls 2>&1 | tee $LOGFILE >> /dev/null

        # Make native libelf and install it
        echo `date` " Making all in $WORKING_DIR/linux32-build/libelf and installing..." 2>&1 | tee $LOGFILE >> /dev/null
        make all -j2 2>&1 | tee $LOGFILE >> /dev/null
        assert_success $? "ERROR: making/installing libelf build"
        make install 2>&1 | tee $LOGFILE >> /dev/null
        assert_success $? "ERROR: making/installing libelf build"
        cd ..

        if [ -e zlib ]
        then
            rm -rf zlib
        fi
        cp -r ../pic32-compiler/src48x/zlib .
        assert_success $? "ERROR: copy src48x/zlib directory to $WORKING_DIR/linux32-build/zlib"

        cd zlib
        echo `date` " Configuring linux zlib build in $WORKING_DIR/linux32-build/zlib..." 2>&1 | tee $LOGFILE >> /dev/null
        CC=$LINUX32_HOST_PREFIX-gcc AR="$LINUX32_HOST_PREFIX-ar" RANLIB=$LINUX32_HOST_PREFIX-ranlib \
        ./configure --prefix=$WORKING_DIR/linux32-build/linux-libs 2>&1 | tee $LOGFILE >> /dev/null

        # Make linux zlib and install it
        echo `date` " Making all in $WORKING_DIR/linux32-build/zlib and installing..." 2>&1 | tee $LOGFILE >> /dev/null
        make all -j2 2>&1 | tee $LOGFILE >> /dev/null
        assert_success $? "ERROR: making/installing zlib build - all"
        make install 2>&1 | tee $LOGFILE >> /dev/null
        assert_success $? "ERROR: making/installing zlib build - install"
        cd ..

        if [ -e gcc ]
        then
            rm -rf gcc
        fi
        mkdir gcc
        assert_success $? "ERROR: creating directory $WORKING_DIR/linux32-build/gcc"

        cd gcc

        # Configure linux cross compiler
        echo `date` " Configuring linux cross compiler build in $WORKING_DIR/linux32-build..." 2>&1 | tee $LOGFILE >> /dev/null

        AR_FOR_TARGET="$WORKING_DIR/$NATIVEIMAGE/pic32-tools/pic32mx/bin/ar" \
        AS_FOR_TARGET="$WORKING_DIR/$NATIVEIMAGE/pic32-tools/pic32mx/bin/as" \
        LD_FOR_TARGET="$WORKING_DIR/$NATIVEIMAGE/pic32-tools/pic32mx/bin/ld" \
        GCC_FOR_TARGET="$WORKING_DIR/$NATIVEIMAGE/pic32-tools/bin/pic32-gcc" \
        CC_FOR_TARGET="$WORKING_DIR/$NATIVEIMAGE/pic32-tools/bin/pic32-gcc" \
        CXX_FOR_TARGET='pic32-gcc' target_alias=pic32- \
        ../../pic32-compiler/src48x/gcc/configure \
        	$GCC_CONFIGURE_FLAGS $BUILDMACHINE --prefix="$WORKING_DIR/$LINUX32IMAGE/pic32-tools" --bindir="$WORKING_DIR/$LINUX32IMAGE/pic32-tools/bin/bin" \
        	--libexecdir="$WORKING_DIR/$LINUX32IMAGE/pic32-tools/bin/bin" --host=$LINUX32_HOST_PREFIX "--with-host-libstdcxx=-static-libgcc -Wl,-Bstatic,-lstdc++,-Bdynamic -lm" \
        	--with-libelf=$WORKING_DIR/linux32-build/linux-libs --with-gmp=$WORKING_DIR/linux32-build/linux-libs $USE_CLOOG $USE_PPL \
        	CFLAGS_FOR_TARGET="-fno-rtti -fno-exceptions -fomit-frame-pointer -DPREFER_SIZE_OVER_SPEED -D__OPTIMIZE_SIZE__ -Os -fshort-wchar -fno-unroll-loops \
        	-fno-enforce-eh-specs -ffunction-sections -fdata-sections" XGCC_FLAGS_FOR_TARGET="-fno-rtti -fno-exceptions -fomit-frame-pointer -DPREFER_SIZE_OVER_SPEED \
        	-D__OPTIMIZE_SIZE__ -Os -fshort-wchar -fno-unroll-loops -fno-enforce-eh-specs -ffunction-sections -fdata-sections" --enable-cxx-flags="-fno-rtti -fno-exceptions \
        	-fomit-frame-pointer -DPREFER_SIZE_OVER_SPEED -D__OPTIMIZE_SIZE__ -Os -fshort-wchar -fno-unroll-loops -fno-enforce-eh-specs -ffunction-sections -fdata-sections" \
        	$SUPPORT_HOSTED_LIBSTDCXX 2>&1 | tee $LOGFILE >> /dev/null
        assert_success $? "ERROR: configuring linux32 cross build"

        # Make cross compiler and install it
        echo `date` " Making all in $WORKING_DIR/linux32-build/gcc and installing..." 2>&1 | tee $LOGFILE >> /dev/null
        make CFLAGS="-O2" CXXFLAGS="-O2" all-gcc \
	        NM_FOR_TARGET="$WORKING_DIR/$NATIVEIMAGE/pic32-tools/bin/pic32-nm" \
	        RANLIB_FOR_TARGET="$WORKING_DIR/$NATIVEIMAGE/pic32-tools/bin/pic32-ranlib" \
	        STRIP_FOR_TARGET="$WORKING_DIR/$NATIVEIMAGE/pic32-tools/bin/pic32-strip" \
	        AR_FOR_TARGET="$WORKING_DIR/$NATIVEIMAGE/pic32-tools/bin/pic32-ar" \
	        AS_FOR_TARGET="$WORKING_DIR/$NATIVEIMAGE/pic32-tools/pic32mx/bin/as" \
	        LD_FOR_TARGET="$WORKING_DIR/$NATIVEIMAGE/pic32-tools/pic32mx/bin/ld" \
	        GCC_FOR_TARGET="$WORKING_DIR/$NATIVEIMAGE/pic32-tools/bin/pic32-gcc" \
	        CC_FOR_TARGET="$WORKING_DIR/$NATIVEIMAGE/pic32-tools/bin/pic32-gcc" -j4 2>&1 | tee $LOGFILE >> /dev/null
        assert_success $? "ERROR: making/installing linux Canadian-cross compiler build"

        make CFLAGS="-O2" CXXFLAGS="-O2" install-gcc 2>&1 | tee $LOGFILE >> /dev/null
        assert_success $? "ERROR: making/installing linux Canadian-cross compiler build"

        cd ..

        if [ "xSKIPMULTIPLENEWLIB" != x ]; then

        if [ -e newlib ]
        then
            rm -rf newlib
        fi
        mkdir newlib
        assert_success $? "ERROR: creating directory $WORKING_DIR/LINUXIMAGE/newlib"

        cd newlib

        echo `date` " Configure newlib for $LINUX32IMAGE..." 2>&1 | tee $LOGFILE >> /dev/null

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
    	../../pic32-newlib/configure  \
    	$NEWLIB_CONFIGURE_FLAGS --prefix=$WORKING_DIR/$LINUX32IMAGE/pic32-tools --bindir="$WORKING_DIR/LINUX32IMAGE/pic32-tools/bin/bin" \
    	--libexecdir="$WORKING_DIR/$LINUX32IMAGE/pic32-tools/bin/bin" CFLAGS_FOR_TARGET="-fno-rtti -fno-exceptions -fomit-frame-pointer -DPREFER_SIZE_OVER_SPEED \
    	-D__OPTIMIZE_SIZE__ -Os -fshort-wchar -fno-unroll-loops -fno-enforce-eh-specs -ffunction-sections -fdata-sections -DSMALL_MEMORY -D_NO_GETLOGIN -D_NO_GETPWENT \
    	-D_NO_GETUT -D_NO_GETPASS -D_NO_SIGSET" CCASFLAGS_FOR_TARGET="-fno-rtti -fno-exceptions -fomit-frame-pointer -DPREFER_SIZE_OVER_SPEED -D__OPTIMIZE_SIZE__ -Os \
    	-fshort-wchar -fno-unroll-loops -fno-enforce-eh-specs -ffunction-sections -fdata-sections -DSMALL_MEMORY -D_NO_GETLOGIN -D_NO_GETPWENT -D_NO_GETUT -D_NO_GETPASS \
    	-D_NO_SIGSET" XGCC_FLAGS_FOR_TARGET="-fno-rtti -fno-exceptions -fomit-frame-pointer -DPREFER_SIZE_OVER_SPEED -D__OPTIMIZE_SIZE__ -Os -fshort-wchar -fno-unroll-loops \
    	-fno-enforce-eh-specs -ffunction-sections -fdata-sections" --enable-cxx-flags="-fno-rtti -fno-exceptions -fomit-frame-pointer -DPREFER_SIZE_OVER_SPEED \
    	-D__OPTIMIZE_SIZE__ -Os -fshort-wchar -fno-unroll-loops -fno-enforce-eh-specs -ffunction-sections -fdata-sections" 2>&1 | tee $LOGFILE >> /dev/null

        echo `date` " Make newlib for $LINUX32IMAGE..." 2>&1 | tee $LOGFILE >> /dev/null
        make all -j4 CFLAGS_FOR_TARGET="-fno-rtti -fno-exceptions -fomit-frame-pointer -DPREFER_SIZE_OVER_SPEED -D__OPTIMIZE_SIZE__ -Os -fshort-wchar \
	        -fno-unroll-loops -fno-enforce-eh-specs -ffunction-sections -fdata-sections -DSMALL_MEMORY -D_NO_GETLOGIN -D_NO_GETPWENT -D_NO_GETUT -D_NO_GETPASS \
	        -D_NO_SIGSET" CCASFLAGS_FOR_TARGET="-fno-rtti -fno-exceptions -fomit-frame-pointer -DPREFER_SIZE_OVER_SPEED -D__OPTIMIZE_SIZE__ -Os -fshort-wchar -fno-unroll-loops \
	        -fno-enforce-eh-specs -ffunction-sections -fdata-sections -DSMALL_MEMORY -D_NO_GETLOGIN -D_NO_GETPWENT -D_NO_GETUT -D_NO_GETPASS -D_NO_SIGSET" \
	        XGCC_FLAGS_FOR_TARGET="-fno-rtti -fno-exceptions -fomit-frame-pointer -DPREFER_SIZE_OVER_SPEED -D__OPTIMIZE_SIZE__ -Os -fshort-wchar -fno-unroll-loops \
	        -fno-enforce-eh-specs -ffunction-sections -fdata-sections" 2>&1 | tee $LOGFILE >> /dev/null
        assert_success $? "ERROR: Make newlib for cross build"
        
        make install 2>&1 | tee $LOGFILE >> /dev/null
        assert_success $? "ERROR: Install newlib for cross build"

        cd ..
        fi

        if [ -e gcc ]
        then
            rm -rf gcc
        fi
        mkdir gcc
        assert_success $? "ERROR: creating directory $WORKING_DIR/linux32-build/gcc"

        cd gcc

        echo `date` " Configure gcc after making Newlib for $LINUX32IMAGE..." 2>&1 | tee $LOGFILE >> /dev/null
        GCC_FOR_TARGET=$WORKING_DIR/$NATIVEIMAGE/pic32-tools/bin/pic32-gcc \
        CC_FOR_TARGET="$WORKING_DIR/$NATIVEIMAGE/pic32-tools/bin/pic32-gcc -I$WORKING_DIR/pic32-compiler/src48x/gcc/gcc/ginclude" \
        CXX_FOR_TARGET=$WORKING_DIR/$NATIVEIMAGE/pic32-tools/bin/pic32-g++ \
        CPP_FOR_TARGET=$WORKING_DIR/$NATIVEIMAGE/pic32-tools/bin/pic32-g++ \
        AR_FOR_TARGET=$WORKING_DIR/$NATIVEIMAGE/pic32-tools/bin/pic32-ar \
        RANLIB_FOR_TARGET=$WORKING_DIR/$NATIVEIMAGE/pic32-tools/bin/pic32-ranlib \
        READELF_FOR_TARGET=$WORKING_DIR/$NATIVEIMAGE/pic32-tools/bin/pic32-readelf \
        STRIP_FOR_TARGET=$WORKING_DIR/$NATIVEIMAGE/pic32-tools/bin/pic32-strip \
        AS_FOR_TARGET=$WORKING_DIR/$NATIVEIMAGE/pic32-tools/pic32mx/bin/as \
        LD_FOR_TARGET=$WORKING_DIR/$NATIVEIMAGE/pic32-tools/pic32mx/bin/ld \
        	../../pic32-compiler/src48x/gcc/configure $GCC_CONFIGURE_FLAGS $BUILDMACHINE --host=$LINUX32_HOST_PREFIX --prefix=$WORKING_DIR/$LINUX32IMAGE/pic32-tools \
        	--bindir="$WORKING_DIR/$LINUX32IMAGE/pic32-tools/bin/bin" --libexecdir="$WORKING_DIR/$LINUX32IMAGE/pic32-tools/bin/bin" \
        	--with-dwarf2 "--with-host-libstdcxx=-static-libgcc -Wl,-Bstatic,-lstdc++,-Bdynamic -lm" --with-libelf=$WORKING_DIR/linux32-build/linux-libs \
        	--with-gmp=$WORKING_DIR/linux32-build/linux-libs $USE_CLOOG $USE_PPL CFLAGS_FOR_TARGET="-fno-rtti -fno-exceptions -fomit-frame-pointer -DPREFER_SIZE_OVER_SPEED \
        	-D__OPTIMIZE_SIZE__ -Os -fshort-wchar -fno-unroll-loops -fno-enforce-eh-specs -ffunction-sections -fdata-sections" \
        	XGCC_FLAGS_FOR_TARGET="-fno-rtti -fno-exceptions -fomit-frame-pointer -DPREFER_SIZE_OVER_SPEED -D__OPTIMIZE_SIZE__ -Os -fshort-wchar -fno-unroll-loops \
        	-fno-enforce-eh-specs -ffunction-sections -fdata-sections" --enable-cxx-flags="-fno-rtti -fno-exceptions -fomit-frame-pointer -DPREFER_SIZE_OVER_SPEED \
        	-D__OPTIMIZE_SIZE__ -Os -fshort-wchar -fno-unroll-loops -fno-enforce-eh-specs -ffunction-sections -fdata-sections" $SUPPORT_HOSTED_LIBSTDCXX 2>&1 | tee $LOGFILE >> /dev/null
        assert_success $? "ERROR: configuring linux32 cross build 2"

        make CFLAGS="-O2" CXXFLAGS="-O2" all \
        	GCC_FOR_TARGET=$WORKING_DIR/$NATIVEIMAGE/pic32-tools/bin/pic32-gcc \
        	CC_FOR_TARGET="$WORKING_DIR/$NATIVEIMAGE/pic32-tools/bin/pic32-gcc -I$WORKING_DIR/pic32-compiler/src48x/gcc/gcc/ginclude" \
        	CXX_FOR_TARGET=$WORKING_DIR/$NATIVEIMAGE/pic32-tools/bin/pic32-g++ \
        	CPP_FOR_TARGET=$WORKING_DIR/$NATIVEIMAGE/pic32-tools/bin/pic32-g++ \
        	AR_FOR_TARGET=$WORKING_DIR/$NATIVEIMAGE/pic32-tools/bin/pic32-ar \
        	RANLIB_FOR_TARGET=$WORKING_DIR/$NATIVEIMAGE/pic32-tools/bin/pic32-ranlib \
        	READELF_FOR_TARGET=$WORKING_DIR/$NATIVEIMAGE/pic32-tools/bin/pic32-readelf S\
        	TRIP_FOR_TARGET=$WORKING_DIR/$NATIVEIMAGE/pic32-tools/bin/pic32-strip \
        	AS_FOR_TARGET=$WORKING_DIR/$NATIVEIMAGE/pic32-tools/pic32mx/bin/as \
        	LD_FOR_TARGET=$WORKING_DIR/$NATIVEIMAGE/pic32-tools/pic32mx/bin/ld  -j4 2>&1 | tee $LOGFILE >> /dev/null
        make CFLAGS="-O2" CXXFLAGS="-O2" install 2>&1 | tee $LOGFILE >> /dev/null
        assert_success $? "ERROR: installing linux Canadian-cross compiler build"

        cd ..
        if [ -e gdb ]
        then
            rm -rf gdb
        fi
        mkdir gdb
        assert_success $? "ERROR: creating directory $WORKING_DIR/linux32-build/gdb"

        cd gdb
        echo `date` " Configuring gdb in $WORKING_DIR/linux32-build..." 2>&1 | tee $LOGFILE >> /dev/null
        ../../pic32-compiler/src48x/gdb/configure \
            --prefix=$WORKING_DIR/$NATIVEIMAGE/pic32-tools \
            --bindir="$WORKING_DIR/$NATIVEIMAGE/pic32-tools/bin/bin" \
            --host="$LINUX32_HOST_PREFIX" \
            --target=pic32mx \
            --program-prefix=pic32- \
            --disable-binutils \
            --disable-gas \
            --disable-ld \
            --disable-gprof \
            --disable-sim \
            --disable-tui \
            --disable-gdbtk 2>&1 | tee $LOGFILE >> /dev/null
        assert_success $? "ERROR: Configure gdb for linux32 build"
        echo `date` " Building gdb in $WORKING_DIR/linux32-build..." 2>&1 | tee $LOGFILE >> /dev/null
        make 2>&1 | tee $LOGFILE >> /dev/null
        assert_success $? "ERROR: making gdb for linux32 build"
        make install 2>&1 | tee $LOGFILE >> /dev/null
        assert_success $? "ERROR: installing gdb for linux32 build"

        cd ../..
        status_update "Make linux32 Canadian cross build complete"

        unset CC
        unset CPP
        unset CXX

        # strip Linux32-image
        cd $WORKING_DIR/$LINUX32IMAGE/pic32-tools
        find ./bin -type f -perm -g+x -follow | xargs file | grep ELF | cut -d: -f1 | xargs $LINUX32_HOST_PREFIX-strip
        find ./pic32mx/bin -type f -perm -g+x -follow | xargs file | grep ELF | cut -d: -f1 | xargs $LINUX32_HOST_PREFIX-strip
        cd $WORKING_DIR

    fi
fi

unset CC
unset CPP
unset CXX

cd $WORKING_DIR

################ end build linux compiler ##############

################ Begin build win32 compiler ##############

if [ "x$SKIPWIN32" == "x" ] ; then

	# Build xc32 shell
	build_xc32_sh "$WORKING_DIR/win32-image" "TARGET=mingw"

	# Build mingw32 cross compiler
	echo `date` " Creating cross build in $WORKING_DIR/win32-build..." 2>&1 | tee $LOGFILE >> /dev/null
	if [ -e win32-build ]
	then
	    rm -rf win32-build
	fi
	mkdir win32-build
	assert_success $? "ERROR: creating directory $WORKING_DIR/win32-build"

	cd win32-build

	if [ -e binutils ]
	then
	    rm -rf binutils
	fi
	mkdir binutils
	assert_success $? "ERROR: creating directory $WORKING_DIR/win32-build/binutils"

	cd binutils

	# Configure mingw32-cross binutils
	echo `date` " Configuring win32 binutils build in $WORKING_DIR/win32-build..." 2>&1 | tee $LOGFILE >> /dev/null
	../../pic32-compiler/src48x/binutils/configure \
		--target=pic32mx --prefix=$WORKING_DIR/win32-image/pic32-tools \--bindir="$WORKING_DIR/win32-image/pic32-tools/bin/bin" \
		--libexecdir="$WORKING_DIR/win32-image/pic32-tools/bin/bin" --host=$MINGW32_HOST_PREFIX --disable-nls --disable-tui --disable-gdbtk --disable-shared --enable-static \
		--disable-threads --disable-bootstrap  --with-dwarf2 --enable-multilib --without-newlib --disable-sim --with-lib-path=: --enable-poison-system-directories \
		--program-prefix=pic32- --with-bugurl=https://github.com/CyberCastle/PIC32-MCU-Compiler --disable-werror 2>&1 | tee $LOGFILE >> /dev/null
	assert_success $? "ERROR: configuring win32 binutils build"

	# Make MinGW32-cross binutils and install it
	echo `date` " Making all in $WORKING_DIR/win32-build/binutils and installing..." 2>&1 | tee $LOGFILE >> /dev/null
	make CFLAGS="-O2 -D_WIN32_WINNT=0x0501 -DWINVER=0x501 -DMCHP_VERSION=${MCHP_VERSION}" all -j4 2>&1 | tee $LOGFILE >> /dev/null
	assert_success $? "ERROR: making/installing win32 Canadian-cross binutils build"
	make CFLAGS="-O2 -D_WIN32_WINNT=0x0501 -DWINVER=0x501 -DMCHP_VERSION=${MCHP_VERSION}" install 2>&1 | tee $LOGFILE >> /dev/null
	assert_success $? "ERROR: making/installing win32 Canadian-cross binutils build"
	cd ..

	if [ -e gmp ]
	then
	    rm -rf gmp
	fi
	mkdir gmp
	assert_success $? "ERROR: creating directory $WORKING_DIR/win32-build/gmp"

	cd gmp

	echo `date` " Configuring win32 gmp build in $WORKING_DIR/win32-build/gmp..." 2>&1 | tee $LOGFILE >> /dev/null
	CPPFLAGS="-fexceptions" ../../pic32-compiler/src48x/gmp/configure \
		--enable-cxx $BUILDMACHINE --prefix=$WORKING_DIR/win32-build/host-libs --disable-shared --target=$MINGW32_HOST_PREFIX --host=$MINGW32_HOST_PREFIX --disable-nls \
		--with-gnu-ld --disable-debug --disable-rpath --enable-fft "--with-host-libstdcxx=-static-libgcc -Wl,-Bstatic,-lstdc++,-Bdynamic -lm" 2>&1 | tee $LOGFILE >> /dev/null

	# Make win32 gmp and install it
	echo `date` " Making all in $WORKING_DIR/win32-build/gmp and installing..." 2>&1 | tee $LOGFILE >> /dev/null
	make CPPFLAGS="-fexceptions" all -j4 2>&1 | tee $LOGFILE >> /dev/null
	assert_success $? "ERROR: making/installing gmp build"
	make install 2>&1 | tee $LOGFILE >> /dev/null
	assert_success $? "ERROR: making/installing gmp build"

	cd ..

	if [ "x$SKIPGRAPHITE" == "x" ]; then

	    if [ -e ppl ]
	    then
	        rm -rf ppl
	    fi
	    mkdir ppl
	    assert_success $? "ERROR: creating directory $WORKING_DIR/win32-build/ppl"

	    cd ppl
	    echo `date` " Configuring native ppl build in $WORKING_DIR/win32-build/ppl..." 2>&1 | tee $LOGFILE >> /dev/null
	    ../../pic32-compiler/src48x/ppl/configure \
	    	--prefix=$WORKING_DIR/win32-build/host-libs --disable-shared --enable-static --with-gnu-ld --host=$MINGW32_HOST_PREFIX --target=pic32mx --disable-nls \
	    	--enable-optimization=speed --disable-rpath --with-gmp-=$WORKING_DIR/win32-build/host-libs --with-libgmp-prefix=$WORKING_DIR/win32-build/host-libs 2>&1 | tee $LOGFILE >> /dev/null

	    # Make native ppl and install it
	    echo `date` " Making all in $WORKING_DIR/win32-build/ppl and installing..." 2>&1 | tee $LOGFILE >> /dev/null
	    make all -j2 2>&1 | tee $LOGFILE >> /dev/null
	    assert_success $? "ERROR: making/installing ppl build"
	    make install 2>&1 | tee $LOGFILE >> /dev/null
	    assert_success $? "ERROR: making/installing ppl build"

	    cd ..

	    if [ -e cloog ]
	    then
	        rm -rf cloog
	    fi
	    mkdir cloog
	    assert_success $? "ERROR: creating directory $WORKING_DIR/win32-build/cloog"

	    cd cloog

	    echo `date` " Configuring win32 cloog build in $WORKING_DIR/win32-build/cloog..." 2>&1 | tee $LOGFILE >> /dev/null
	    ../../pic32-compiler/src48x/cloog/configure \
	    	$BUILDMACHINE --with-gnu-ld --prefix=$WORKING_DIR/win32-build/host-libs --host=$MINGW32_HOST_PREFIX --target=pic32mx \
	    	--with-gmp=$WORKING_DIR/win32-build/host-libs --with-ppl=$WORKING_DIR/win32-build/host-libs --target=pic32mx \
	    	--disable-shared --enable-static --disable-shared 2>&1 | tee $LOGFILE >> /dev/null

	    # Make native cloog and install it
	    echo `date` " Making all in $WORKING_DIR/win32-build/cloog and installing..." 2>&1 | tee $LOGFILE >> /dev/null
	    make all -j2 2>&1 | tee $LOGFILE >> /dev/null
	    assert_success $? "ERROR: making/installing cloog build"
	    make install 2>&1 | tee $LOGFILE >> /dev/null
	    assert_success $? "ERROR: making/installing cloog build"

	    cd ..
	else
	    USE_CLOOG="--without-cloog"
	    USE_PPL="--without-isl"
	fi

	if [ -e libelf ]
	then
	    rm -rf libelf
	fi
	mkdir libelf
	assert_success $? "ERROR: creating directory $WORKING_DIR/win32-build/libelf"

	cd libelf
	echo `date` " Configuring native libelf build in $WORKING_DIR/win32-build/libelf..." 2>&1 | tee $LOGFILE >> /dev/null
	GCC_FOR_TARGET='pic32-gcc' \
	CC_FOR_TARGET='pic32-gcc' \
	CPP_FOR_TARGET='pic32-g++' \
	AS_FOR_TARGET=pic32-as \
	LD_FOR_TARGET=pic32-ld \
	CFLAGS_FOR_BUILD="-Os" \
	../../pic32-compiler/src48x/libelf/configure \
		--prefix=$WORKING_DIR/win32-build/host-libs --host=$MINGW32_HOST_PREFIX $BUILDMACHINE --target=pic32mx --disable-shared --disable-debug --disable-nls 2>&1 | tee $LOGFILE >> /dev/null

	# Make native libelf and install it
	echo `date` " Making all in $WORKING_DIR/win32-build/libelf and installing..." 2>&1 | tee $LOGFILE >> /dev/null
	make all -j4 2>&1 | tee $LOGFILE >> /dev/null
	assert_success $? "ERROR: making/installing libelf build"
	make install 2>&1 | tee $LOGFILE >> /dev/null
	assert_success $? "ERROR: making/installing libelf build"
	cd ..

	if [ -e zlib ]
	then
	    rm -rf zlib
	fi
	cp -r ../pic32-compiler/src48x/zlib .
	assert_success $? "ERROR: copy src48x/zlib directory to $WORKING_DIR/win32-build/zlib"

	cd zlib
	echo `date` " Configuring win32 zlib build in $WORKING_DIR/win32-build/zlib..." 2>&1 | tee $LOGFILE >> /dev/null
	CC=$MINGW32_HOST_PREFIX-gcc AR="$MINGW32_HOST_PREFIX-ar" RANLIB=$MINGW32_HOST_PREFIX-ranlib ./configure --prefix=$WORKING_DIR/win32-build/host-libs 2>&1 | tee $LOGFILE >> /dev/null

	# Make win32 zlib and install it
	echo `date` " Making all in $WORKING_DIR/win32-build/zlib and installing..." 2>&1 | tee $LOGFILE >> /dev/null
	make all -j4 2>&1 | tee $LOGFILE >> /dev/null
	assert_success $? "ERROR: making/installing zlib build - all"
	make install 2>&1 | tee $LOGFILE >> /dev/null
	assert_success $? "ERROR: making/installing zlib build - install"
	cd ..

	if [ -e gcc ]
	then
	    rm -rf gcc
	fi
	mkdir gcc
	assert_success $? "ERROR: creating directory $WORKING_DIR/win32-build/gcc"

	cd gcc

	# Configure win32 cross compiler
	echo `date` " Configuring win32 cross compiler build in $WORKING_DIR/win32-build..." 2>&1 | tee $LOGFILE >> /dev/null

	AR_FOR_TARGET="$WORKING_DIR/$NATIVEIMAGE/pic32-tools/pic32mx/bin/ar"
	AS_FOR_TARGET="$WORKING_DIR/$NATIVEIMAGE/pic32-tools/pic32mx/bin/as"
	LD_FOR_TARGET="$WORKING_DIR/$NATIVEIMAGE/pic32-tools/pic32mx/bin/ld"
	GCC_FOR_TARGET="$WORKING_DIR/$NATIVEIMAGE/pic32-tools/bin/pic32-gcc" 

	if [ "x$NATIVEIMAGE" == "xLinux-image" ]; then
		CC_FOR_TARGET="$WORKING_DIR/$NATIVEIMAGE/pic32-tools/bin/pic32-gcc" \
		CXX_FOR_TARGET='pic32-gcc' target_alias=pic32- \
		../../pic32-compiler/src48x/gcc/configure \
			$GCC_CONFIGURE_FLAGS $BUILDMACHINE --host=$MINGW32_HOST_PREFIX --prefix=$WORKING_DIR/win32-image/pic32-tools --bindir="$WORKING_DIR/win32-image/pic32-tools/bin/bin" \
			--libexecdir="$WORKING_DIR/win32-image/pic32-tools/bin/bin" --with-libelf=$WORKING_DIR/win32-build/host-libs \
			--with-gmp=$WORKING_DIR/win32-build/host-libs $USE_CLOOG $USE_PPL CFLAGS_FOR_TARGET="-fno-rtti -fno-exceptions -fomit-frame-pointer -DPREFER_SIZE_OVER_SPEED \
			-D__OPTIMIZE_SIZE__ -Os -fshort-wchar -fno-unroll-loops -fno-enforce-eh-specs -ffunction-sections -fdata-sections" \
			XGCC_FLAGS_FOR_TARGET="-fno-rtti -fno-exceptions -fomit-frame-pointer -DPREFER_SIZE_OVER_SPEED -D__OPTIMIZE_SIZE__ -Os -fshort-wchar -fno-unroll-loops \
			-fno-enforce-eh-specs -ffunction-sections -fdata-sections" --enable-cxx-flags="-fno-rtti -fno-exceptions -fomit-frame-pointer -DPREFER_SIZE_OVER_SPEED \
			-D__OPTIMIZE_SIZE__ -Os -fshort-wchar -fno-unroll-loops -fno-enforce-eh-specs -ffunction-sections -fdata-sections" \
			$SUPPORT_HOSTED_LIBSTDCXX "--with-host-libstdcxx=-static-libstdc++ -Wl,-Bstatic,-lstdc++,-lwinpthread,-Bdynamic,-lm" 2>&1 | tee $LOGFILE >> /dev/null
	else
	  CC_FOR_TARGET="$WORKING_DIR/$NATIVEIMAGE/pic32-tools/bin/pic32-gcc" CXX_FOR_TARGET='pic32-gcc' target_alias=pic32- \
	  ../../pic32-compiler/src48x/gcc/configure \
	  	$GCC_CONFIGURE_FLAGS $BUILDMACHINE --host=$MINGW32_HOST_PREFIX "--with-host-libstdcxx=-static-libgcc -Wl,-Bstatic,-lstdc++,-Bdynamic -lm" \
	  	--prefix=$WORKING_DIR/win32-image/pic32-tools --bindir="$WORKING_DIR/win32-image/pic32-tools/bin/bin" --libexecdir="$WORKING_DIR/win32-image/pic32-tools/bin/bin" \
	  	--with-libelf=$WORKING_DIR/win32-build/host-libs --with-gmp=$WORKING_DIR/win32-build/host-libs $USE_CLOOG $USE_PPL CFLAGS_FOR_TARGET="-fno-rtti -fno-exceptions \
	  	-fomit-frame-pointer -DPREFER_SIZE_OVER_SPEED -D__OPTIMIZE_SIZE__ -Os -fshort-wchar -fno-unroll-loops -fno-enforce-eh-specs -ffunction-sections -fdata-sections" \
	  	XGCC_FLAGS_FOR_TARGET="-fno-rtti -fno-exceptions -fomit-frame-pointer -DPREFER_SIZE_OVER_SPEED -D__OPTIMIZE_SIZE__ -Os -fshort-wchar -fno-unroll-loops \
	  	-fno-enforce-eh-specs -ffunction-sections -fdata-sections" --enable-cxx-flags="-fno-rtti -fno-exceptions -fomit-frame-pointer -DPREFER_SIZE_OVER_SPEED \
	  	-D__OPTIMIZE_SIZE__ -Os -fshort-wchar -fno-unroll-loops -fno-enforce-eh-specs -ffunction-sections -fdata-sections" $SUPPORT_HOSTED_LIBSTDCXX 2>&1 | tee $LOGFILE >> /dev/null
	fi
	assert_success $? "ERROR: configuring win3232 cross build"

	# Make cross compiler and install it
	echo `date` " Making all in $WORKING_DIR/win32-build/gcc and installing..." 2>&1 | tee $LOGFILE >> /dev/null
	make CFLAGS="-Os -D_WIN32_WINNT=0x0501 -DWINVER=0x501" CXXFLAGS="-Os -D_WIN32_WINNT=0x0501 -DWINVER=0x501" all-gcc \
		NM_FOR_TARGET="$WORKING_DIR/$NATIVEIMAGE/pic32-tools/bin/pic32-nm" \
		RANLIB_FOR_TARGET="$WORKING_DIR/$NATIVEIMAGE/pic32-tools/bin/pic32-ranlib" \
		STRIP_FOR_TARGET="$WORKING_DIR/$NATIVEIMAGE/pic32-tools/bin/pic32-strip" \
		AR_FOR_TARGET="$WORKING_DIR/$NATIVEIMAGE/pic32-tools/bin/pic32-ar" \
		AS_FOR_TARGET="$WORKING_DIR/$NATIVEIMAGE/pic32-tools/pic32mx/bin/as" \
		LD_FOR_TARGET="$WORKING_DIR/$NATIVEIMAGE/pic32-tools/pic32mx/bin/ld" \
		GCC_FOR_TARGET="$WORKING_DIR/$NATIVEIMAGE/pic32-tools/bin/pic32-gcc" \
		CC_FOR_TARGET="$WORKING_DIR/$NATIVEIMAGE/pic32-tools/bin/pic32-gcc" -j4 CXXFLAGS="$CXXFLAGS_FOR_TARGET -Os" 2>&1 | tee $LOGFILE >> /dev/null
	assert_success $? "ERROR: making/installing win32 Canadian-cross compiler build"

	make CFLAGS="-Os -D_WIN32_WINNT=0x0501 -DWINVER=0x501" install-gcc 2>&1 | tee $LOGFILE >> /dev/null
	assert_success $? "ERROR: making/installing win32 Canadian-cross compiler build"

	cd ..

	if [ "xSKIPMULTIPLENEWLIB" != x ]; then

	if [ -e newlib ]
	then
	    rm -rf newlib
	fi
	mkdir newlib
	assert_success $? "ERROR: creating directory $WORKING_DIR/win32IMAGE/newlib"

	cd newlib

	echo `date` " Configure newlib for win32-image..." 2>&1 | tee $LOGFILE >> /dev/null

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
		$NEWLIB_CONFIGURE_FLAGS --prefix=$WORKING_DIR/win32-image/pic32-tools --bindir="$WORKING_DIR/win32-image/pic32-tools/bin/bin" \
		--libexecdir="$WORKING_DIR/win32-image/pic32-tools/bin/bin" CFLAGS_FOR_TARGET="-fno-rtti -fno-exceptions -fomit-frame-pointer -DPREFER_SIZE_OVER_SPEED \
		-D__OPTIMIZE_SIZE__ -Os -fshort-wchar -fno-unroll-loops -fno-enforce-eh-specs -ffunction-sections -fdata-sections -DSMALL_MEMORY -D_NO_GETLOGIN -D_NO_GETPWENT \
		-D_NO_GETUT -D_NO_GETPASS -D_NO_SIGSET" CCASFLAGS_FOR_TARGET="-fno-rtti -fno-exceptions -fomit-frame-pointer -DPREFER_SIZE_OVER_SPEED -D__OPTIMIZE_SIZE__ -Os \
		-fshort-wchar -fno-unroll-loops -fno-enforce-eh-specs -ffunction-sections -fdata-sections -DSMALL_MEMORY -D_NO_GETLOGIN -D_NO_GETPWENT -D_NO_GETUT -D_NO_GETPASS \
		-D_NO_SIGSET" XGCC_FLAGS_FOR_TARGET="-fno-rtti -fno-exceptions -fomit-frame-pointer -DPREFER_SIZE_OVER_SPEED -D__OPTIMIZE_SIZE__ -Os -fshort-wchar -fno-unroll-loops \
		-fno-enforce-eh-specs -ffunction-sections -fdata-sections" --enable-cxx-flags="-fno-rtti -fno-exceptions -fomit-frame-pointer -DPREFER_SIZE_OVER_SPEED \
		-D__OPTIMIZE_SIZE__ -Os -fshort-wchar -fno-unroll-loops -fno-enforce-eh-specs -ffunction-sections -fdata-sections" 2>&1 | tee $LOGFILE >> /dev/null

	echo `date` " Make newlib for win32-image..." 2>&1 | tee $LOGFILE >> /dev/null

	make all -j4 \
		CFLAGS_FOR_TARGET="-D_WIN32_WINNT=0x0501 -DWINVER=0x501 -fno-rtti -fno-exceptions -fomit-frame-pointer -DPREFER_SIZE_OVER_SPEED -D__OPTIMIZE_SIZE__ \
		-Os -fshort-wchar -fno-unroll-loops -fno-enforce-eh-specs -ffunction-sections -fdata-sections -DSMALL_MEMORY -D_NO_GETLOGIN -D_NO_GETPWENT -D_NO_GETUT -D_NO_GETPASS \
		-D_NO_SIGSET" CCASFLAGS_FOR_TARGET="-fno-rtti -fno-exceptions -fomit-frame-pointer -DPREFER_SIZE_OVER_SPEED -D__OPTIMIZE_SIZE__ -Os -fshort-wchar -fno-unroll-loops \
		-fno-enforce-eh-specs -ffunction-sections -fdata-sections -DSMALL_MEMORY -D_NO_GETLOGIN -D_NO_GETPWENT -D_NO_GETUT -D_NO_GETPASS -D_NO_SIGSET" \
		XGCC_FLAGS_FOR_TARGET="-fno-rtti -fno-exceptions -fomit-frame-pointer -DPREFER_SIZE_OVER_SPEED -D__OPTIMIZE_SIZE__ -Os -fshort-wchar -fno-unroll-loops \
		-fno-enforce-eh-specs -ffunction-sections -fdata-sections" 2>&1 | tee $LOGFILE >> /dev/null
	assert_success $? "ERROR: Make newlib for cross build"

	make install 2>&1 | tee $LOGFILE >> /dev/null
	assert_success $? "ERROR: Install newlib for cross build"

	cd ..
	fi

	if [ -e gcc ]
	then
	    rm -rf gcc
	fi
	mkdir gcc
	assert_success $? "ERROR: creating directory $WORKING_DIR/win32-build/gcc"

	cd gcc

	echo `date` " Configure gcc after making Newlib for win32-image..." 2>&1 | tee $LOGFILE >> /dev/null
	GCC_FOR_TARGET=$WORKING_DIR/$NATIVEIMAGE/pic32-tools/bin/pic32-gcc \
	CC_FOR_TARGET="$WORKING_DIR/$NATIVEIMAGE/pic32-tools/bin/pic32-gcc -I$WORKING_DIR/pic32-compiler/src48x/gcc/gcc/ginclude" \
	CXX_FOR_TARGET=$WORKING_DIR/$NATIVEIMAGE/pic32-tools/bin/pic32-g++ \
	CPP_FOR_TARGET=$WORKING_DIR/$NATIVEIMAGE/pic32-tools/bin/pic32-g++ \
	AR_FOR_TARGET=$WORKING_DIR/$NATIVEIMAGE/pic32-tools/bin/pic32-ar \
	RANLIB_FOR_TARGET=$WORKING_DIR/$NATIVEIMAGE/pic32-tools/bin/pic32-ranlib \
	READELF_FOR_TARGET=$WORKING_DIR/$NATIVEIMAGE/pic32-tools/bin/pic32-readelf \
	STRIP_FOR_TARGET=$WORKING_DIR/$NATIVEIMAGE/pic32-tools/bin/pic32-strip \
	AS_FOR_TARGET=$WORKING_DIR/$NATIVEIMAGE/pic32-tools/pic32mx/bin/as \
	LD_FOR_TARGET=$WORKING_DIR/$NATIVEIMAGE/pic32-tools/pic32mx/bin/ld \
	../../pic32-compiler/src48x/gcc/configure \
		$GCC_CONFIGURE_FLAGS $BUILDMACHINE --host=$MINGW32_HOST_PREFIX --prefix=$WORKING_DIR/win32-image/pic32-tools --bindir="$WORKING_DIR/win32-image/pic32-tools/bin/bin" \
		--libexecdir="$WORKING_DIR/win32-image/pic32-tools/bin/bin" --with-libelf=$WORKING_DIR/win32-build/host-libs \
		--with-gmp=$WORKING_DIR/win32-build/host-libs $USE_CLOOG $USE_PPL CFLAGS_FOR_TARGET="-fno-rtti -fno-exceptions -fomit-frame-pointer -DPREFER_SIZE_OVER_SPEED \
		-D__OPTIMIZE_SIZE__ -Os -fshort-wchar -fno-unroll-loops -fno-enforce-eh-specs -ffunction-sections -fdata-sections" XGCC_FLAGS_FOR_TARGET="-fno-rtti -fno-exceptions \
		-fomit-frame-pointer -DPREFER_SIZE_OVER_SPEED -D__OPTIMIZE_SIZE__ -Os -fshort-wchar -fno-unroll-loops -fno-enforce-eh-specs -ffunction-sections -fdata-sections" \
		--enable-cxx-flags="-fno-rtti -fno-exceptions -fomit-frame-pointer -DPREFER_SIZE_OVER_SPEED -D__OPTIMIZE_SIZE__ -Os -fshort-wchar -fno-unroll-loops \
		-fno-enforce-eh-specs -ffunction-sections -fdata-sections" $SUPPORT_HOSTED_LIBSTDCXX 2>&1 | tee $LOGFILE >> /dev/null
	assert_success $? "ERROR: configuring win32 cross build 2"

	make CFLAGS="-O2 -D_WIN32_WINNT=0x0501 -DWINVER=0x501" CXXFLAGS="-O2 -D_WIN32_WINNT=0x0501 -DWINVER=0x501" all \
		GCC_FOR_TARGET=$WORKING_DIR/$NATIVEIMAGE/pic32-tools/bin/pic32-gcc \
		CC_FOR_TARGET="$WORKING_DIR/$NATIVEIMAGE/pic32-tools/bin/pic32-gcc -I$WORKING_DIR/pic32-compiler/src48x/gcc/gcc/ginclude" \
		CXX_FOR_TARGET=$WORKING_DIR/$NATIVEIMAGE/pic32-tools/bin/pic32-g++ \
		CPP_FOR_TARGET=$WORKING_DIR/$NATIVEIMAGE/pic32-tools/bin/pic32-g++ \
		AR_FOR_TARGET=$WORKING_DIR/$NATIVEIMAGE/pic32-tools/bin/pic32-ar \
		RANLIB_FOR_TARGET=$WORKING_DIR/$NATIVEIMAGE/pic32-tools/bin/pic32-ranlib \
		READELF_FOR_TARGET=$WORKING_DIR/$NATIVEIMAGE/pic32-tools/bin/pic32-readelf \
		STRIP_FOR_TARGET=$WORKING_DIR/$NATIVEIMAGE/pic32-tools/bin/pic32-strip \
		AS_FOR_TARGET=$WORKING_DIR/$NATIVEIMAGE/pic32-tools/pic32mx/bin/as \
		LD_FOR_TARGET=$WORKING_DIR/$NATIVEIMAGE/pic32-tools/pic32mx/bin/ld -j4 2>&1 | tee $LOGFILE >> /dev/null
	assert_success $? "ERROR: making win32 Canadian-cross compiler build"
	
	make CFLAGS="-O2 -D_WIN32_WINNT=0x0501 -DWINVER=0x501" CXXFLAGS="-O2 -D_WIN32_WINNT=0x0501 -DWINVER=0x501" install 2>&1 | tee $LOGFILE >> /dev/null
	assert_success $? "ERROR: installing win32 Canadian-cross compiler build"

	cd ..
	if [ -e gdb ]
	then
	    rm -rf gdb
	fi
	mkdir gdb
	assert_success $? "ERROR: creating directory $WORKING_DIR/win32-build/gdb"

	cd gdb
	echo `date` " Configuring gdb in $WORKING_DIR/win32-build..." 2>&1 | tee $LOGFILE >> /dev/null
	../../pic32-compiler/src48x/gdb/configure \
	    --prefix=$WORKING_DIR/win32-image/pic32-tools \
	    --bindir="$WORKING_DIR/win32-image/pic32-tools/bin/bin" \
	    --target=pic32mx \
	    --host=$MINGW32_HOST_PREFIX \
	    --program-prefix=pic32- \
	    --disable-binutils \
	    --disable-gas \
	    --disable-ld \
	    --disable-gprof \
	    --disable-sim \
	    --disable-tui \
	    --disable-gdbtk 2>&1 | tee $LOGFILE >> /dev/null
	assert_success $? "ERROR: Configure gdb for native build"
	echo `date` " Building gdb in $WORKING_DIR/win32-build..." 2>&1 | tee $LOGFILE >> /dev/null
	make 2>&1 | tee $LOGFILE >> /dev/null
	assert_success $? "ERROR: making gdb for win32 build"
	make install 2>&1 | tee $LOGFILE >> /dev/null
	assert_success $? "ERROR: installing gdb for win32 build"

	cd ../..
	status_update "Make win32 Canadian cross build complete"

	unset CC
	unset CPP
	unset CXX

	cd $WORKING_DIR/win32-image/pic32-tools
	find . -type f -name "*.exe" | xargs $MINGW32_HOST_PREFIX-strip

	status_update "Make minGW32 Canadian cross build complete"

fi

cd $WORKING_DIR


####### End win32 build ###############

if [ "x$SKIPARMLINUX" == "x" ]; then
####### Begin arm-linux build ############

	# Build xc32 shell
	build_xc32_sh "$WORKING_DIR/arm-linux-image" "TARGET=arm"

	################ Begin build arm-linux compiler ##############
	# Build ARMLINUX32 cross compiler
	echo `date` " Creating cross build in $WORKING_DIR/arm-linux-build..." 2>&1 | tee $LOGFILE >> /dev/null
	if [ -e arm-linux-build ]
	then
	    rm -rf arm-linux-build
	fi
	mkdir arm-linux-build
	assert_success $? "ERROR: creating directory $WORKING_DIR/arm-linux-build"

	cd arm-linux-build

	if [ -e binutils ]
	then
	    rm -rf binutils
	fi
	mkdir binutils
	assert_success $? "ERROR: creating directory $WORKING_DIR/arm-linux-build/binutils"

	cd binutils

	# Configure ARMLINUX32-cross binutils
	echo `date` " Configuring arm-linux binutils build in $WORKING_DIR/arm-linux-build..." 2>&1 | tee $LOGFILE >> /dev/null
	../../pic32-compiler/src48x/binutils/configure \
		--target=pic32mx --prefix=$WORKING_DIR/arm-linux-image/pic32-tools --bindir="$WORKING_DIR/arm-linux-image/pic32-tools/bin/bin" \
		--libexecdir="$WORKING_DIR/arm-linux-image/bin/bin" --host=$ARMLINUX32_HOST_PREFIX --disable-nls --disable-tui --disable-gdbtk --disable-shared --enable-static \
		--disable-threads --disable-bootstrap  --with-dwarf2 --enable-multilib --without-newlib --disable-sim --with-lib-path=: --enable-poison-system-directories \
		--program-prefix=pic32- --with-bugurl=https://github.com/CyberCastle/PIC32-MCU-Compiler --disable-werror 2>&1 | tee $LOGFILE >> /dev/null
	assert_success $? "ERROR: configuring arm-linux binutils build"

	# Make ARMLINUX32-cross binutils and install it
	echo `date` " Making all in $WORKING_DIR/arm-linux-build/binutils and installing..." 2>&1 | tee $LOGFILE >> /dev/null
	make CFLAGS="-Os -DMCHP_VERSION=${MCHP_VERSION}" all -j4 2>&1 | tee $LOGFILE >> /dev/null
	assert_success $? "ERROR: making/installing arm-linux Canadian-cross binutils build"

	make CFLAGS="-Os -DMCHP_VERSION=${MCHP_VERSION}"  install 2>&1 | tee $LOGFILE >> /dev/null
	assert_success $? "ERROR: making/installing arm-linux Canadian-cross binutils build"
	cd ..

	if [ -e gmp ]
	then
	    rm -rf gmp
	fi
	mkdir gmp
	assert_success $? "ERROR: creating directory $WORKING_DIR/arm-linux-build/gmp"

	cd gmp

	echo `date` " Configuring arm-linux gmp build in $WORKING_DIR/arm-linux-build/gmp..." 2>&1 | tee $LOGFILE >> /dev/null
	CPPFLAGS="-fexceptions" ../../pic32-compiler/src48x/gmp/configure \
		--enable-cxx --prefix=$WORKING_DIR/arm-linux-build/host-libs --disable-shared --host=$ARMLINUX32_HOST_PREFIX --disable-nls --with-gnu-ld --disable-debug \
		--disable-rpath --enable-fft "--with-host-libstdcxx=-static-libgcc -Wl,-Bstatic,-lstdc++,-Bdynamic -lm" 2>&1 | tee $LOGFILE >> /dev/null

	# Make arm-linux gmp and install it
	echo `date` " Making all in $WORKING_DIR/arm-linux-build/gmp and installing..." 2>&1 | tee $LOGFILE >> /dev/null
	make CPPFLAGS="-fexceptions" all -j4 2>&1 | tee $LOGFILE >> /dev/null
	assert_success $? "ERROR: making/installing gmp build"

	make install 2>&1 | tee $LOGFILE >> /dev/null
	assert_success $? "ERROR: making/installing gmp build"

	cd ..

	if [ "x$SKIPGRAPHITE" == "x" ]; then

	    if [ -e ppl ]
	    then
	        rm -rf ppl
	    fi
	    mkdir ppl
	    assert_success $? "ERROR: creating directory $WORKING_DIR/arm-linux-build/ppl"

	    cd ppl
	    echo `date` " Configuring native ppl build in $WORKING_DIR/arm-linux-build/ppl..." 2>&1 | tee $LOGFILE >> /dev/null
	    ../../pic32-compiler/src48x/ppl/configure \
	    	--prefix=$WORKING_DIR/arm-linux-build/host-libs --disable-shared --enable-static --with-gnu-ld --host=$ARMLINUX32_HOST_PREFIX --target=pic32mx --disable-nls \
	    	--enable-optimization=speed --disable-rpath --with-gmp-=$WORKING_DIR/arm-linux-build/host-libs \
            --with-libgmp-prefix=$WORKING_DIR/arm-linux-build/host-libs 2>&1 | tee $LOGFILE >> /dev/null

	    # Make native ppl and install it
	    echo `date` " Making all in $WORKING_DIR/arm-linux-build/ppl and installing..." 2>&1 | tee $LOGFILE >> /dev/null
	    make all -j4 2>&1 | tee $LOGFILE >> /dev/null
	    assert_success $? "ERROR: making/installing ppl build"
	    make install 2>&1 | tee $LOGFILE >> /dev/null
	    assert_success $? "ERROR: making/installing ppl build"

	    cd ..

	    if [ -e cloog ]
	    then
	        rm -rf cloog
	    fi
	    mkdir cloog
	    assert_success $? "ERROR: creating directory $WORKING_DIR/arm-linux-build/cloog"

	    cd cloog

	    echo `date` " Configuring arm-linux cloog build in $WORKING_DIR/arm-linux-build/cloog..." 2>&1 | tee $LOGFILE >> /dev/null
	    ../../pic32-compiler/src48x/cloog/configure \
	    	$BUILDMACHINE --with-gnu-ld --prefix=$WORKING_DIR/arm-linux-build/host-libs --host=$ARMLINUX32_HOST_PREFIX --target=pic32mx \
	    	--with-gmp=$WORKING_DIR/arm-linux-build/host-libs --with-ppl=$WORKING_DIR/arm-linux-build/host-libs --target=pic32mx --disable-shared \
	    	--enable-static --disable-shared 2>&1 | tee $LOGFILE >> /dev/null

	    # Make native cloog and install it
	    echo `date` " Making all in $WORKING_DIR/arm-linux-build/cloog and installing..." 2>&1 | tee $LOGFILE >> /dev/null
	    make all -j4 2>&1 | tee $LOGFILE >> /dev/null
	    assert_success $? "ERROR: making/installing cloog build"
	    make install 2>&1 | tee $LOGFILE >> /dev/null
	    assert_success $? "ERROR: making/installing cloog build"

	    cd ..
	else
	    USE_CLOOG="--without-cloog"
	    USE_PPL="--without-isl"
	fi

	if [ -e libelf ]
	then
	    rm -rf libelf
	fi
	mkdir libelf
	assert_success $? "ERROR: creating directory $WORKING_DIR/arm-linux-build/libelf"

	cd libelf
	echo `date` " Configuring native libelf build in $WORKING_DIR/arm-linux-build/libelf..." 2>&1 | tee $LOGFILE >> /dev/null
	GCC_FOR_TARGET='pic32-gcc' \
	CC_FOR_TARGET='pic32-gcc' \
	CPP_FOR_TARGET='pic32-g++' \
	AS_FOR_TARGET=pic32-as \
	LD_FOR_TARGET=pic32-ld \
	CFLAGS_FOR_BUILD="-Os" \
	../../pic32-compiler/src48x/libelf/configure \
			--prefix=$WORKING_DIR/arm-linux-build/host-libs \
			--host=$ARMLINUX32_HOST_PREFIX $BUILDMACHINE --target=pic32mx --disable-shared --disable-debug --disable-nls 2>&1 | tee $LOGFILE >> /dev/null

	# Make native libelf and install it
	echo `date` " Making all in $WORKING_DIR/arm-linux-build/libelf and installing..." 2>&1 | tee $LOGFILE >> /dev/null
	make all -j4 2>&1 | tee $LOGFILE >> /dev/null
	assert_success $? "ERROR: making/installing libelf build"
	make install 2>&1 | tee $LOGFILE >> /dev/null
	assert_success $? "ERROR: making/installing libelf build"
	cd ..

	if [ -e zlib ]
	then
	    rm -rf zlib
	fi
	cp -r ../pic32-compiler/src48x/zlib .
	assert_success $? "ERROR: copy src48x/zlib directory to $WORKING_DIR/arm-linux-build/zlib"

	cd zlib
	echo `date` " Configuring arm-linux zlib build in $WORKING_DIR/arm-linux-build/zlib..." 2>&1 | tee $LOGFILE >> /dev/null
	CC=$ARMLINUX32_HOST_PREFIX-gcc AR="$ARMLINUX32_HOST_PREFIX-ar" RANLIB=$ARMLINUX32_HOST_PREFIX-ranlib \
    ./configure --prefix=$WORKING_DIR/arm-linux-build/host-libs 2>&1 | tee $LOGFILE >> /dev/null

	# Make arm-linux zlib and install it
	echo `date` " Making all in $WORKING_DIR/arm-linux-build/zlib and installing..." 2>&1 | tee $LOGFILE >> /dev/null
	make all -j4 2>&1 | tee $LOGFILE >> /dev/null
	assert_success $? "ERROR: making/installing zlib build - all"

	make install 2>&1 | tee $LOGFILE >> /dev/null
	assert_success $? "ERROR: making/installing zlib build - install"
	cd ..

	if [ -e gcc ]
	then
	    rm -rf gcc
	fi
	mkdir gcc
	assert_success $? "ERROR: creating directory $WORKING_DIR/arm-linux-build/gcc"

	cd gcc

	# Configure arm-linux cross compiler
	echo `date` " Configuring arm-linux cross compiler build in $WORKING_DIR/arm-linux-build..." 2>&1 | tee $LOGFILE >> /dev/null

	AR_FOR_TARGET="$WORKING_DIR/$NATIVEIMAGE/pic32-tools/pic32mx/bin/ar" \
	AS_FOR_TARGET="$WORKING_DIR/$NATIVEIMAGE/pic32-tools/pic32mx/bin/as" \
	LD_FOR_TARGET="$WORKING_DIR/$NATIVEIMAGE/pic32-tools/pic32mx/bin/ld" \
	GCC_FOR_TARGET="$WORKING_DIR/$NATIVEIMAGE/pic32-tools/bin/pic32-gcc" \
	CC_FOR_TARGET="$WORKING_DIR/$NATIVEIMAGE/pic32-tools/bin/pic32-gcc" \
	CXX_FOR_TARGET='pic32-gcc' target_alias=pic32- \
	../../pic32-compiler/src48x/gcc/configure \
		$GCC_CONFIGURE_FLAGS $BUILDMACHINE --host=$ARMLINUX32_HOST_PREFIX --prefix=$WORKING_DIR/arm-linux-image/pic32-tools \
		--bindir="$WORKING_DIR/arm-linux-image/pic32-tools/bin/bin" --libexecdir="$WORKING_DIR/arm-linux-image/pic32-tools/bin/bin" \
		--with-libelf=$WORKING_DIR/arm-linux-build/host-libs --with-gmp=$WORKING_DIR/arm-linux-build/host-libs $USE_CLOOG $USE_PPL \
		CFLAGS_FOR_TARGET="-fno-rtti -fno-exceptions -fomit-frame-pointer -DPREFER_SIZE_OVER_SPEED -D__OPTIMIZE_SIZE__ -Os -fshort-wchar -fno-unroll-loops \
		-fno-enforce-eh-specs -ffunction-sections -fdata-sections" XGCC_FLAGS_FOR_TARGET="-fno-rtti -fno-exceptions -fomit-frame-pointer -DPREFER_SIZE_OVER_SPEED \
		-D__OPTIMIZE_SIZE__ -Os -fshort-wchar -fno-unroll-loops -fno-enforce-eh-specs -ffunction-sections -fdata-sections" --enable-cxx-flags="-fno-rtti -fno-exceptions \
		-fomit-frame-pointer -DPREFER_SIZE_OVER_SPEED -D__OPTIMIZE_SIZE__ -Os -fshort-wchar -fno-unroll-loops -fno-enforce-eh-specs -ffunction-sections -fdata-sections" \
		$SUPPORT_HOSTED_LIBSTDCXX 2>&1 | tee $LOGFILE >> /dev/null

	assert_success $? "ERROR: configuring arm-linux32 cross build"

	# Make cross compiler and install it
	echo `date` " Making all in $WORKING_DIR/arm-linux-build/gcc and installing..." 2>&1 | tee $LOGFILE >> /dev/null
	make CFLAGS="-Os" all-gcc \
		NM_FOR_TARGET="$WORKING_DIR/$NATIVEIMAGE/pic32-tools/bin/pic32-nm" \
		RANLIB_FOR_TARGET="$WORKING_DIR/$NATIVEIMAGE/pic32-tools/bin/pic32-ranlib" \
		STRIP_FOR_TARGET="$WORKING_DIR/$NATIVEIMAGE/pic32-tools/bin/pic32-strip" \
		AR_FOR_TARGET="$WORKING_DIR/$NATIVEIMAGE/pic32-tools/bin/pic32-ar" \
		AS_FOR_TARGET="$WORKING_DIR/$NATIVEIMAGE/pic32-tools/pic32mx/bin/as" \
		LD_FOR_TARGET="$WORKING_DIR/$NATIVEIMAGE/pic32-tools/pic32mx/bin/ld" \
		GCC_FOR_TARGET="$WORKING_DIR/$NATIVEIMAGE/pic32-tools/bin/pic32-gcc" \
		CC_FOR_TARGET="$WORKING_DIR/$NATIVEIMAGE/pic32-tools/bin/pic32-gcc" -j4 CXXFLAGS="$CXXFLAGS_FOR_TARGET -Os" 2>&1 | tee $LOGFILE >> /dev/null
	assert_success $? "ERROR: making/installing arm-linux Canadian-cross compiler build"

	make CFLAGS="-Os" install-gcc 2>&1 | tee $LOGFILE >> /dev/null
	assert_success $? "ERROR: making/installing arm-linux Canadian-cross compiler build"

	cd ..

	if [ "xSKIPMULTIPLENEWLIB" != x ]; then

	if [ -e newlib ]
	then
	    rm -rf newlib
	fi
	mkdir newlib
	assert_success $? "ERROR: creating directory $WORKING_DIR/arm-linuxIMAGE/newlib"

	cd newlib

	#build newlib here
	echo `date` " Configure newlib for arm-linux-image..." 2>&1 | tee $LOGFILE >> /dev/null
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
		$NEWLIB_CONFIGURE_FLAGS --prefix=$WORKING_DIR/arm-linux-image/pic32-tools --bindir="$WORKING_DIR/arm-linux-image/pic32-tools/bin/bin" \
		--libexecdir="$WORKING_DIR/arm-linux-image/pic32-tools/bin/bin" CFLAGS_FOR_TARGET="-fno-rtti -fno-exceptions -fomit-frame-pointer -DPREFER_SIZE_OVER_SPEED \
		-D__OPTIMIZE_SIZE__ -Os -fshort-wchar -fno-unroll-loops -fno-enforce-eh-specs -ffunction-sections -fdata-sections -DSMALL_MEMORY -D_NO_GETLOGIN -D_NO_GETPWENT \
		-D_NO_GETUT -D_NO_GETPASS -D_NO_SIGSET" CCASFLAGS_FOR_TARGET="-fno-rtti -fno-exceptions -fomit-frame-pointer -DPREFER_SIZE_OVER_SPEED -D__OPTIMIZE_SIZE__ -Os \
		-fshort-wchar -fno-unroll-loops -fno-enforce-eh-specs -ffunction-sections -fdata-sections -DSMALL_MEMORY -D_NO_GETLOGIN -D_NO_GETPWENT -D_NO_GETUT -D_NO_GETPASS \
		-D_NO_SIGSET" XGCC_FLAGS_FOR_TARGET="-fno-rtti -fno-exceptions -fomit-frame-pointer -DPREFER_SIZE_OVER_SPEED -D__OPTIMIZE_SIZE__ -Os -fshort-wchar -fno-unroll-loops \
		-fno-enforce-eh-specs -ffunction-sections -fdata-sections" --enable-cxx-flags="-fno-rtti -fno-exceptions -fomit-frame-pointer -DPREFER_SIZE_OVER_SPEED \
		-D__OPTIMIZE_SIZE__ -Os -fshort-wchar -fno-unroll-loops -fno-enforce-eh-specs -ffunction-sections -fdata-sections" 2>&1 | tee $LOGFILE >> /dev/null

	echo `date` " Make newlib for arm-linux-image..." 2>&1 | tee $LOGFILE >> /dev/null
	make all -j4 CFLAGS_FOR_TARGET="-fno-rtti -fno-exceptions -fomit-frame-pointer -DPREFER_SIZE_OVER_SPEED -D__OPTIMIZE_SIZE__ -Os -fshort-wchar \
		-fno-unroll-loops -fno-enforce-eh-specs -ffunction-sections -fdata-sections -DSMALL_MEMORY -D_NO_GETLOGIN -D_NO_GETPWENT -D_NO_GETUT -D_NO_GETPASS -D_NO_SIGSET" \
		CCASFLAGS_FOR_TARGET="-fno-rtti -fno-exceptions -fomit-frame-pointer -DPREFER_SIZE_OVER_SPEED -D__OPTIMIZE_SIZE__ -Os -fshort-wchar -fno-unroll-loops \
		-fno-enforce-eh-specs -ffunction-sections -fdata-sections -DSMALL_MEMORY -D_NO_GETLOGIN -D_NO_GETPWENT -D_NO_GETUT -D_NO_GETPASS -D_NO_SIGSET" \
		XGCC_FLAGS_FOR_TARGET="-fno-rtti -fno-exceptions -fomit-frame-pointer -DPREFER_SIZE_OVER_SPEED -D__OPTIMIZE_SIZE__ -Os -fshort-wchar -fno-unroll-loops \
		-fno-enforce-eh-specs -ffunction-sections -fdata-sections" 2>&1 | tee $LOGFILE >> /dev/null
	assert_success $? "ERROR: Make newlib for cross build"

	make install 2>&1 | tee $LOGFILE >> /dev/null
	assert_success $? "ERROR: Install newlib for cross build"

	cd ..
	fi

	if [ -e gcc ]
	then
	    rm -rf gcc
	fi
	mkdir gcc
	assert_success $? "ERROR: creating directory $WORKING_DIR/arm-linux-build/gcc"

	cd gcc

	echo `date` " Configure gcc after making Newlib for arm-linux-image..." 2>&1 | tee $LOGFILE >> /dev/null
	GCC_FOR_TARGET=$WORKING_DIR/$NATIVEIMAGE/pic32-tools/bin/pic32-gcc \
	CC_FOR_TARGET="$WORKING_DIR/$NATIVEIMAGE/pic32-tools/bin/pic32-gcc -I$WORKING_DIR/pic32-compiler/src48x/gcc/gcc/ginclude" \
	CXX_FOR_TARGET=$WORKING_DIR/$NATIVEIMAGE/pic32-tools/bin/pic32-g++ \
	CPP_FOR_TARGET=$WORKING_DIR/$NATIVEIMAGE/pic32-tools/bin/pic32-g++ \
	AR_FOR_TARGET=$WORKING_DIR/$NATIVEIMAGE/pic32-tools/bin/pic32-ar \
	RANLIB_FOR_TARGET=$WORKING_DIR/$NATIVEIMAGE/pic32-tools/bin/pic32-ranlib \
	READELF_FOR_TARGET=$WORKING_DIR/$NATIVEIMAGE/pic32-tools/bin/pic32-readelf \
	STRIP_FOR_TARGET=$WORKING_DIR/$NATIVEIMAGE/pic32-tools/bin/pic32-strip \
	AS_FOR_TARGET=$WORKING_DIR/$NATIVEIMAGE/pic32-tools/pic32mx/bin/as \
	LD_FOR_TARGET=$WORKING_DIR/$NATIVEIMAGE/pic32-tools/pic32mx/bin/ld \
	../../pic32-compiler/src48x/gcc/configure \
		$GCC_CONFIGURE_FLAGS $BUILDMACHINE --host=$ARMLINUX32_HOST_PREFIX --prefix=$WORKING_DIR/arm-linux-image/pic32-tools \
		--bindir="$WORKING_DIR/arm-linux-image/pic32-tools/bin/bin" --libexecdir="$WORKING_DIR/arm-linux-image/pic32-tools/bin/bin" \
		--with-libelf=$WORKING_DIR/arm-linux-build/host-libs --with-gmp=$WORKING_DIR/arm-linux-build/host-libs $USE_CLOOG $USE_PPL \
		CFLAGS_FOR_TARGET="-fno-rtti -fno-exceptions -fomit-frame-pointer  PREFER_SIZE_OVER_SPEED -D__OPTIMIZE_SIZE__ -Os -fshort-wchar -fno-unroll-loops \
		-fno-enforce-eh-specs -ffunction-sections -fdata-sections" XGCC_FLAGS_FOR_TARGET="-fno-rtti -fno-exceptions -fomit-frame-pointer -DPREFER_SIZE_OVER_SPEED \
		-D__OPTIMIZE_SIZE__ -Os -fshort-wchar -fno-unroll-loops -fno-enforce-eh-specs -ffunction-sections -fdata-sections" --enable-cxx-flags="-fno-rtti -fno-exceptions \
		-fomit-frame-pointer -DPREFER_SIZE_OVER_SPEED -D__OPTIMIZE_SIZE__ -Os -fshort-wchar -fno-unroll-loops -fno-enforce-eh-specs -ffunction-sections -fdata-sections" \
		$SUPPORT_HOSTED_LIBSTDCXX 2>&1 | tee $LOGFILE >> /dev/null
	assert_success $? "ERROR: configuring arm-linux32 cross build 2"

	make CFLAGS="-Os" CXXFLAGS="-Os" all \
		GCC_FOR_TARGET=$WORKING_DIR/$NATIVEIMAGE/pic32-tools/bin/pic32-gcc \
		CC_FOR_TARGET="$WORKING_DIR/$NATIVEIMAGE/pic32-tools/bin/pic32-gcc -I$WORKING_DIR/pic32-compiler/src48x/gcc/gcc/ginclude" \
		CXX_FOR_TARGET=$WORKING_DIR/$NATIVEIMAGE/pic32-tools/bin/pic32-g++ \
		CPP_FOR_TARGET=$WORKING_DIR/$NATIVEIMAGE/pic32-tools/bin/pic32-g++ \
		AR_FOR_TARGET=$WORKING_DIR/$NATIVEIMAGE/pic32-tools/bin/pic32-ar \
		RANLIB_FOR_TARGET=$WORKING_DIR/$NATIVEIMAGE/pic32-tools/bin/pic32-ranlib \
		READELF_FOR_TARGET=$WORKING_DIR/$NATIVEIMAGE/pic32-tools/bin/pic32-readelf \
		STRIP_FOR_TARGET=$WORKING_DIR/$NATIVEIMAGE/pic32-tools/bin/pic32-strip \
		AS_FOR_TARGET=$WORKING_DIR/$NATIVEIMAGE/pic32-tools/pic32mx/bin/as \
		LD_FOR_TARGET=$WORKING_DIR/$NATIVEIMAGE/pic32-tools/pic32mx/bin/ld  -j4 \
		CXXFLAGS="$CXXFLAGS_FOR_TARGET -Os" 2>&1 | tee $LOGFILE >> /dev/null

	make CFLAGS="-Os" CXXFLAGS="-Os" install 2>&1 | tee $LOGFILE >> /dev/null
	assert_success $? "ERROR: installing arm-linux Canadian-cross compiler build"

	cd ../..
	status_update "Make arm-linux Canadian cross build complete"

	unset CC
	unset CPP
	unset CXX

	cd $WORKING_DIR/arm-linux-image/pic32-tools
	find . -type f -name "*.exe" | xargs $ARMLINUX32_HOST_PREFIX-strip

	status_update "Make ARMLINUX32 Canadian cross build complete"

	fi # SKIPARMLINUX

cd $WORKING_DIR

############   End Arm-Linux build #############

###
# In the resultant install directory, there are a few extra directories
# that we don"t want for our Windows build.
###
echo `date` " Removing unecessary directories from build..." 2>&1 | tee $LOGFILE >> /dev/null
echo "Directory clean up for pic-tools"

rmdir  $WORKING_DIR/win32-image/pic32-tools/include
rm -rf $WORKING_DIR/win32-image/pic32-tools/man
rm -rf $WORKING_DIR/win32-image/pic32-tools/info
rm -rf $WORKING_DIR/win32-image/pic32-tools/share
rm -rf $WORKING_DIR/win32-image/pic32-tools/pic32mx/share
rm -rf $WORKING_DIR/win32-image/pic32-tools/libsrc

rmdir  $WORKING_DIR/arm-linux-image/pic32-tools/include
rm -rf $WORKING_DIR/arm-linux-image/pic32-tools/man
rm -rf $WORKING_DIR/arm-linux-image/pic32-tools/info
rm -rf $WORKING_DIR/arm-linux-image/pic32-tools/share
rm -rf $WORKING_DIR/arm-linux-image/pic32-tools/pic32mx/share
rm -rf $WORKING_DIR/arm-linux-image/pic32-tools/libsrc

rmdir  $WORKING_DIR/$NATIVEIMAGE/pic32-tools/include
rm -rf $WORKING_DIR/$NATIVEIMAGE/pic32-tools/man
rm -rf $WORKING_DIR/$NATIVEIMAGE/pic32-tools/info
rm -rf $WORKING_DIR/$NATIVEIMAGE/pic32-tools/share
rm -rf $WORKING_DIR/$NATIVEIMAGE/pic32-tools/pic32mx/share
rm -rf $WORKING_DIR/$NATIVEIMAGE/pic32-tools/libsrc

rm -rf $WORKING_DIR/export-image/pic32-tools/man
rm -rf $WORKING_DIR/export-image/pic32-tools/info
rm -rf $WORKING_DIR/export-image/pic32-tools/share
rm -rf $WORKING_DIR/export-image/pic32-tools/libsrc

if [ "x$LINUX32IMAGE" != "x" ] ; then
    rmdir  $WORKING_DIR/$LINUX32IMAGE/pic32-tools/include
    rm -rf $WORKING_DIR/$LINUX32IMAGE/pic32-tools/man
    rm -rf $WORKING_DIR/$LINUX32IMAGE/pic32-tools/info
    rm -rf $WORKING_DIR/$LINUX32IMAGE/pic32-tools/share
    rm -rf $WORKING_DIR/$LINUX32IMAGE/pic32-tools/pic32mx/share
    rm -rf $WORKING_DIR/$LINUX32IMAGE/pic32-tools/libsrc
fi

if [ "x$SKIPPLIBIMAGE" == "x" ]
then
    cd $WORKING_DIR

    echo "Downloading $HTTP_PLIB_IMAGE_TAR."
    echo `date` "Downloading $HTTP_PLIB_IMAGE_TAR..." 2>&1 | tee $LOGFILE >> /dev/null
    if [ -e plib-image ]
    then
        rm -rf plib-image
    fi
    curl -L $HTTP_PLIB_IMAGE_TAR | tar jx
    assert_success $? "Downloading the peripheral-library image from $HTTP_PLIB_IMAGE_TAR"

    if [ "x$NATIVEIMAGE" != "x" ]
    then
      rsync -qavzC --include "*/" --include "*" plib-image/ $NATIVEIMAGE/pic32-tools/
      assert_success $? "ERROR: Install plib in $NATIVEIMAGE"
    fi
    if [ "x$LINUX32IMAGE" != "x" ]
    then
      rsync -qavzC --include "*/" --include "*" plib-image/ $LINUX32IMAGE/pic32-tools/
      assert_success $? "ERROR: Install plib in $LINUX32IMAGE"
    fi
    if [ -e win32-image ]
    then
      rsync -qavzC --include "*/" --include "*" plib-image/ win32-image/pic32-tools/
      assert_success $? "ERROR: Install plib in win32-image"
    fi
    if [ -e arm-linux-image ]
    then
      rsync -qavzC --include "*/" --include "*" plib-image/ arm-linux-image/pic32-tools/
      assert_success $? "ERROR: Install plib in arm-linux-image"
    fi
fi

cd $WORKING_DIR

# copy resource files
cd win32-image
copy_device_files

cd ../$NATIVEIMAGE
copy_device_files

cd ../arm-linux-image
copy_device_files

cd ../Linux32-image
copy_device_files

cd ..

echo "Making zip files"
#ZIP installation directory.

echo `date` " Tar components to $WORKING_DIR/zips directory..." 2>&1 | tee $LOGFILE >> /dev/null
echo `date` " Tar installation directory..." 2>&1 | tee $LOGFILE >> /dev/null
cd $WORKING_DIR
if [[ ! -e zips ]] ; then
    mkdir zips
fi

cd $WORKING_DIR

REV=${BUILD##pic32-}

cd win32-image
# delete *.py  *-pic.o  *_pic.o files
find . -type f -name *.py -delete
find . -name *-gdb.py -delete
find . -name *_pic.o -delete
find . -name *-pic.o -delete
tar -czvf $WORKING_DIR/zips/pic32-tools-$REV-win32-image.tar.gz pic32-tools

cd ../$NATIVEIMAGE
# delete *.py  files
find . -type f -name *.py -delete
find . -name *-gdb.py -delete
find . -name *_pic.o -delete
find . -name *-pic.o -delete
tar -czvf $WORKING_DIR/zips/pic32-tools-$REV-$NATIVEIMAGE.tar.gz pic32-tools

cd ../export-image
# delete *.py files
find . -type f -name *.py -delete
find . -name *-gdb.py -delete
find . -name *_pic.o -delete
find . -name *-pic.o -delete
tar -czvf $WORKING_DIR/zips/pic32-tools-$REV-export-image.tar.gz pic32-tools

cd ../arm-linux-image
# delete *.py files
find . -type f -name *.py -delete
find . -name *-gdb.py -delete
find . -name *_pic.o -delete
find . -name *-pic.o -delete
tar -czvf $WORKING_DIR/zips/pic32-tools-$REV-arm-linux-image.tar.gz pic32-tools

cd ..
if [ "x$LINUX32IMAGE" != "x" ]; then
    cd $LINUX32IMAGE
        # delete *.py files
    find . -type f -name *.py -delete
    find . -name *-gdb.py -delete
    find . -name *_pic.o -delete
    find . -name *-pic.o -delete
    tar -czvf $WORKING_DIR/zips/pic32-tools-$REV-$LINUX32IMAGE.tar.gz pic32-tools
    cd ..
fi

shasum -a 256 $WORKING_DIR/zips/pic32-tools-$REV-win32-image.tar.gz > $WORKING_DIR/zips/pic32-tools-$REV-win32-image.sha256
shasum -a 256 $WORKING_DIR/zips/pic32-tools-$REV-$NATIVEIMAGE.tar.gz > $WORKING_DIR/zips/pic32-tools-$REV-$NATIVEIMAGE.sha256
shasum -a 256 $WORKING_DIR/zips/pic32-tools-$REV-export-image.tar.gz > $WORKING_DIR/zips/pic32-tools-$REV-export-image.sha256
shasum -a 256 $WORKING_DIR/zips/pic32-tools-$REV-arm-linux-image.tar.gz > $WORKING_DIR/zips/pic32-tools-$REV-arm-linux-image.sha256
shasum -a 256 $WORKING_DIR/zips/pic32-tools-$REV-$LINUX32IMAGE.tar.gz > $WORKING_DIR/zips/pic32-tools-$REV-$LINUX32IMAGE.sha256


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
echo `date` " DONE..." 2>&1 | tee $LOGFILE >> /dev/null
echo DONE.
status_update "DONE"

exit 0
