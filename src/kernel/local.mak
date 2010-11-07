#         -*- makefile -*-
# This makefile template was createdy at Wed Jul 28 10:24:56 2004
# command line: ./build_init --package ldap_slp --version 0,1,1 --rootdir /home/mdday/src/ldap_slp --subdir src --subdir src/kernel --subdir src/cmd_util --subdir src/cmd-util/slp_client --subdir src/cmd_util/slp_query --subdir src/cmd_util/slp_srvreg --subdir src/cmd_util/slp_attrreq 
# This file is included into the build by the actual Makefile.


# This is the local makefile template. It is read into the actual Makefile. 
# You need to define your targets in this file. Note that you must define 
# values for the variables that you want built by this make file.


# If there are subdirectories under this one, and you want those subdirectories
# to be included in one of the recursive make targets, you must define it here. 
# 
SUBDIRS =
ifeq ($(HOST_OS), aix)


# define the aix targets here 
PROGRAMS = lslp-kernel
OBJS =  lslp-kernel.o \
	lslp-config.o \
	rtt.o \
	lslp-aix.o \
	$(top_builddir)$(OS_PORT_DIR)/os400_qslputil.o \
	lslp-crypt.o \
	lslp-da-sa.o \
	lslp-msg.o \
	lslp-web.o \
	l_opaque.o \
	y_url.o l_url.o \
	y_attr.o l_attr.o \
	y_filter.o l_filter.o \
	mslp.o
LIBS = 
SLIBS = 
TESTS = 
DOCS =
CONF = slp.conf

endif 

ifeq ($(HOST_OS), linux)


# define the linux targets here 
PROGRAMS = lslp-kernel
OBJS =  lslp-kernel.o \
	lslp-config.o \
	rtt.o \
	lslp-linux.o \
	lslp-crypt.o \
	lslp-da-sa.o \
	lslp-msg.o \
	lslp-web.o \
	l_opaque.o \
	y_url.o l_url.o \
	y_attr.o l_attr.o \
	y_filter.o l_filter.o \
	mslp.o
LIBS = 
SLIBS = 
TESTS = 
DOCS =

endif 


ifeq ($(HOST_OS), windows)
# define the windows targets here 
PROGRAMS = lslp-kernel.exe
OBJS =  lslp-kernel.obj \
	lslp-config.obj \
	rtt.obj \
	lslp-windows.obj \
	lslp-crypt.obj \
	lslp-da-sa.obj \
	lslp-msg.obj \
	lslp-web.obj \
	l_opaque.obj \
	y_url.obj l_url.obj \
	y_attr.obj l_attr.obj \
	y_filter.obj l_filter.obj \
	mslp.obj
LIBS = 
SLIBS = 
TESTS = 
DOCS =
endif 

# these are the targets that can be made by the actual Makefile 
all: $(PROGRAMS) $(LIBS) $(SLIBS) $(TESTS) $(DOCS) test Makefile
lib: $(LIBS) $(SLIBS) 
bin: $(PROGRAMS)

install:
ifeq ($(HOST_OS),aix)	
	@echo "installing $(PROGRAMS) in $(OS_PORT_PASEDIR) and adding symbolic links in $(bindir)"
	$(INSTALL) -M 0755 -O QSYS -G 0 -f $(OS_PORT_PASEDIR) $(PROGRAMS)
	$(INSTALL) -o -M 0755 -O QSYS -G 0 -f $(OS_PORT_PASEDIR) $(CONF)
	ln -f -s $(OS_PORT_PASEDIR)/$(PROGRAMS) $(bindir)/$(PROGRAMS) 
	ln -f -s $(OS_PORT_PASEDIR)/$(CONF) $(bindir)/$(CONF)
endif


ifeq ($(HOST_OS), aix)

lslp-kernel : $(OBJS)
	$(CC) $(LDFLAGS) $(REENTRANT_FLAG) $(THREAD_LIBS) -bI:$(libdir)/as400_libc.exp -o lslp-kernel $(OBJS) 

%.o : %.c 
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $(REENTRANT_FLAG) $(THREAD_LIBS) -I$(top_builddir)$(OS_PORT_DIR) $< -o $@ 

endif

ifeq ($(HOST_OS), linux)

lslp-kernel : $(OBJS)
	$(CC) $(LDFLAGS) $(REENTRANT_FLAG) -o lslp-kernel $(OBJS) -lpthread 

%.o : %.c 
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $(REENTRANT_FLAG) $< -o $@

endif

ifeq ($(HOST_OS), windows)

lslp-kernel.exe : $(OBJS)
	link  /OUT:$@ /LIBPATH:"$(LIB)" /DEFAULTLIB:LIBCMT.lib $(OBJS) Ws2_32.lib Advapi32.lib.  
%.obj : %.c
	cl $(CPPFLAGS) $(CFLAGS) /MT $< /Fo$@

# put your actual Makefile rules for windows in here 

endif

l_url.c : url.l 
	$(LEX) -p -v -o$@ -P$(basename $<) $<
y_url.c : url.y 
	$(YACC) -o$@ -p$(basename $<) -v -d $<

l_attr.c : attr.l 
	$(LEX) -p -v -o$@ -P$(basename $<) $<
y_attr.c : attr.y 
	$(YACC) -o$@ -p$(basename $<) -v -d $<

l_filter.c : filter.l 
	$(LEX) -p -v -o$@ -P$(basename $<) $<
y_filter.c : filter.y 
	$(YACC) -o$@ -p$(basename $<) -v -d $<

l_opaque.c : opaque.l 
	$(LEX) -p -v -o$@ -P$(basename $<) $<

