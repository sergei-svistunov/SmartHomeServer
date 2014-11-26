AUTOMAKE_OPTIONS=subdir-objects

bin_PROGRAMS=smarthome.server
smarthome_server_SOURCES=smarthome.cpp \
    WebServer.cpp WebServer.h \
    X10/Controller.cpp X10/Controller.h

smarthome_server_CPPFLAGS=-I../simple_ws_server $(BOOST_CPPFLAGS) ${GLOG_CFLAGS} ${OPENSSL_CFLAGS}
smarthome_server_LDADD=$(BOOST_LDFLAGS) $(BOOST_SYSTEM_LIB) $(BOOST_ASIO_LIB) $(BOOST_THREAD_LIB) \
    ${GLOG_LIBS} ${OPENSSL_LIBS}