#ifndef X10_CONTROLLER_H_
#define X10_CONTROLLER_H_

#include <stdint.h>

#include <string>
#include <list>
#include <thread>
#include <mutex>

using namespace std;

namespace X10 {

enum class Home
    : uint8_t {
        M, E, C, K, O, G, A, I, N, F, D, L, P, H, B, J
};

enum class Device
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

class Controller {
public:
    Controller(string TTY);
    virtual ~Controller();

    void SendOff(Home home, Device device);
    void SendOn(Home home, Device device);
    void SendStatusRequest(Home home, Device device);

protected:
    bool SetAddr(Home home, Device device, uint8_t repeats = 2);
    bool SendCommand(Home home, Command command, uint8_t repeats = 2);

private:
    uint8_t _GetHeader(uint8_t repeats, HeaderType type, bool extended) {
        return (repeats << 3) | (1 << 2) | (type == HeaderType::FUNCTION ? 1 << 1 : 0) | (extended ? 1 : 0);
    }
    bool _WriteWithConfirm(void* buffer, size_t length);
    void _RecieveData();

    int _fd;
    mutex _fdMutex;
    list<pair<Home, Device>> _recievedAddresses;
};

} /* namespace X10 */

#endif /* X10_CONTROLLER_H_ */
