#include <glog/logging.h>

#include "WebServer.h"
#include "X10/Controller.h"
#include "X10/MDTx07.h"

using namespace std;

int main(int argc, char* argv[]) {
    google::InitGoogleLogging(argv[0]);

    X10::Controller X10Controller("/tmp/remser1");
    X10::MDTx07 bedRoomDimmer(X10Controller, {X10::HomeID::A, X10::DeviceID::D1}, "Bedroom's light");
    X10::MDTx07 hallDimmer(X10Controller, {X10::HomeID::A, X10::DeviceID::D2}, "Hall's light");

    WebServer webServer(38080, X10Controller);
    webServer.start();

    webServer.join();

    return 0;
}
