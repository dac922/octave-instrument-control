#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.67])
AC_INIT([octave instrument control package], [0.0.0+])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CXX
AC_LANG(C++)

# Define macros needed
#AC_DEFINE(__STDC_CONSTANT_MACROS, [], [workaround for C++ programs to use C99 macros])

AC_CHECK_PROG([HAVE_MKOCTFILE], [mkoctfile], [yes], [no])
if [test $HAVE_MKOCTFILE = "no"]; then
  AC_MSG_ERROR([mkoctfile required to install $PACKAGE_NAME])
fi

build_usbtmc=no
build_tcp=no
build_gpib=no
build_vxi11=no
build_visa=no

# Howto check if usbtmc is available?
build_usbtmc=yes


# build tcp, available for most platforms
AC_CHECK_HEADERS([sys/socket.h],[build_tcp=yes])

# Check for winsock2 and ws2_32
AC_CHECK_HEADERS([winsock2.h],
  [AC_SEARCH_LIBS([_head_libws2_32_a], [ws2_32],
    [AC_SUBST(TCPLIBS, ["-lws2_32"]) build_tcp=yes] , [] )] , [])


# Checks for GPIB
AC_CHECK_HEADERS([gpib/ib.h],
  [AC_SEARCH_LIBS([ibrd], [gpib], [build_gpib=yes], [])] , [] )

if test "x$ac_cv_search_ibrd" != "xnone required"; then
  AC_SUBST(GPIBLIBS, [$ac_cv_search_ibrd])
fi


# Checks for RPC/VXI11
AC_ARG_WITH([tirpcinclude],
  [AC_HELP_STRING([--with-tirpcinclude=DIR],
    [use TI-RPC headers in DIR])],
    [tirpc_header_dir=$withval],
    [tirpc_header_dir=/usr/include/tirpc])

AC_CHECK_PROG([HAVE_RPCGEN], [rpcgen], [yes], [no])
if test "x$HAVE_RPCGEN" = "xyes"; then
  AC_SEARCH_LIBS([clnt_create], [tirpc], [build_vxi11=yes])
  AC_CHECK_HEADERS([rpc/rpc.h],[],
    [AC_CHECK_HEADERS([${tirpc_header_dir}/netconfig.h],
      [AC_SUBST([RPCINCLUDE], ["-I${tirpc_header_dir}"])],[build_vxi11=no])])

  if test "x$ac_cv_search_clnt_create" != "xnone required"; then
    AC_SUBST(RPCLIBS, [$ac_cv_search_clnt_create])
  fi

fi


# Checks for openvisa
AC_CHECK_HEADERS([visa/visa.h],
  [AC_SEARCH_LIBS([viOpenDefaultRM], [openvisa], [build_vxi11=yes], [])] , [] )

if test "x$ac_cv_search_viOpenDefaultRM" != "xnone required"; then
  AC_SUBST(VISALIBS, [$ac_cv_search_viOpenDefaultRM])
fi


#

if test $build_usbtmc = yes; then
  AC_DEFINE([BUILD_USBTMC], [], [build USBTMC interface functions])
fi

if test $build_tcp = yes; then
  AC_DEFINE([BUILD_TCP], [], [build TCP interface functions])
fi

if test $build_gpib = yes; then
  AC_DEFINE([BUILD_GPIB], [], [build GPIB interface functions]) 
fi

if test $build_vxi11 = yes; then
  AC_DEFINE([BUILD_VXI11], [], [build VXI11 interface functions])
fi

if test $build_visa = yes; then
  AC_DEFINE([BUILD_VISA], [], [build VISA interface functions])
fi


# all done

AC_CONFIG_FILES([Makefile gpib/Makefile tcp/Makefile vxi11/Makefile usbtmc/Makefile])
AC_OUTPUT

AC_MSG_NOTICE([

$PACKAGE_NAME is now configured with

   BUILD TCP:         $build_tcp
   BUILD USBTMC:      $build_usbtmc
   BUILD GPIB:        $build_gpib
   BUILD VXI11:       $build_vxi11

   GPIB LIBS:         $GPIBLIBS
   VXI11 LIBS:        $RPCLIBS
   VXI11 INCLUDE:     $RPCINCLUDE
   VISA LIBS:         $VISALIBS

])
