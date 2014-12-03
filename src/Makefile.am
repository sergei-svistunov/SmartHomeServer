AUTOMAKE_OPTIONS=subdir-objects

bin_PROGRAMS=smarthome.server

smarthome_server_SOURCES = smarthome.cpp \
 WebServer.cpp WebServer.h \
 X10/Controller.cpp X10/Controller.h

smarthome_server_CPPFLAGS = -I../simple_ws_server -I../json -I../json/build \
 $(PTHREAD_CFLAGS) $(BOOST_CPPFLAGS) ${GLOG_CFLAGS} ${OPENSSL_CFLAGS}
smarthome_server_LDADD = $(PTHREAD_CFLAGS) $(PTHREAD_LIBS) $(BOOST_LDFLAGS) \
 $(BOOST_SYSTEM_LIB) $(BOOST_ASIO_LIB) \
 ${GLOG_LIBS} ${OPENSSL_LIBS} \
 ../json/build/libjson.a