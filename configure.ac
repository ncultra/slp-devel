#                                               -*- Autoconf -*-
# This configuration template was created at Thu Nov  4 11:03:23 2004
# command line: ./build_init --package ldap_slp --version 0.1.4 --rootdir /home/jstevens/src/ldap_slp --subdir src --subdir src/port --subdir src/port/aix5 --subdir src/kernel --subdir src/cmd-utils --subdir src/cmd-utils/slp_client --subdir src/cmd-utils/slp_query --subdir src/cmd-utils/slp_srvreg --subdir src/cmd-utils/slp_attrreq 
# Process this file with autoconf to produce a configure script.

AC_INIT([ldap_slp],[0.2.9],[rc8])


#-------------------------- Initialization --------------------------#
# get the processor, vendor, and host information
AC_CANONICAL_BUILD

# create a time-date stamp that will be available in config.h	
config_time=`date`
AC_DEFINE_UNQUOTED([CONFIG_TIME], ["$config_time"], [Configuration Time])
	
# get the build host
config_host=$(hostname)'.'$(hostname -d)
AC_DEFINE_UNQUOTED([CONFIG_HOST], ["$config_host"], [Configuration Host])

AC_DEFINE_UNQUOTED([CONFIG_USER], ["$USER"], [Configuration User])

canonical_config="Configure/Build initiated by $USER@$config_host on $config_time"
AC_DEFINE_UNQUOTED([CANONICAL_CONFIG_STRING], ["$canonical_config"], [Canonical configuration string])

#AC_CONFIG_AUX_DIR(["/usr/share/automake-1.4/"])

#------------- define HOST_PROCESSOR, HOST_VENDOR, HOST_OS  ------------#
# see http://gcc.gnu.org/install/specific.html for a list of targets
# -----------------------------------------------------------------------

#---------------- Get the cononical build string and set host variables -#
AC_DEFINE_UNQUOTED([CANONICAL_BUILD], ["$build"], [Canonical Build])

if [[ -n "$PRODUCTION" ]] ; then 	
	AC_DEFINE_UNQUOTED([PRODUCTION_BUILD], ["TRUE"], [Build optimized code])

else 
	AC_DEFINE_UNQUOTED([DEBUG_BUILD], ["TRUE"], [Build for debugging])
fi 

if [[ -n "$TRACE" ]] ; then 

	AC_DEFINE_UNQUOTED([TRACE_ENABLED], ["TRUE"], [define trace macro])
fi

AC_SUBST([PRODUCTION_BUILD])
AC_SUBST([DEBUG_BUILD])
AC_SUBST([TRACE_ENABLED])


# This next three case expressions provide conditional code blocks for 
# the host processor, vendor, and operating system. 
# Most of the conditional configuration code is in the operating 
# system case expression. 

case $build in 
	i?86-*-*)
		AC_DEFINE([HOST_PROCESSOR], ["x86"], [Host Processor])	
		HOST_PROCESSOR=x86		
	;;	

	ia64-*-*)
		AC_DEFINE([HOST_PROCESSOR], ["ia64"], [Host Processor])	
		HOST_PROCESSOR=ia64
	;;
	'@(x86_64-*-* | amd64-*-*)' )
		AC_DEFINE([HOST_PROCESSOR], ["amd64"], [Host Processor])	
		HOST_PROCESSOR=amd64
	;;


	powerpc-*-*)
		AC_DEFINE([HOST_PROCESSOR], ["power"], [Host Processor])	
		HOST_PROCESSOR=power
	;;	

	powerpcle-*-*)
		AC_DEFINE([HOST_PROCESSOR], ["power_le"], [Host Processor])	
		HOST_PROCESSOR=power_le
	;;	

	s390-*-*)
		AC_DEFINE([HOST_PROCESSOR], ["390"], [Host Processor])	
		HOST_PROCESSOR=390
	;;
	s390x-*-*)
		AC_DEFINE([HOST_PROCESSOR], ["390_x"], [Host Processor])	
		HOST_PROCESSOR=390_x	
	;;
esac


