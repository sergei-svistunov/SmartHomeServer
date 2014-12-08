AUTOMAKE_OPTIONS=subdir-objects

BUILT_SOURCES=../json/build/libjson.a ../json/build/json.tab.hh

bin_PROGRAMS=smarthome.server

smarthome_server_SOURCES = smarthome.cpp \
 ../json/build/json.tab.hh \
 WebServer.cpp WebServer.h \
 X10/Controller.cpp X10/Controller.h \
 X10/BaseDevice.h \
 X10/MDTx07.cpp X10\MDTx07.h

smarthome_server_CPPFLAGS = -I../simple_ws_server -I../json -I../json/build \
 $(PTHREAD_CFLAGS) $(BOOST_CPPFLAGS) ${GLOG_CFLAGS} ${OPENSSL_CFLAGS}
smarthome_server_LDADD = $(PTHREAD_CFLAGS) $(PTHREAD_LIBS) $(BOOST_LDFLAGS) \
 $(BOOST_SYSTEM_LIB) $(BOOST_ASIO_LIB) $(BOOST_FILESYSTEM_LIB) \
 ${GLOG_LIBS} ${OPENSSL_LIBS} \
 ../json/build/libjson.a
 
../json/build:
	mkdir  ../json/build
 	
../json/build/json.tab.hh: ../json/build/libjson.a

../json/build/libjson.a: ../json/build
	cd ../json/build/ && cmake .. && make