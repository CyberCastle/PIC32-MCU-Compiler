STRIP=strip
SLASH=/
CDEF=-DCHECK_PATH 

.PHONY: force_install

BUILD_OS := $(shell uname | sed 's/[_\.].*//')

ifeq ($(BUILD_OS),CYGWIN)
# building on CYGWIN for CYGWIN
EXE=.exe
CC=gcc
endif

ifeq ($(OSTYPE),solaris2.6)
# building on SOLARIS for SOLARIS
EXE=
CC=gcc
STRIP=strip
endif

ifeq ($(OSTYPE),solaris)
# building on SOLARIS for SOLARIS; using something other than bash
EXE=
CC=gcc
STRIP=strip
endif


ifeq ($(TARGET), mingw)
# building on HOST for mingw
CC=i586-pc-mingw32-gcc -I /usr/include/w32api
STRIP=mingw32-strip
CDEF=-DCHECK_PATH -DMINGW
LINK=-L /usr/lib/w32api
ifeq ($(BUILD_OS), CYGWIN)
SLASH=\\
endif
ifeq ($(BUILD_OS), Linux)
  TMINGW32 := $(shell which i386-mingw32-gcc )
  ifeq ($(TMINGW32), )
    TMINGW32 := $(shell which i586-mingw32msvc-gcc )
    ifeq ($(TMINGW32), )
      CC=i686-w64-mingw32-gcc
      STRIP=i686-w64-mingw32-ar
    else
      CC=i586-mingw32msvc-gcc
      STRIP=i586-mingw32msvc-strip
    endif
  else 
    CC=i386-mingw32-gcc
    STRIP=i386-mingw32-strip
  endif
EXE=.exe
endif # build os Linux
ifeq ($(BUILD_OS), Darwin)
CC=i586-mingw32-gcc
STRIP=i586-mingw32-strip
EXE=.exe
endif
endif # target Mingw

ifeq ($(BUILD_OS), Darwin)
  MACOS_DEVELOPERDIR=/Library/Developer/CommandLineTools
  EXTRAOPS=-isysroot $(MACOS_DEVELOPERDIR)/SDKs/MacOSX.sdk -mmacosx-version-min=10.14 -m64
  ifeq ($(TARGET), linux)
    CC=i586-pc-linux-gcc
    STRIP=i586-pc-linux-strip
    EXTRAOPS=
  endif
  ifeq ($(TARGET), mingw)
    EXTRAOPS=
  endif

  ifeq ($(TARGET), arm)
  	CC=arm-none-linux-gnueabi-gcc
  	STRIP=arm-none-linux-gnueabi-strip
  	EXTRAOPS=
  endif
endif #build os Darwin

test = $(OSTYPE) $(TARGET)
ifndef test
# building on HOST for HOST
EXE=
CC=gcc
STRIP=strip
endif

COPT = -c -Wall -g -I..$(SLASH)include -DMCHP_VERSION=$(MCHP_VERSION)
OBJ= pic30-sh.o linker.o
OBJ32= xc32-sh.o xc32-linker.o

%.o : ../src/%.c
	@echo $@
	$(CC) $(EXTRAOPS) $(COPT) $(CDEF) ..$(SLASH)src$(SLASH)$(<F)

all: xc16 xc32

xc16: pic30-sh$(EXE)
xc32: xc32-sh$(EXE)

pic30-sh$(EXE): $(OBJ)
	@echo $@
	$(CC) -g -o $@ $(OBJ) $(EXTRAOPS) $(LINK)

xc32-sh$(EXE): $(OBJ32)
	@echo $@
	$(CC) -g -o $@ $(OBJ32) $(EXTRAOPS) $(LINK) 

clean:
	-rm -f $(OBJ) $(OBJ32) pic30-sh$(EXE) xc32-sh$(EXE)

