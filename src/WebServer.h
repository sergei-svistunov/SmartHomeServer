#ifndef WEBSERVER_H_
#define WEBSERVER_H_

#include <server_ws.hpp>

#include "X10/Controller.h"

#include <stdint.h>

using namespace std;
using namespace SimpleWeb;

class WebServer {
public:
    WebServer(uint16_t port, string documentRoot, X10::Controller& X10_Controller);
    virtual ~WebServer();
    bool start();
    void join();
private:
    SocketServer<WS>* _server = NULL;
    thread* _serverThread = NULL;
    X10::Controller& _X10_Controller;
};

#endif /* WEBSERVER_H_ */
