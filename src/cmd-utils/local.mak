#         -*- makefile -*-
# This makefile template was createdy at Mon Aug  2 09:39:37 2004
# command line: ./build_init --package ldap_slp --version 0,1,1 --rootdir /home/mdday/src/ldap_slp --subdir src --subdir src/kernel --subdir src/cmd-utils --subdir src/cmd-utils/slp_client --subdir src/cmd_utils/slp_query --subdir src/cmd_utils/slp_srvreg --subdir src/cmd_utils/slp_attrreq 
# This file is included into the build by the actual Makefile.


# This is the local makefile template. It is read into the actual Makefile. 
# You need to define your targets in this file. Note that you must define 
# values for the variables that you want built by this make file.


# If there are subdirectories under this one, and you want those subdirectories
# to be included in one of the recursive make targets, you must define it here. 
# 
SUBDIRS = slp_client \
	slp_query \
	slp_srvreg \
	slp_attrreq

ifeq ($(HOST_OS), aix)

# define the aix targets here 
PROGRAMS = 
OBJS =
LIBS = 
SLIBS = 
TESTS = 
DOCS =

endif 

ifeq ($(HOST_OS), linux)

# define the linux targets here 
PROGRAMS = 
OBJS =
LIBS = 
SLIBS = 
TESTS = 
DOCS =

endif 

ifeq ($(HOST_OS), windows)
# define the windows targets here 
PROGRAMS = 
OBJS = 
LIBS = 
SLIBS = 
TESTS = 
DOCS =
endif 

# these are the targets that can be made by the actual Makefile 
all: $(PROGRAMS) $(LIBS) $(SLIBS) $(TESTS) $(DOCS) test Makefile
lib: $(LIBS) $(SLIBS) 
bin: $(PROGRAMS)


ifeq ($(HOST_OS), aix)

# implicit pattern-based rules, not sufficient for most projects
% : %.o
	$(CC) $(LDFLAGS) $(CPPFLAGS) -bI:$(libdir)/as400_libc.exp $< -o $@ 

%.o : %.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS) -I$(top_builddir)$(OS_PORT_DIR) $< -o $@ 

# put your actual Makefile rules for aix in here


endif

ifeq ($(HOST_OS), linux)

# implicit pattern-based rules, not sufficient for most projects
% : %.o
	$(CC) $(LDFLAGS) $(CPPFLAGS) $< -o $@

%.o : %.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $< -o $@

# put your actual Makefile rules for linux in here


endif


ifeq ($(HOST_OS), windows)
# implicit pattern-based rules, not sufficient for most projects
%.exe : %.obj
	cl $(LDFLAGS) $< /Fe$@ $(LINKER_OPTIONS)
%.obj : %.c
	cl $(CPPFLAGS) $(CFLAGS) $< /Fo$@

# put your actual Makefile rules for windows in here 

endif