case $build in 
	*-*-linux*)
		AC_DEFINE([HOST_OS], ["linux"], [Host OS])
		  HOST_OS=linux
		  AC_DEFINE([OS_PORT_INCLUDE], ["lslp-linux.h"], [Portability Header])
		  OS_PORT_INCLUDE=lslp-linux.h
		  OS_PORT_SRC=lslp-linux.c
		  OS_PORT_OBJ=lslp-linux.o

		AC_DEFINE([OS_PORT_DIR], ["src/port/linux"], [Portability Directory])
		OS_PORT_DIR="src/port/linux"

		  AC_PROG_CC()
		  LINKER=$CC
		  EXE=
		  LIBS=
		  LIB_INCLUDE_FLAG=-l
		  THREAD_LIBS=-lpthread
		  REENTRANT_FLAG=-D_REENTRANT
		  NO_LINK_FLAG=-c
		  LIB_SUFFIX=.o
		  DLL_SUFFIX=.so
		  EXE_OUT=-o
		  OBJ_OUT=-o
		  INSTALL_EXE=install
		  INSTALL_FLAGS=--mode=install
		
		  LIBTOOL=/usr/bin/libtool
		  LIBTOOL_MODE_COMPILE=--mode=compile
		  LIBTOOL_MODE_LINK=--mode=link
		  LIBTOOL_MODE_CLEAN=--mode=clean
		  LIBTOOL_MODE_INSTALL=--mode=install


# 	check see if the production variable is set 
	if [[ -z "$PRODUCTION" ]] ; then 	
	
		echo "Configuring Compiler for a debug build..."	

		CPPFLAGS=
		CFLAGS="-fPIC -g -Wall"	
		CXXFLAGS=$CFLAGS	
	
		echo "Configuring Linker for a debug build..."	
		LDFLAGS=-g
	
	else 	
		echo "Configuring Compiler for a production build..."
	
		CPPFLAGS=
		CFLAGS="-fPIC -g -Wall"	
		CXXFLAGS=$CFLAGS	
	
		echo "Configuring Linker for a production build..."		
		LDFLAGS=-s		
	fi 	
		
		AC_PROG_MAKE_SET

		# Checks for libraries.
#		AC_CHECK_LIB([dl], [dlopen])


		prefix=/usr/local
		exec_prefix=$prefix
		bindir=$prefix/bin
		sbindir=$prefix/sbin
		libexecdir=$sbindir
		libdir=$prefix/lib
  	        includedir=$prefix/include

	;;

	*-*-aix*)
		AC_DEFINE([HOST_OS], ["aix"], [Host OS])
		HOST_OS=aix
		AC_DEFINE([OS_PORT_INCLUDE], ["lslp-aix.h"], [Portability Header])
		AC_DEFINE([OS_PORT_SRC], ["lslp-aix.c"], [Portability Source])
		AC_DEFINE([OS_PORT_OBJ], ["lslp-aix.o"], [Portability Object])
		OS_PORT_INCLUDE=lslp-aix.h
		OS_PORT_SRC=lslp-aix.c
		OS_PORT_OBJ=lslp-aix.o
		
		AC_DEFINE([OS_PORT_DIR], ["src/port/aix5"], [Portability Directory])
		AC_DEFINE([OS_PORT_PASEDIR], ["/QOpenSys/QIBM/ProdData/SC1"], [iSeries PASE Install Directory])
		OS_PORT_DIR="src/port/aix5"
		OS_PORT_PASEDIR="/QOpenSys/QIBM/ProdData/SC1"

		AC_PROG_CC()
		LINKER=$CC
		EXE=
		LIBS=
		LIB_INCLUDE_FLAG=-l
		THREAD_LIBS="-lpthread -lc -L/usr/lib/threads"
		REENTRANT_FLAG="-D_REENTRANT -D_THREAD_SAFE"
		NO_LINK_FLAG=-c
		LIB_SUFFIX=.o
		DLL_SUFFIX=.so
		EXE_OUT=-o
		OBJ_OUT=-o
		INSTALL_EXE=install
		INSTALL_FLAGS=--mode=install
		
		LIBTOOL=/usr/bin/libtool
		LIBTOOL_MODE_COMPILE=--mode=compile
		LIBTOOL_MODE_LINK=--mode=link
		LIBTOOL_MODE_CLEAN=--mode=clean
		LIBTOOL_MODE_INSTALL=--mode=install


# 	check see if the production variable is set 
	if [[ -z "$PRODUCTION" ]] ; then 	
	
		echo "Configuring Compiler for a debug build..."	

		# Thread safe:  cc_r options: -D_THREAD_SAFE -qthreaded -lpthread -lc -L/usr/lib/threads
		CPPFLAGS=
		CFLAGS="-g -qMAXMEM=8192 -qldbl128 -qalign=natural -qlonglong -qthreaded -qcpluscmt -w"
		CXXFLAGS=$CFLAGS	
	
		echo "Configuring Linker for a debug build..."	
		LDFLAGS=-g
	
	else 	
		echo "Configuring Compiler for a production build..."
	
		CPPFLAGS=
		CFLAGS="-g -qMAXMEM=8192 -qldbl128 -qalign=natural -qlonglong -qthreaded -qcpluscmt -w"
		CXXFLAGS=$CFLAGS	
	
		echo "Configuring Linker for a production build..."		
		LDFLAGS=-s		
	fi 	
		
		AC_PROG_MAKE_SET

		# Checks for libraries.
