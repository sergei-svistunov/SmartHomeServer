#include <glog/logging.h>

#include "WebServer.h"
#include "X10/Controller.h"
#include "X10/MDTx07.h"

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

using namespace std;
using namespace boost::filesystem;

int main(int argc, char* argv[]) {
    google::InitGoogleLogging(argv[0]);

    X10::Controller X10Controller("/tmp/remser1");
//    X10::MDTx07 bedRoomDimmer(X10Controller, { X10::HomeID::A, X10::DeviceID::D1 }, "Bedroom's light");
//    X10::MDTx07 hallDimmer(X10Controller, { X10::HomeID::A, X10::DeviceID::D2 }, "Hall's light");

    path binPath(initial_path<path>());
    binPath = system_complete(path(argv[0])).parent_path();

    WebServer webServer(38080, binPath.generic_string() + "/../html", X10Controller);
    webServer.start();
    webServer.join();

    return 0;
}
