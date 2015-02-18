#ifndef WEBSERVER_H_
#define WEBSERVER_H_

#include <server_ws.hpp>

#include "X10/Controller.h"
#include "Torrent.h"

#include <stdint.h>

using namespace std;
using namespace SimpleWeb;

bool charToX10Home(char home, X10::HomeID& result);
bool intToX10Device(uint8_t device, X10::DeviceID& result);

class WebServer {
public:
    WebServer(uint16_t port, string documentRoot, X10::Controller& X10_Controller, Torrent& torrent);
    virtual ~WebServer();
    bool start();
    void join();
private:
    void _endPointDevices();
    void _endPointTorrents();

    SocketServer<WS>* _server = NULL;
    thread* _serverThread = NULL;
    X10::Controller& _X10_Controller;
    Torrent& _torrent;
};

#endif /* WEBSERVER_H_ */
