dnl Process this file with autoconf to produce a configure script.

AC_PREREQ(2.69)
AC_INIT(smarthome, 1.0)

AC_CONFIG_MACRO_DIR([m4])

AC_CANONICAL_SYSTEM
AM_INIT_AUTOMAKE()

CXXFLAGS="$CXXFLAGS -std=c++1y -Wno-deprecated"
AC_PROG_CXX

AX_PTHREAD(, [AC_MSG_ERROR([smarthome needs pthread, but it was not found in your system])])

AX_BOOST_BASE([1.55],,[AC_MSG_ERROR([smarthome needs Boost, but it was not found in your system])])
AX_BOOST_SYSTEM
AX_BOOST_ASIO
AX_BOOST_THREAD

PKG_CHECK_MODULES([GLOG], [libglog >= 0.3.3])
PKG_CHECK_MODULES([OPENSSL], [openssl >= 1.0.0])

AC_CONFIG_FILES(Makefile src/Makefile)
AC_OUTPUT

