dnl Process this file with autoconf to produce a configure script.

AC_PREREQ(2.69)
AC_INIT(smarthome, 1.0)

AC_CANONICAL_SYSTEM
AM_INIT_AUTOMAKE()

CXXFLAGS="-std=c++11"
LDFLAGS="-static-libstdc++ -pthread"
AC_PROG_CXX

PKG_CHECK_MODULES([GLOG], [libglog >= 0.3.3])

AC_CONFIG_FILES(Makefile src/Makefile)
AC_OUTPUT

