#include "MDTx07.h"

namespace X10 {

MDTx07::MDTx07(Controller& controller, Address address, string name) : BaseDevice(controller, address, name) {

}

MDTx07::~MDTx07() {

}

JSON::Object MDTx07::GetInfo() const {
    auto result = BaseDevice::GetInfo();

    result["is_on"] = _is_on;
    result["volume"] = 10;

    return result;
}

} /* namespace X10 */
