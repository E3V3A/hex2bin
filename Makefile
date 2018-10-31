# Makefile hex2bin/mot2bin

OS := $(shell uname)

ifeq ($(OS),Darwin)
LIBARCHS = i386 x86_64
PREFIX ?= /usr/local
endif

ifeq ($(OS),CYGWIN_NT-6.3)
LIBARCHS = i386 x86_64
PREFIX ?= /usr/local
MAN_DIR ?= /usr/man/man1
endif


SFLAGS = -static -static-libgcc -static-libstdc++
# If you want a statically linked executable, insclude the flags above:
CPFLAGS = -std=c99 -O2 -Wall -pedantic $(SFLAGS)

# Compile
%.o : %.c
	gcc -c $(CPFLAGS) $< -o $@

#WIN64_GCC = x86_64-w64-mingw32-gcc
WINDOWS = i686-w64-mingw32
WIN_GCC = $(WINDOWS)-gcc
WIN_STRIP = $(WINDOWS)-strip

INSTALL_DIR = /usr/local

# Linux uses:
#MAN_DIR = $(INSTALL_DIR)/man/man1
# Cygwin uses:
MAN_DIR = /usr/man/man1

all: hex2bin mot2bin hex2bin.1

hex2bin.1: hex2bin.pod
	pod2man hex2bin.pod > hex2bin.1

hex2bin: hex2bin.o common.o libcrc.o binary.o
	gcc -O2 -Wall -o hex2bin hex2bin.o common.o libcrc.o binary.o

mot2bin: mot2bin.o common.o libcrc.o binary.o
	gcc -O2 -Wall -o mot2bin mot2bin.o common.o libcrc.o binary.o

windows:
#	$(WIN32_GCC) $(CPFLAGS) -o Win32/hex2bin.exe hex2bin.c common.c libcrc.c binary.c
#	$(WIN32_GCC) $(CPFLAGS) -o Win32/mot2bin.exe mot2bin.c common.c libcrc.c binary.c
	$(WIN_GCC) $(CPFLAGS) -o Win64/hex2bin.exe hex2bin.c common.c libcrc.c binary.c
	$(WIN_GCC) $(CPFLAGS) -o Win64/mot2bin.exe mot2bin.c common.c libcrc.c binary.c
	$(WIN_STRIP) Win64/hex2bin.exe
	$(WIN_STRIP) Win64/mot2bin.exe

install:
	strip hex2bin
	strip mot2bin
	cp hex2bin mot2bin $(INSTALL_DIR)/bin
	cp hex2bin.1 $(MAN_DIR)

clean:
	rm core *.o hex2bin mot2bin hex2bin.exe mot2bin.exe
