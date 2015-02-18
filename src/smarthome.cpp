#include <glog/logging.h>

#include "Config.h"
#include "WebServer.h"
#include "VoiceControl.h"
#include "X10/Controller.h"
#include "X10/MDTx07.h"
#include "Torrent.h"

#include <list>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/asio/impl/src.hpp>

#include <gst/gst.h>

using namespace std;
using namespace boost::filesystem;

int main(int argc, char* argv[]) {
    google::InitGoogleLogging(argv[0]);
    gst_init(&argc, &argv);

    path binPath(initial_path<path>());
    binPath = system_complete(path(argv[0])).parent_path();

    Config config(binPath.generic_string() + "/config.json");

    X10::Controller X10Controller(config.GetX10ControllerTTY());
    list<X10::BaseDevice*> X10Devices;
    for (auto& device: config.GetX10Devices()) {
        X10::HomeID home;
        if (!charToX10Home(device.home, home))
            throw runtime_error("Invalid home " + device.home);

        X10::DeviceID id;
        if (!intToX10Device(device.id, id))
            throw runtime_error("Invalid id " + device.id);

        if (device.type == "MDTx07")
            X10Devices.push_back(new X10::MDTx07(X10Controller, {home, id}, device.caption));
        else
            throw runtime_error("Invalid type " + device.type);
    }

    Torrent torrent(config.GetTorrentSavePath());
    WebServer webServer(config.GetWebInterfacePort(), binPath.generic_string() + "/../html", X10Controller, torrent);

    webServer.start();
    torrent.start();

    VoiceControl voiceControl;

    torrent.join();
    webServer.join();

    for (auto& device: X10Devices)
        delete device;

    return 0;
}