#		AC_CHECK_LIB([dl], [dlopen])

		# i5/OS PASE symlinks /QOpenSys/usr/lib to /usr/lib
		# i5/OS PASE symlinks SLP executables in /QOpenSys/QIBM/ProdData/SC1 to /QOpenSys/usr/bin
		# 	/QOpenSys/usr/bin:	slp_query, slp_srvreg, slp_attrreq, lslp_kernel, slp.conf
		#
		# Build process required files:
		# 	/QOpenSys/usr/include:	as400_protos.h as400_types.h, os400msg.h
		# 	/QOpenSys/usr/lib:	as400_libc.exp

		prefix=/QOpenSys/usr
		#prefix=/usr/local
		exec_prefix=$prefix
		bindir=$prefix/bin
		sbindir=$prefix/sbin
		libexecdir=$sbindir
		libdir=$prefix/lib
  	        includedir=$prefix/include

	;;


	*-*-go32* | *-*-mingw32* | *-*-cygwin* | *-*-windows*)

	# force the user to have VCINSTALLDIR defined in the 
	# environment before running the configuration script
	# 
		if test -z "$VCINSTALLDIR" ; then 
			echo "Error - VCINSTALLDIR not defined in environment"
			echo "Must define VCINSTALLDIR to point to the Visual Studio root"
			exit 1
		else
			echo "MSFT Visual Studio located at $VCINSTALLDIR"

		fi 

		AC_DEFINE([HOST_OS], ["windows"], [Host OS])
		HOST_OS=windows
		AC_DEFINE([OS_PORT_INCLUDE], ["lslp-windows.h"], [Portability Header])
                AC_DEFINE([OS_PORT_SRC], ["lslp-windows.c"], [Portability Source])
		AC_DEFINE([OS_PORT_OBJ], ["lslp-windows.obj"], [Portability Object])
		OS_PORT_INCLUDE=lslp-windows.h
                OS_PORT_SRC=lslp-windows.c
                OS_PORT_OBJ=lslp-windows.obj

		AC_DEFINE([OS_PORT_DIR], ["src/port/windows"], [Portability Directory])
		OS_PORT_DIR="src/port/windows"


	# use the MSFT SYSTEMDRIVE environment variable to construct 
	# the default pathways. 
		prefix=$SYSTEMDRIVE
		exec_prefix=$prefix
		bindir=$prefix\\WINDOWS\\system32
		sbindir=$bindir
		libexecdir=$bindir
		libdir=$bindir	
		



		includedir="$VCINSTALLDIR\\Vc7\\include;$VCINSTALLDIR\\Vc7\\PlatformSDK\\include"	
		echo "MSFT cl INCLUDE variable set to: $includedir"	
	
		if [[ -z "$PRODUCTION" ]] ;  then 

			echo "Configuring Compiler for a debug build..." 
			CPPFLAGS=""

			CFLAGS="/nologo /c /Zi"	
			CXXFLAGS=$CFLAGS
			LDFLAGS="/nologo /DEBUG"
			LINKER_OPTIONS="/link /INCREMENTAL:NO /DEBUG /MAP /MAPINFO:EXPORTS /MAPINFO:LINES"
			AC_SUBST([LINKER_OPTIONS])
		else 

			echo "Configuring Compiler for a production build..." 
			CPPFLAGS=""

			CFLAGS="/nologo /c "	
			CXXFLAGS=$CFLAGS
			LDFLAGS="/nologo"
			LINKER_OPTIONS="/link /INCREMENTAL:NO"
			AC_SUBST([LINKER_OPTIONS])
		fi 


		INCLUDE="$includedir"
		AC_SUBST([INCLUDE])
		LIB="$VCINSTALLDIR\\Vc7\\lib;$VCINSTALLDIR\\Vc7\PlatformSDK\\lib;$libdir"
		echo "MSFT cl LIB variable set to: $LIB"		
		AC_SUBST([LIB])	
		
	;;
	*-*-openbsd*)
        AC_DEFINE([HOST_OS], ["openBSD"], [Target Operation System])
	  HOST_OS=openbsd			
	  AC_DEFINE([OS_PORT_INCLUDE], ["lslp-linux.h"], [Portability Header])
	  OS_PORT_INCLUDE=lslp-linux.h
	  OS_PORT_SRC=lslp-linux.c
	  OS_PORT_OBJ=lslp-linux.o
	  HOST_OS=openBSD
	  AC_PROG_CC()
	  LINKER=$CC
	  EXE=
	  LIBS=
	  LIB_INCLUDE_FLAG=-l
	  THREAD_LIBS=-lpthread
	  REENTRANT_FLAG=-D_REENTRANT
	  NO_LINK_FLAG=-c
	  LIB_SUFFIX=.o
	  DLL_SUFFIX=.so
	  EXE_OUT=-o
	  OBJ_OUT=-o
	  INSTALL_EXE=installMa
	  INSTALL_FLAGS=
	  LIBTOOL_MODE_COMPILE=
	  LIBTOOL_MODE_LINK=
	  LIBTOOL_MODE_CLEAN=
	  LIBTOOL_MODE_INSTALL=

	  
	  if [[ -z "$PRODUCTION" ]] ; then
	  CFLAGS="-g -Wall -Dlinux -DBSD -DDEBUG_ALLOC"
	  LDFLAGS=-g 
	  LFLAGS="-p -v -d "
	  YFLAGS="-v -d"	
	  
	  else	
	  CFLAGS="-O -Wall -Dlinux -DBSD"
	  LDFLAGS=-s 
	  LFLAGS="-f -w"
	  YFLAGS=-l
	  fi

	  prefix=/usr/local
	  exec_prefix=$prefix
	  bindir=$prefix/bin
	  sbindir=$prefix/sbin
	  libexecdir=$sbindir
	  libdir=$prefix/lib
	  includedir=$prefix/include

	;;	
