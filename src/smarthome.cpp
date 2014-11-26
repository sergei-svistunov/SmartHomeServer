#include <glog/logging.h>

#include "WebServer.h"
#include "X10/Controller.h"

using namespace std;

int main(int argc, char* argv[]) {
    google::InitGoogleLogging(argv[0]);

    X10::Controller X10Controller("/tmp/remser1");

    WebServer webServer(38080, X10Controller);
    webServer.start();
//    X10Controller.SendOn(X10::Home::A, X10::Device::D1);
//    sleep(5);
//    X10Controller.SendOff(X10::Home::A, X10::Device::D1);

    webServer.join();

    return 0;
}
