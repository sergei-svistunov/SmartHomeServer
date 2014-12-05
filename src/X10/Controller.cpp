#include "Controller.h"

#include <stdio.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <time.h>

#include <stdexcept>
#include <iostream>
#include <vector>
#include <bitset>

#include <glog/logging.h>
#include <glog/stl_logging.h>

using namespace std;

namespace X10 {

std::ostream& operator<<(std::ostream& os, HomeID home) {
    const char* letters = "MECKOGAINFDLPHBJ";

    os << letters[(uint8_t) home];

    return os;
}

std::ostream& operator<<(std::ostream& os, DeviceID device) {
    unsigned int ids[16] = { 13, 5, 3, 11, 15, 7, 1, 9, 14, 6, 4, 12, 16, 8, 2, 10 };

    os << ids[(uint8_t) device];

    return os;
}

std::ostream& operator<<(std::ostream& os, Command command) {
    const char* commands[16] = { "ALL_UNITS_OFF", "ALL_LIGHTS_ON", "ON", "OFF", "DIM", "BRIGHT", "ALL_LIGHTS_OFF",
            "EXTENDED", "HAIL_REQUEST", "HAIL_ACKNOWLEDGE", "EXT3", "EXT4", "EXT2", "STATUS_ON", "STATUS_OFF",
            "STATUS_REQUEST" };

    os << commands[(uint8_t) command];

    return os;
}

std::ostream& operator<<(std::ostream& os, Address address) {
    os << address.homeId << address.deviceId;

    return os;
}

std::ostream& operator<<(std::ostream& os, uint8_t buffer[10]) {
    for (auto i = 0; i < 10; ++i)
        os << static_cast<unsigned int>(buffer[i]) << ' ';

    return os;
}

BaseDevice::BaseDevice(Controller& controller, Address address, string name) :
        _controller(controller), _address(address), _name(name) {
    _controller._AddDevice(address, this);
}

Controller::Controller(string TTY) {
    _fd = open(TTY.c_str(), O_RDWR | O_NOCTTY | O_NONBLOCK);

    if (_fd == -1) {
        throw runtime_error("Cannot open " + TTY);
    } else {
        fcntl(_fd, F_SETFL, 0);
    }

    struct termios portSettings;

    cfsetospeed(&portSettings, B4800);
    cfsetispeed(&portSettings, B4800);

    portSettings.c_cflag = (portSettings.c_cflag & ~CSIZE) | CS8 | B4800; // 8 databits
    portSettings.c_cflag |= (CLOCAL | CREAD);
    portSettings.c_cflag &= ~(PARENB | PARODD); // No parity
    portSettings.c_cflag &= ~CRTSCTS; // No hardware handshake
    portSettings.c_cflag &= ~CSTOPB; // 1 stopbit
    portSettings.c_iflag = IGNBRK;
    portSettings.c_iflag &= ~(IXON | IXOFF | IXANY); // No software handshake
    portSettings.c_lflag = 0;
    portSettings.c_oflag = 0;
    portSettings.c_cc[VTIME] = 1;
    portSettings.c_cc[VMIN] = 60;

    cfmakeraw(&portSettings);

    tcsetattr(_fd, TCSANOW, &portSettings);
    tcflush(_fd, TCIOFLUSH); // Clear IO buffer

    thread checkRecieveThread([this]() {
        const timespec sleepTime = {0, 500000000}; // Half second
            timespec remainingTime;

            while (true) {
                _RecieveData();
                nanosleep(&sleepTime, &remainingTime);
            }
        });
    checkRecieveThread.detach();
}

Controller::~Controller() {

}

const DevicesVector Controller::GetDevices() {
    DevicesVector result;
    result.reserve(_devices.size());

    for (auto it=_devices.cbegin(); it != _devices.cend(); ++it)
        result.push_back(it->second);

    return result;
}

//map<Address, BaseDevice*>::const_iterator Controller::GetDevicesIterator() {
//    return _devices.cbegin();
//}

void Controller::SendOff(HomeID home, DeviceID device) {
    _fdMutex.lock();
    SetAddr(home, device) && SendCommand(home, Command::OFF);
    _fdMutex.unlock();
}

void Controller::SendOn(HomeID home, DeviceID device) {
    _fdMutex.lock();
    SetAddr(home, device) && SendCommand(home, Command::ON);
    _fdMutex.unlock();
}

void Controller::SendStatusRequest(HomeID home, DeviceID device) {
    _fdMutex.lock();
    SetAddr(home, device) && SendCommand(home, Command::STATUS_REQUEST);
    _fdMutex.unlock();
}

bool Controller::SetAddr(HomeID home, DeviceID device, uint8_t repeats) {
    uint8_t data[2] = { _GetHeader(repeats, HeaderType::ADDRESS, false), (uint8_t) ((uint8_t) home << 4
            | (uint8_t) device) };

    LOG(INFO)<< "Set address " << home << device;

    return _WriteWithConfirm(data, 2);
}

bool Controller::SendCommand(HomeID home, Command command, uint8_t repeats) {
    uint8_t data[2] = { _GetHeader(repeats, HeaderType::FUNCTION, false), (uint8_t) ((uint8_t) home << 4
            | (uint8_t) command) };

    LOG(INFO)<< "Send command " << home << " " << command;

    return _WriteWithConfirm(data, 2);
}

bool Controller::_WriteWithConfirm(void* buffer, size_t length) {
    uint8_t dataCheckSum = 0;
    for (auto i = 0; i < length; ++i)
        dataCheckSum += ((uint8_t*) buffer)[i];

    for (auto i = 1; i <= 3; ++i) {
        auto n = write(_fd, buffer, length);
        if (n != length) {
            LOG(INFO)<< "  Cannot write data (#" << i << ")";
            continue;
        }

        uint8_t checkSum;
        read(_fd, &checkSum, 1);
        LOG(INFO)<< "  Checksum (#" << i << ") = " << static_cast<unsigned int>(checkSum);

        if (checkSum != dataCheckSum) {
            LOG(INFO)<< "  Invalid checksum (#" << i << "): expected " << static_cast<unsigned int>(dataCheckSum);
            continue;
        }

        uint8_t zero = 0;
        write(_fd, &zero, 1);

        uint8_t confirm;
        read(_fd, &confirm, 1);
        if (confirm != 0x55) {
            LOG(INFO)<< "  Invalid confirm (#" << i << ") (" << static_cast<unsigned int>(confirm) << ")";
            continue;
        }

        LOG(INFO)<< "  Data was writed (#" << i << ")";
        return true;
    }

    return false;
}

void Controller::_RecieveData() {
    _fdMutex.lock();

    // Is controller has data?
    struct timeval timeout = { 0, 0 };
    fd_set rdfs;
    FD_SET(_fd, &rdfs);
    auto res = select(_fd + 1, &rdfs, NULL, NULL, &timeout);

    if (res < 1) {
        _fdMutex.unlock();
        return;
    }

    uint8_t flagByte;
    auto n0 = read(_fd, &flagByte, 1);
    if (n0 != 1 || flagByte != 0x5a) {
        _fdMutex.unlock();
        return;
    }

    LOG(INFO)<< "X10 Controller has data";

    uint8_t confirm = 0xc3;
    write(_fd, &confirm, 1);

    uint8_t bytesCount;
    auto n = read(_fd, &bytesCount, 1);

    uint8_t buffer[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

    size_t n1 = 0;
    while (n1 < bytesCount)
        n1 += read(_fd, buffer + n1, bytesCount - n1);

    LOG(INFO)<<"  Recieved data " << static_cast<unsigned int>(bytesCount) << " | " << buffer;

    bitset<8> flags(buffer[0]);
    LOG(INFO)<< "    " << flags;
    for (auto i = 0; i < bytesCount - 1; ++i) {
        if (flags[i]) {
            LOG(INFO)<< "      " << (Command)(buffer[i+1] & 0x0f) << ": " << _recievedAddresses;
            _recievedAddresses.clear();
        } else {
            _recievedAddresses.push_back( {(HomeID)((buffer[i+1] >> 4) & 0x0f), (DeviceID)(buffer[i+1] & 0x0f)});
        }
    }

    _fdMutex.unlock();
}

void Controller::_AddDevice(Address address, BaseDevice* device) {
    _devices[address] = device;
}

}
/* namespace X10 */