pic30-install: pic30-sh$(EXE) force_install
	@if [ "$(PIC30_CD)" != "" -a -d $(PIC30_CD)/bin ]; then \
	  cp -v pic30-sh$(EXE) $(PIC30_CD)/bin/pic30-ar$(EXE); \
	  cp -v pic30-sh$(EXE) $(PIC30_CD)/bin/pic30-as$(EXE); \
	  cp -v pic30-sh$(EXE) $(PIC30_CD)/bin/pic30-bin2hex$(EXE); \
	  cp -v pic30-sh$(EXE) $(PIC30_CD)/bin/pic30-cc1$(EXE); \
	  cp -v pic30-sh$(EXE) $(PIC30_CD)/bin/pic30-gcc$(EXE); \
	  cp -v pic30-sh$(EXE) $(PIC30_CD)/bin/pic30-ld$(EXE); \
	  cp -v pic30-sh$(EXE) $(PIC30_CD)/bin/pic30-nm$(EXE); \
	  cp -v pic30-sh$(EXE) $(PIC30_CD)/bin/pic30-objdump$(EXE); \
	  cp -v pic30-sh$(EXE) $(PIC30_CD)/bin/pic30-pa$(EXE); \
	  cp -v pic30-sh$(EXE) $(PIC30_CD)/bin/pic30-ranlib$(EXE); \
	  cp -v pic30-sh$(EXE) $(PIC30_CD)/bin/pic30-strip$(EXE); \
	else \
	   echo "Please set PIC30_CD and make PIC30_CD/bin available."; \
	   exit 1; \
        fi

xc16-install: pic30-sh$(EXE) force_install
	@if [ "$(PIC30_CD)" != "" -a -d $(PIC30_CD)/bin ]; then \
	  cp -v pic30-sh$(EXE) $(PIC30_CD)/bin/xc16-ar$(EXE); \
	  cp -v pic30-sh$(EXE) $(PIC30_CD)/bin/xc16-as$(EXE); \
	  cp -v pic30-sh$(EXE) $(PIC30_CD)/bin/xc16-bin2hex$(EXE); \
	  cp -v pic30-sh$(EXE) $(PIC30_CD)/bin/xc16-cc1$(EXE); \
	  cp -v pic30-sh$(EXE) $(PIC30_CD)/bin/xc16-gcc$(EXE); \
	  cp -v pic30-sh$(EXE) $(PIC30_CD)/bin/xc16-ld$(EXE); \
	  cp -v pic30-sh$(EXE) $(PIC30_CD)/bin/xc16-nm$(EXE); \
	  cp -v pic30-sh$(EXE) $(PIC30_CD)/bin/xc16-objdump$(EXE); \
	  cp -v pic30-sh$(EXE) $(PIC30_CD)/bin/xc16-pa$(EXE); \
	  cp -v pic30-sh$(EXE) $(PIC30_CD)/bin/xc16-ranlib$(EXE); \
	  cp -v pic30-sh$(EXE) $(PIC30_CD)/bin/xc16-strip$(EXE); \
	else \
	  echo "Please set PIC30_CD and make PIC30_CD/bin available."; \
	  exit 1; \
	fi

xc32-install: xc32-sh$(EXE) force_install
	@if [ "$(XC32_INSTALL)" != "" -a -d $(XC32_INSTALL)/bin ]; then \
          $(STRIP) xc32-sh$(EXE); \
	  cp xc32-sh$(EXE) $(XC32_INSTALL)/bin/xc32-addr2line$(EXE); \
	  cp xc32-sh$(EXE) $(XC32_INSTALL)/bin/xc32-ar$(EXE); \
	  cp xc32-sh$(EXE) $(XC32_INSTALL)/bin/xc32-as$(EXE); \
	  cp xc32-sh$(EXE) $(XC32_INSTALL)/bin/xc32-bin2hex$(EXE); \
	  cp xc32-sh$(EXE) $(XC32_INSTALL)/bin/xc32-c++$(EXE); \
	  cp xc32-sh$(EXE) $(XC32_INSTALL)/bin/xc32-c++filt$(EXE); \
	  cp xc32-sh$(EXE) $(XC32_INSTALL)/bin/xc32-cc1$(EXE); \
	  cp xc32-sh$(EXE) $(XC32_INSTALL)/bin/xc32-cc1plus$(EXE); \
	  cp xc32-sh$(EXE) $(XC32_INSTALL)/bin/xc32-collect2$(EXE); \
	  cp xc32-sh$(EXE) $(XC32_INSTALL)/bin/xc32-conv$(EXE); \
	  cp xc32-sh$(EXE) $(XC32_INSTALL)/bin/xc32-cpp$(EXE); \
	  cp xc32-sh$(EXE) $(XC32_INSTALL)/bin/xc32-elfedit$(EXE); \
	  cp xc32-sh$(EXE) $(XC32_INSTALL)/bin/xc32-g++$(EXE); \
	  cp xc32-sh$(EXE) $(XC32_INSTALL)/bin/xc32-gcc$(EXE); \
	  cp xc32-sh$(EXE) $(XC32_INSTALL)/bin/xc32-ld$(EXE); \
	  cp xc32-sh$(EXE) $(XC32_INSTALL)/bin/xc32-nm$(EXE); \
	  cp xc32-sh$(EXE) $(XC32_INSTALL)/bin/xc32-objcopy$(EXE); \
	  cp xc32-sh$(EXE) $(XC32_INSTALL)/bin/xc32-objdump$(EXE); \
	  cp xc32-sh$(EXE) $(XC32_INSTALL)/bin/xc32-ranlib$(EXE); \
	  cp xc32-sh$(EXE) $(XC32_INSTALL)/bin/xc32-readelf$(EXE); \
	  cp xc32-sh$(EXE) $(XC32_INSTALL)/bin/xc32-size$(EXE); \
	  cp xc32-sh$(EXE) $(XC32_INSTALL)/bin/xc32-strings$(EXE); \
	  cp xc32-sh$(EXE) $(XC32_INSTALL)/bin/xc32-strip$(EXE); \
	else \
	  echo "Please set XC32_INSTALL and make XC32_INSTALL/bin available."; \
	  exit 1; \
	fi

