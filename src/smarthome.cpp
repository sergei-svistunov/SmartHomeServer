#include <glog/logging.h>

#include "WebServer.h"
#include "VoiceControl.h"
#include "X10/Controller.h"
#include "X10/MDTx07.h"
#include "Torrent.h"

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

#include <gst/gst.h>

using namespace std;
using namespace boost::filesystem;

int main(int argc, char* argv[]) {
    google::InitGoogleLogging(argv[0]);
    gst_init(&argc, &argv);

    X10::Controller X10Controller("/dev/ttyUSB0");
    X10::MDTx07 childRoomDimmer(X10Controller, { X10::HomeID::A, X10::DeviceID::D1 }, "Childroom's light");
    X10::MDTx07 hallDimmer(X10Controller, { X10::HomeID::A, X10::DeviceID::D2 }, "Hall's light");
    X10::MDTx07 kitchenDimmer(X10Controller, { X10::HomeID::A, X10::DeviceID::D3 }, "Kitchen's light");
    X10::MDTx07 bedRoomDimmer(X10Controller, { X10::HomeID::A, X10::DeviceID::D4 }, "Bedroom's light");

    Torrent torrent;
    torrent.start();

    path binPath(initial_path<path>());
    binPath = system_complete(path(argv[0])).parent_path();

    WebServer webServer(38080, binPath.generic_string() + "/../html", X10Controller);
    webServer.start();

    VoiceControl voiceControl;

    torrent.join();
    webServer.join();

    return 0;
}
