#include <glog/logging.h>

#include "WebServer.h"
#include "X10/Controller.h"

using namespace std;

int main(int argc, char* argv[]) {
    google::InitGoogleLogging(argv[0]);

    X10::Controller X10Controller("/tmp/remser1");

    WebServer webServer(38080, X10Controller);
    webServer.start();

    webServer.join();

    return 0;
}
