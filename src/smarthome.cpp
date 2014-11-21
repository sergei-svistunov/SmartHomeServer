#include <iostream>
#include <memory>

#include <server_ws.hpp>
#include "X10/Controller.h"
#include <glog/logging.h>

using namespace std;
using namespace SimpleWeb;

int main(int argc, char* argv[]) {
	google::InitGoogleLogging(argv[0]);

	SocketServer<WS> server(38080, 4);

	LOG(INFO)<< "Start server";
	auto& devices = server.endpoint["^/devices/?$"];

	devices.onmessage =
			[&server](auto connection, auto message) {
				stringstream data_ss;
				message->data >> data_ss.rdbuf();

				LOG(INFO) << "Server: Message received: \"" << data_ss.str() << "\" from " << (size_t)connection.get();
			};

	devices.onopen =
			[](auto connection) {
				LOG(INFO) << "Server: Opened connection " << (size_t)connection.get();
			};

	devices.onclose =
			[](auto connection, int status, const string& reason) {
				LOG(INFO) << "Server: Closed connection " << (size_t)connection.get() << " with status code " << status;
			};

	devices.onerror =
			[](auto connection, const boost::system::error_code& ec) {
				LOG(INFO) << "Server: Error in connection " << (size_t)connection.get() << ". " <<
				"Error: " << ec << ", error message: " << ec.message();
			};

    thread server_thread([&server](){
        server.start();
    });

    while (true) {
    	auto connections = server.get_connections();
    	for (auto it = connections.cbegin(); it != connections.cend(); ++it) {
    	    stringstream data_ss;
    	    data_ss << "Hello";

    		server.send(*it, data_ss);
    	}
    	sleep(5);
    }

    server_thread.join();

//	X10::Controller X10Controller("/dev/ttyU0");
//
//	X10Controller.SendOn(X10::Home::A, X10::Device::D1);
//	sleep(5);
//	X10Controller.SendOff(X10::Home::A, X10::Device::D1);
	return 0;
}
