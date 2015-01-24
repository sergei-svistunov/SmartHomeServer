AUTOMAKE_OPTIONS=subdir-objects

BUILT_SOURCES=../json/build/json.tab.hh

bin_PROGRAMS=smarthome.server

smarthome_server_SOURCES = smarthome.cpp \
 WebServer.cpp WebServer.h \
 VoiceControl.cpp VoiceControl.h \
 X10/Controller.cpp X10/Controller.h \
 X10/BaseDevice.h \
 X10/MDTx07.cpp X10\MDTx07.h \
 Torrent.cpp Torrent.h

smarthome_server_CPPFLAGS = -I../simple_ws_server -I../json -I../json/build \
 $(PTHREAD_CFLAGS) $(BOOST_CPPFLAGS) ${GLOG_CFLAGS} ${OPENSSL_CFLAGS} \
 ${GSTREAMER_CFLAGS} ${GSTREAMER_BASE_CFLAGS} ${GSTREAMER_APP_CFLAGS} $(TORRENT_CFLAGS) \
 ${SPOTTER_CPPFLAGS}
smarthome_server_LDADD = $(PTHREAD_CFLAGS) $(PTHREAD_LIBS) $(BOOST_LDFLAGS) \
 $(BOOST_SYSTEM_LIB) $(BOOST_ASIO_LIB) $(BOOST_FILESYSTEM_LIB) \
 ${GLOG_LIBS} ${OPENSSL_LIBS} ${GSTREAMER_LIBS} ${GSTREAMER_BASE_LIBS} ${GSTREAMER_APP_LIBS} $(TORRENT_LIBS) \
 ${SPOTTER_LDFLAGS} \
 ../json/build/libjson.a
 	
../json/build/json.tab.hh ../json/build/libjson.a:
	mkdir -p ../json/build
	cd ../json/build/ && cmake .. && make