pic32-compiler-install: xc32-sh$(EXE) force_install
	@if [ "$(XC32_INSTALL)" != "" -a -d $(XC32_INSTALL)/bin ]; then \
          $(STRIP) xc32-sh$(EXE); \
	  cp xc32-sh$(EXE) $(XC32_INSTALL)/bin/pic32-addr2line$(EXE); \
	  cp xc32-sh$(EXE) $(XC32_INSTALL)/bin/pic32-ar$(EXE); \
	  cp xc32-sh$(EXE) $(XC32_INSTALL)/bin/pic32-as$(EXE); \
	  cp xc32-sh$(EXE) $(XC32_INSTALL)/bin/pic32-bin2hex$(EXE); \
	  cp xc32-sh$(EXE) $(XC32_INSTALL)/bin/pic32-c++$(EXE); \
	  cp xc32-sh$(EXE) $(XC32_INSTALL)/bin/pic32-c++filt$(EXE); \
	  cp xc32-sh$(EXE) $(XC32_INSTALL)/bin/pic32-cc1$(EXE); \
	  cp xc32-sh$(EXE) $(XC32_INSTALL)/bin/pic32-cc1plus$(EXE); \
	  cp xc32-sh$(EXE) $(XC32_INSTALL)/bin/pic32-collect2$(EXE); \
	  cp xc32-sh$(EXE) $(XC32_INSTALL)/bin/pic32-conv$(EXE); \
	  cp xc32-sh$(EXE) $(XC32_INSTALL)/bin/pic32-cpp$(EXE); \
	  cp xc32-sh$(EXE) $(XC32_INSTALL)/bin/pic32-elfedit$(EXE); \
	  cp xc32-sh$(EXE) $(XC32_INSTALL)/bin/pic32-g++$(EXE); \
	  cp xc32-sh$(EXE) $(XC32_INSTALL)/bin/pic32-gcc$(EXE); \
	  cp xc32-sh$(EXE) $(XC32_INSTALL)/bin/pic32-gdb$(EXE); \
	  cp xc32-sh$(EXE) $(XC32_INSTALL)/bin/pic32-ld$(EXE); \
	  cp xc32-sh$(EXE) $(XC32_INSTALL)/bin/pic32-nm$(EXE); \
	  cp xc32-sh$(EXE) $(XC32_INSTALL)/bin/pic32-objcopy$(EXE); \
	  cp xc32-sh$(EXE) $(XC32_INSTALL)/bin/pic32-objdump$(EXE); \
	  cp xc32-sh$(EXE) $(XC32_INSTALL)/bin/pic32-ranlib$(EXE); \
	  cp xc32-sh$(EXE) $(XC32_INSTALL)/bin/pic32-readelf$(EXE); \
	  cp xc32-sh$(EXE) $(XC32_INSTALL)/bin/pic32-size$(EXE); \
	  cp xc32-sh$(EXE) $(XC32_INSTALL)/bin/pic32-strings$(EXE); \
	  cp xc32-sh$(EXE) $(XC32_INSTALL)/bin/pic32-strip$(EXE); \
	else \
	  echo "Please set XC32_INSTALL and make XC32_INSTALL/bin available."; \
	  exit 1; \
	fi
 

install: pic30-install xc16-install
