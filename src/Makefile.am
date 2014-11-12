AUTOMAKE_OPTIONS=subdir-objects

bin_PROGRAMS=smarthome.server
smarthome_server_SOURCES=smarthome.cpp X10/Controller.cpp X10/Controller.h

smarthome_server_CPPFLAGS=${GLOG_CFLAGS}
smarthome_server_LDADD=${GLOG_LIBS}