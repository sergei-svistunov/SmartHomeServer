#ifndef X10_CONTROLLER_H_
#define X10_CONTROLLER_H_

//#include "BaseDevice.h"

#include <stdint.h>

#include <string>
#include <list>
#include <map>
#include <vector>
#include <thread>
#include <mutex>

using namespace std;

namespace X10 {

enum class HomeID
    : uint8_t {
        M, E, C, K, O, G, A, I, N, F, D, L, P, H, B, J
};

enum class DeviceID
    : uint8_t {
        D13, D5, D3, D11, D15, D7, D1, D9, D14, D6, D4, D12, D16, D8, D2, D10
};

enum class Command {
    ALL_UNITS_OFF,
    ALL_LIGHTS_ON,
    ON,
    OFF,
    DIM,
    BRIGHT,
    ALL_LIGHTS_OFF,
    EXTENDED,
    HAIL_REQUEST,
    HAIL_ACKNOWLEDGE,
    EXT3,
    EXT4,
    EXT2,
    STATUS_ON,
    STATUS_OFF,
    STATUS_REQUEST
};

enum class HeaderType {
    ADDRESS, FUNCTION, UNKNOWN
};

struct Address {
    HomeID homeId;
    DeviceID deviceId;
};

struct ClassAddressCmp {
    bool operator()(const Address& l, const Address& r) const {
        return l.homeId == r.homeId ? l.deviceId < r.deviceId : l.homeId < r.homeId;
    }
};

class Controller;

class BaseDevice {
public:
    BaseDevice(Controller& controller, Address address, string name);
    virtual ~BaseDevice() {}
    const string& GetName() const {return _name;}

private:
    Controller& _controller;
    Address _address;
    string _name;
};

typedef map<Address, BaseDevice*, ClassAddressCmp> AddressDeviceMap;
typedef vector<const BaseDevice*> DevicesVector;

class Controller {
    friend class BaseDevice;
public:
    Controller(string TTY);
    virtual ~Controller();

    const DevicesVector GetDevices();

    void SendOff(HomeID home, DeviceID device);
    void SendOn(HomeID home, DeviceID device);
    void SendStatusRequest(HomeID home, DeviceID device);

protected:
    bool SetAddr(HomeID home, DeviceID device, uint8_t repeats = 2);
    bool SendCommand(HomeID home, Command command, uint8_t repeats = 2);

private:
    uint8_t _GetHeader(uint8_t repeats, HeaderType type, bool extended) {
        return (repeats << 3) | (1 << 2) | (type == HeaderType::FUNCTION ? 1 << 1 : 0) | (extended ? 1 : 0);
    }
    bool _WriteWithConfirm(void* buffer, size_t length);
    void _RecieveData();
    void _AddDevice(Address address, BaseDevice* device);

    int _fd;
    mutex _fdMutex;
    list<Address> _recievedAddresses;
    AddressDeviceMap _devices;
};

} /* namespace X10 */

#endif /* X10_CONTROLLER_H_ */
