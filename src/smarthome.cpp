#include <iostream>

#include "X10/Controller.h"
#include <glog/logging.h>

using namespace std;

int main(int argc, char* argv[]) {
	google::InitGoogleLogging(argv[0]);

	LOG(INFO) << "Start server";
	X10::Controller X10Controller("/dev/ttyU0");

	X10Controller.SendOn(X10::Home::A, X10::Device::D1);
	sleep(5);
	X10Controller.SendOff(X10::Home::A, X10::Device::D1);

	return 0;
}