esac	
	
	

#------------------ special windows|notwindows  initializations --------------------
# The following if/else/endif block is a convenient way to do initialization that 
# is different for Windows, but the same for the other platforms. 
#
# Basically this block uses home-grown checks for Windows, but uses canned Autoconf 
# macros for all the other platforms. 
#
# I could have equally put this block in one or more of the case expressions above
# but it is more compact to do it this way. 
#-----------------------------------------------------------------------------------
if [[ $HOST_OS == "windows" ]] ; then 	


	# Define the dll import and export directives to be active on MSFT platforms 
	AC_DEFINE([DLL_EXPORT], [__declspec(dllexport)], [DLL Export directive for MSFT])
	AC_DEFINE([DLL_IMPORT], [__declspec(dllimport)], [DLL Import directive for MSFT])
	echo "defined DLL_EXPORT: __declspec(dllexport)"
	echo "defined DLL_IMPORT: __declspec(dllimport)"

	AC_DEFINE([_WIN32_WINNT], [0x400], [Required for Windows Platform Headers])
	echo "_WIN32_WINNT Preprocessor variable set to WINNT 4.0 and Above"
	windows_save_ifs=$IFS
	IFS=';'

	for dir in $includedir ; do 

	# home-grown header checks for MSFT. Loops to check all defined include dirs 
		IFS=$windows_save_ifs	
		if [[ -e "$dir\\inttypes.h" ]] ; then 
			AC_DEFINE([HAVE_INTTYPES_H], [1], 
			[Define to 1 if you have the <inttypes.h> header file])
		fi 

		if [[ -e "$dir\\memory.h" ]] ; then 
			AC_DEFINE([HAVE_MEMORY_H], [1], 
			[Define to 1 if you have the <memory.h> header file])
		fi 

		if [[ -e "$dir\\stdint.h" ]] ; then 
			AC_DEFINE([HAVE_STDINT_H], [1], 
			[Define to 1 if you have the <stdint.h> header file])
		fi 

		if [[ -e "$dir\\stdlib.h" ]] ; then 
			AC_DEFINE([HAVE_STDLIB_H], [1], 
			[Define to 1 if you have the <stdlib.h> header file])
		fi 

		if [[ -e "$dir\\strings.h" ]] ; then 
			AC_DEFINE([HAVE_STRINGS_H], [1], 
			[Define to 1 if you have the <strings.h> header file])
		fi 

		if [[ -e "$dir\\string.h" ]] ; then 
			AC_DEFINE([HAVE_STRING_H], [1], 
			[Define to 1 if you have the <string.h> header file])
		fi 

		if [[ -e "$dir\\sys\\stat.h" ]] ; then 
			AC_DEFINE([HAVE_SYS_STAT_H], [1], 
			[Define to 1 if you have the <sys/stat.h> header file])
		fi 

		if [[ -e "$dir\\sys\\types.h" ]] ; then 
			AC_DEFINE([HAVE_SYS_TYPES_H], [1], 
			[Define to 1 if you have the <sys/types.h> header file])
		fi 

		if [[ -e "$dir\\unistd.h" ]] ; then 
			AC_DEFINE([HAVE_UNISTD_H], [1], 
			[Define to 1 if you have the <unistd.h> header file])
		fi

		if [[ -e "$dir\\WinVer.h" ]] ; then 
			AC_DEFINE([HAVE_WINVER_H], [1], 
			[Define to 1 if you have <WinVer.h> header file])
		fi
	 	IFS=';'
	done 

	IFS=$windows_save_ifs
	AC_DEFINE_UNQUOTED([WINSOCK_2], [1], [Using Winsock 2 interface])

