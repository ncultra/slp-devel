#         -*- makefile -*-
# This makefile template was createdy at Mon Aug  2 10:08:55 2004
# command line: ./build_init --package ldap_slp --version 0,1,1 --rootdir /home/mdday/src/ldap_slp --subdir src --subdir src/kernel --subdir src/cmd-utils --subdir src/cmd-utils/slp_client --subdir src/cmd-utils/slp_query --subdir src/cmd-utils/slp_srvreg --subdir src/cmd-utils/slp_attrreq 
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
PROGRAMS = slp_srvreg
OBJS = 	slp_srvreg.o
LIBS = 
SLIBS = 
TESTS = 
DOCS =

endif 

ifeq ($(HOST_OS), linux)

# define the linux targets here 
PROGRAMS = slp_srvreg
OBJS = slp_srvreg.o
LIBS = 
SLIBS = $(libdir)/libslp_client.so.4
TESTS = 
DOCS =

endif 

ifeq ($(HOST_OS), windows)
# define the windows targets here 
PROGRAMS = slp_srvreg.exe
OBJS = slp_srvreg.obj
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
ifeq ($(HOST_OS), aix)
	@echo "installing $(PROGRAMS) in $(OS_PORT_PASEDIR) and adding sybolic links in $(bindir)"
	$(INSTALL) -M 0755 -O QSYS -G 0 -f $(OS_PORT_PASEDIR) $(PROGRAMS)
	ln -f -s $(OS_PORT_PASEDIR)/$(PROGRAMS) $(bindir)/$(PROGRAMS) 

endif 
ifeq ($(HOST_OS), linux)
	echo "installing slp_srvreg in $(bindir)"
	$(INSTALL) -v -b slp_srvreg -m0755 $(bindir)/slp_srvreg

endif 
ifeq ($(HOST_OS), windows)	
	@echo "installing $(PROGRAMS) in $(bindir)"
	cp -v $(PROGRAMS) $(bindir)\\$(PROGRAMS)
endif 

ifeq ($(HOST_OS), aix)

# implicit pattern-based rules, not sufficient for most projects
slp_srvreg : $(OBJS)
	$(CC) $(LDFLAGS) -bI:$(libdir)/as400_libc.exp $(libdir)/libslp_client.so -o $@ $(OBJS) 
	@echo 
	@echo "--------------------------------------------------------------"
	@echo "NOTICE: $(PROGRAMS) uses a shared library located in $(libdir)"
	@echo "make certain your ld.so.conf or LD_LIBRARY_PATH include $(libdir)"
	@echo "--------------------------------------------------------------"
%.o : %.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS) -I$(top_builddir)$(OS_PORT_DIR) $< -o $@ 

# put your actual Makefile rules for aix in here


endif

ifeq ($(HOST_OS), linux)

.PHONY : changeset

# if this is a development mercurial tree, update changeset.h
changeset :
	if [[ -e $(abs_top_srcdir)/.hg && \
	-e $(abs_top_srcdir)/version.sh  && \
	-x $(abs_top_srcdir)/version.sh ]] ; then \
		cd $(abs_top_srcdir) && ./version.sh; \
	fi

$(SLIBS) :
	@cd ../slp_client && make && sudo make install

# implicit pattern-based rules, not sufficient for most projects
slp_srvreg : $(SLIBS) $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(SLIBS)
	@echo 
	@echo "--------------------------------------------------------------"
	@echo "NOTICE: $(PROGRAMS) uses a shared library located in $(libdir)"
	@echo "make certain your ld.so.conf or LD_LIBRARY_PATH include $(libdir)"
	@echo "--------------------------------------------------------------"
%.o : %.c changeset
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $< -o $@

# put your actual Makefile rules for linux in here


endif

ifeq ($(HOST_OS), windows)

slp_srvreg.exe: $(OBJS)
	link $(LDFLAGS) /OUT:$@ /LIBPATH:"$(LIB)"  $(OBJS) Ws2_32.lib ..//slp_client//slp_client.lib
%.obj : %.c
	cl $(CPPFLAGS) $(CFLAGS) /DSLP_LIB_IMPORT=1 $< /Fo$@
endif


