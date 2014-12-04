#ifndef X10_BASEDEVICE_H_
#define X10_BASEDEVICE_H_

#include "Controller.h"
#include <string>

namespace X10 {

using namespace std;

class BaseDevice {
public:
    BaseDevice() {};
    BaseDevice(Controller& controller, Address address, string& name) : _address(address), _name(name) {};
    virtual ~BaseDevice() {};

private:
    Controller& _controller;
    Address _address;
    string _name;
};

} /* namespace X10 */

#endif /* X10_BASEDEVICE_H_ */
