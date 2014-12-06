#include "MDTx07.h"

#include <glog/logging.h>
#include <glog/stl_logging.h>

namespace X10 {

MDTx07::MDTx07(Controller& controller, Address address, string name) :
        BaseDevice(controller, address, name) {
    controller.SendStatusRequest(address.homeId, address.deviceId);
}

MDTx07::~MDTx07() {

}

JSON::Object MDTx07::GetInfo() const {
    auto result = BaseDevice::GetInfo();

    result["is_on"] = _is_on;
    result["volume"] = 0;

    return result;
}

void MDTx07::Notify(Command command, vector<uint8_t>& data) {
    if (command == Command::ON || command == Command::STATUS_ON)
        _is_on = true;
    else if (command == Command::OFF || command == Command::STATUS_OFF)
        _is_on = false;
}

} /* namespace X10 */
