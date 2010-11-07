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
SUBDIRS =
ifeq ($(HOST_OS), aix)

# define the aix targets here 
PROGRAMS = 
OBJS = slp_client.o \
	$(top_builddir)$(OS_PORT_DIR)/$(OS_PORT_OBJ) \
	$(top_builddir)$(OS_PORT_DIR)/os400_qslputil.o \
	y_attr.o \
	l_attr.o \
	y_url.o \
	l_url.o \
	y_filter.o \
	l_filter.o 

LIBS = libslp_client.a
SLIBS = libslp_client.so
TESTS = 
DOCS =

endif 

ifeq ($(HOST_OS), linux)

# define the linux targets here 
PROGRAMS = 
OBJS = slp_client.o \
	$(abs_builddir)/../$(OS_PORT_OBJ) \
	y_attr.o \
	l_attr.o \
	y_url.o \
	l_url.o \
	y_filter.o \
	l_filter.o \
	slp_ipv6.o

LIBS = libslp_client.a 
SLIBS = libslp_client.so
TESTS = 
DOCS =

endif 

ifeq ($(HOST_OS), windows)
# define the windows targets here 
PROGRAMS = 
OBJS = slp_client.obj \
	..\\$(OS_PORT_OBJ) \
	y_attr.obj \
	l_attr.obj \
	y_url.obj \
	l_url.obj \
	y_filter.obj \
	l_filter.obj \
	slp_ipv6.obj
LIBS = 
SLIBS = slp_client.dll
TESTS = 
DOCS =
endif 


#library version
MAJOR_VERSION=4
MINOR_VERSION=4
RELEASE_VERSION=a


# these are the targets that can be made by the actual Makefile 
all: $(PROGRAMS) $(LIBS) $(SLIBS) $(TESTS) $(DOCS) test Makefile
lib: $(LIBS) $(SLIBS) 
bin: $(PROGRAMS)

install:
ifeq ($(HOST_OS),aix)	
	@echo "installing $(LIBS) $(SLIBS) in $(OS_PORT_PASEDIR) and adding symbolic links in $(libdir)"
	$(INSTALL) -M 0664 -O QSYS -G 0 -f $(OS_PORT_PASEDIR) libslp_client.a
	$(INSTALL) -M 0644 -O QSYS -G 0 -f $(OS_PORT_PASEDIR) libslp_client.so
	ln -f -s $(OS_PORT_PASEDIR)/libslp_client.a $(libdir)/libslp_client.a 
	ln -f -s $(OS_PORT_PASEDIR)/libslp_client.so $(libdir)/libslp_client.so
endif

ifeq ($(HOST_OS),linux)	
	@echo "installing slp client library in $(libdir)"
	rm -fv $(libdir)/libslp_client.so.$(MAJOR_VERSION)
	rm -fv $(libdir)/libslp_client.so
	$(INSTALL) -v libslp_client.a -m0664 $(libdir)/libslp_client.a
	$(INSTALL) -v libslp_client.so.$(MAJOR_VERSION).$(MINOR_VERSION).$(RELEASE_VERSION) -m0644 $(libdir)/libslp_client.so.$(MAJOR_VERSION).$(MINOR_VERSION).$(RELEASE_VERSION)
	ln --force -s $(libdir)/libslp_client.so.$(MAJOR_VERSION).$(MINOR_VERSION).$(RELEASE_VERSION) $(libdir)/libslp_client.so.$(MAJOR_VERSION)
	ln --force -s $(libdir)/libslp_client.so.$(MAJOR_VERSION).$(MINOR_VERSION).$(RELEASE_VERSION) $(libdir)/libslp_client.so
	$(LDCONFIG) -n $(libdir)
endif

ifeq ($(HOST_OS), windows)	
	@echo "installing slp client library in $(libdir)"
	cp -v slp_client.dll $(libdir)\\slp_client.dll
	cp -v slp_client.lib $(libdir)\\slp_client.lib
	cp -v slp_client.exp $(libdir)\\slp_client.lib
endif 

ifeq ($(HOST_OS),aix)
libslp_client.a: $(OBJS)
	ar rcs $@ $(OBJS)

libslp_client.so: $(OBJS)
	$(CC) -qmkshrobj -bI:$(libdir)/as400_libc.exp -o libslp_client.so $(OBJS) -lc 

%.o : %.c
	$(CC) $(NO_LINK_FLAG) $(CFLAGS) $(DEFS) -I$(top_builddir)$(OS_PORT_DIR) $< 

$(abs_builddir)/../$(OS_PORT_OBJ) : $(abs_builddir)/../$(OS_PORT_SRC) 
	$(CC) $(NO_LINK_FLAG) $(CFLAGS) $(DEFS) -I$(top_builddir)$(OS_PORT_DIR) $(OBJ_OUT) $@ $< 


# implicit pattern-based rules, not sufficient for most projects


endif

ifeq ($(HOST_OS),linux)

.PHONY : changeset

# if this is a development mercurial tree, update changeset.h
changeset :
	if [[ -e $(abs_top_srcdir)/.hg && \
	-e $(abs_top_srcdir)/version.sh  && \
	-x $(abs_top_srcdir)/version.sh ]] ; then \
		cd $(abs_top_srcdir) && ./version.sh; \
	fi

libslp_client.a: $(OBJS)
	ar rcs $@ $(OBJS)

libslp_client.so: $(OBJS)
	$(CC) -shared -Wl,-soname,libslp_client.so.$(MAJOR_VERSION) \
	-o libslp_client.so.$(MAJOR_VERSION).$(MINOR_VERSION).$(RELEASE_VERSION) $(OBJS) -lc

%.o : %.c changeset 
	$(CC) $(NO_LINK_FLAG) $(CFLAGS) $(DEFS) $< 

$(abs_builddir)/../$(OS_PORT_OBJ) : $(abs_builddir)/../$(OS_PORT_SRC) 
	$(CC) $(NO_LINK_FLAG) $(CFLAGS) $(DEFS) $(OBJ_OUT) $@ $< 



# implicit pattern-based rules, not sufficient for most projects


endif

ifeq ($(HOST_OS), windows)
# implicit pattern-based rules, not sufficient for most projects

slp_client.dll : $(OBJS)
	link $(LDFLAGS) /OUT:$@ /DLL /LIBPATH:"$(LIB)" $(OBJS) Ws2_32.lib Iphlpapi.lib
%.obj : %.c
	cl $(CPPFLAGS) $(CFLAGS)  $< /Fo$@

# put your actual Makefile rules for windows in here 
..\\lslp-windows.obj : ..\\lslp-windows.c
	cl $(CPPFLAGS) $(CFLAGS)  $< /Fo$@

endif

l_attr.c : attr.l
	$(LEX) -p -v -o$@ -P$(basename $<) $<
y_attr.c : attr.y 
	$(YACC) -o$@ -p$(basename $<) -v -d $<
l_url.c : url.l
	$(LEX) -p -v -o$@ -P$(basename $<) $<
y_url.c : url.y
	$(YACC) -o$@ -p$(basename $<) -v -d $<
l_filter.c : filter.l 
	$(LEX) -p -v -o$@ -P$(basename $<) $<
y_filter.c : filter.y
	$(YACC) -o$@ -p$(basename $<) -v -d $<

