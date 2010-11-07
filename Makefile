VPATH = .

COMPONENTS = all
CLEAN = clean
INSTALL = install


## capture the appropriate environment variables to 
## control the build process
##
export BUILD_ROOT := /home/mdday/src/ldap_slp
export SRC_DIR := $(BUILD_ROOT)/src
export BUILD_DIR := $(shell pwd)
export WORDS_BIG_ENDIAN := @WORDS_BIG_ENDIAN@
export PACKAGE := slp
export PACKAGE_VERSION := 0.8 
export DEBUG := true
export LIBTOOL := /home/mdday/src/ldap_slp/libtool
export OBJ := .lo
export IGNORE := -
export LIB_PATH := -rpath
export LEX := flex
export YACC := bison
export STATIC_LINK_CMD := -static
export DLL_LINK_CMD := -module
export MV := mv
export MV_FLAGS := -f
export RM := rm
export RM_FLAGS := -f
export INSTALL_EXE := install
export INSTALL_FLAGS := 
export CP := cp
export CP_FLAGS := -f
export MAKE := gmake
export OS_PORT_INC := lslp-linux.h
export OS_PORT_SRC := lslp-linux.c
export OS_PORT_OBJ := lslp-linux.lo
export HOST_OS := linux
export LINKER := gcc
export EXE := 
export LIBS := 
export THREAD_LIBS := -lpthread
export REENTRANT_FLAG := -D_REENTRANT
export NO_LINK_FLAG := -c
export LIB_SUFFIX := .o
export DLL_SUFFIX := .so
export EXE_OUT := -o
export OBJ_OUT := -o
export LIB_INCLUDE_FLAG := -l
export CFLAGS := -g -Wall -Dlinux -DDEBUG_ALLOC
export LDFLAGS := -g
export DLL_LIB_FLAG := 
export LFLAGS := -p -v -d 
export YFLAGS := -v -d
export bindir := /bin
export sbindir := /sbin
export libexecdir := /lib
export sysconfdir := /etc
export libdir := /usr/lib
export PLATFORM := x86
export CC := gcc

export BINDIR := /bin
export SBINDIR := /sbin
export LIBEXECDIR := /lib
export SYSCONFIGDIR := /etc
export LIBDIR := /usr/lib

all: $(COMPONENTS)
.PHONY: all

clean: $(CLEAN)
.PHONY: clean

install: $(INSTALL)
.PHONY: install

test: $(TEST)
.PHONY: test


SRC_DIRS := $(SRC_DIR) 

$(INSTALL) :
	$(foreach dir, $(SRC_DIRS), cd $(dir) && $(MAKE) all;)
	$(foreach dir, $(SRC_DIRS), cd $(dir) && $(MAKE) $(INSTALL);)
$(COMPONENTS) $(CLEAN) :
	$(foreach dir, $(SRC_DIRS),cd $(dir) && $(MAKE) $@;)