else
	export PATH=$PATH:/sbin:/usr/sbin:/usr/local/sbin
	AC_DEFINE([DLL_EXPORT], [], [Dll Export directive for MSFT])
	AC_DEFINE([DLL_IMPORT], [], [Dll Import directive for MSFT])
	AC_CHECK_HEADERS([unistd.h])
	AC_PATH_PROG([LDCONFIG], [ldconfig])
	  AC_PATH_PROG([ARCHIVE], [ar])
	  AC_PATH_PROG([INSTALL], [install])
fi

	AC_PROG_LEX
	AC_PROG_YACC

#--------------------Definitions and Substitutions--------------------------
# definitions
	


# each AC_SUBST will take a shell variable defined above or elsewhere
# and put it into the makefile

AC_SUBST([config_time])
AC_SUBST([config_host])
AC_SUBST([USER])
AC_SUBST([canonical_config])
AC_SUBST([build])

AC_SUBST([HOST_PROCESSOR])
AC_SUBST([HOST_OS])
AC_SUBST([HOST_VENDOR])

AC_SUBST([CPPFLAGS])
AC_SUBST([CFLAGS])
AC_SUBST([CXXFLAGS])
AC_SUBST([LDFLAGS])
AC_SUBST([YACC])
AC_SUBST([YFLAGS])
AC_SUBST([LEX])
AC_SUBST([LFLAGS])

AC_SUBST([builddir])
AC_SUBST([abs_builddir])

AC_SUBST([top_builddir])
AC_SUBST([abs_top_builddir])
AC_SUBST([srcdir])
AC_SUBST([abs_srcdir])
AC_SUBST([top_srcdir])
AC_SUBST([abs_top_srcdir])
AC_SUBST([prefix ])
AC_SUBST([exec_prefix])
AC_SUBST([bindir])
AC_SUBST([sbindir])
AC_SUBST([libexecdir]) # programs to be run by other programs (not users)
AC_SUBST([libdir ]) # $(exec_prefix)/lib
AC_SUBST([includedir])

AC_SUBST([CVS_ROOT])
AC_SUBST([PORTAL])
AC_SUBST([PORTAL_MAILTO])
AC_SUBST([EXTRACT])
AC_SUBST([INSTALL_CMD])
AC_SUBST([UNINSTALL_CMD])

AC_SUBST([OS_PORT_INCLUDE])
AC_SUBST([OS_PORT_SRC])
AC_SUBST([OS_PORT_OBJ])
AC_SUBST([OS_PORT_DIR])
AC_SUBST([OS_PORT_PASEDIR])
AC_SUBST([LINKER])
AC_SUBST([LIB_INCLUDE_FLAG])
AC_SUBST([THREAD_LIBS])
AC_SUBST([REENTRANT_FLAG])
AC_SUBST([NO_LINK_FLAG])
AC_SUBST([LIB_SUFFIX])
AC_SUBST([DLL_SUFFIX])
AC_SUBST([EXE_OUT])
AC_SUBST([OBJ_OUT])
AC_SUBST([INSTALL_EXE])
AC_SUBST([INSTALL_FLAGS])
AC_SUBST([LIBTOOL])
AC_SUBST([LIBTOOL_MODE_COMPILE])
AC_SUBST([LIBTOOL_MODE_LINK])
AC_SUBST([LIBTOOL_MODE_CLEAN])
AC_SUBST([LIBTOOL_MODE_INSTALL])

AC_SUBST([DLL_EXPORT])
AC_SUBST([DLL_IMPORT])

AC_CONFIG_HEADER([config.h])

AC_CONFIG_FILES([Makefile
    src/Makefile
    src/port/Makefile
    src/port/aix5/Makefile
    src/kernel/Makefile
    src/cmd-utils/Makefile
    src/cmd-utils/slp_client/Makefile
    src/cmd-utils/slp_query/Makefile
    src/cmd-utils/slp_srvreg/Makefile
    src/cmd-utils/slp_attrreq/Makefile
    ])
AC_OUTPUT

