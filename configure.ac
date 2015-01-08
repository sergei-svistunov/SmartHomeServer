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
AX_BOOST_FILESYSTEM
AX_BOOST_ASIO

PKG_CHECK_MODULES([GLOG], [libglog >= 0.3.3])
PKG_CHECK_MODULES([OPENSSL], [openssl >= 1.0.0])
PKG_CHECK_MODULES([GSTREAMER], [gstreamer-1.0 >= 1.2.0])
PKG_CHECK_MODULES([GSTREAMER_BASE], [gstreamer-base-1.0 >= 1.2.0])
PKG_CHECK_MODULES([GSTREAMER_APP], [gstreamer-app-1.0 >= 1.2.0])
PKG_CHECK_MODULES([TORRENT], [libtorrent >= 0.13.2])
PKG_CHECK_MODULES([SIGCPP], [sigc++-2.0 >= 2.2.10])

AC_ARG_WITH(
    [spotter-dir],
    AS_HELP_STRING(
        [--with-spotter-dir=LIB_DIR],
        [Directory with spotter lib and header files]),
        [
            if test -d "$withval"
            then
                ac_spotter_dir="$withval"
            else
                AC_MSG_ERROR(--with-spotter-dir expected directory name)
            fi
        ],
        [ac_spotter_dir=""]
)

AC_SUBST(SPOTTER_CPPFLAGS)
AC_CHECK_HEADER([$ac_spotter_dir/libspotter/spotter.h], SPOTTER_CPPFLAGS="-I$ac_spotter_dir -DSPOTTER_CONF=\\\"$ac_spotter_dir/gertruda\\\"", AC_MSG_ERROR("spotter.h not found"))
AC_SUBST(SPOTTER_LDFLAGS)
AC_CHECK_FILE([$ac_spotter_dir/libspotter/libspotter.a], SPOTTER_LDFLAGS="$ac_spotter_dir/libspotter/libspotter.a $ac_spotter_dir/libtask/libtask.a $ac_spotter_dir/webrtc_vad/webrtc_vad.a", AC_MSG_ERROR("libspotter.a not found"))

AC_CONFIG_FILES(Makefile src/Makefile)
AC_OUTPUT

