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

std::ostream& operator<<(std::ostream& os, Home home) {
    const char* letters = "MECKOGAINFDLPHBJ";

    os << letters[(uint8_t) home];

    return os;
}

std::ostream& operator<<(std::ostream& os, Device device) {
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

Controller::Controller(string TTY) {
    fd = open(TTY.c_str(), O_RDWR | O_NOCTTY | O_NONBLOCK);

    if (fd == -1) {
        throw runtime_error("Cannot open " + TTY);
    } else {
        fcntl(fd, F_SETFL, 0);
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

    tcsetattr(fd, TCSANOW, &portSettings);
    tcflush(fd, TCIOFLUSH); // Clear IO buffer

    /*	while (1) {
     uint8_t data;
     auto n = read(fd, &data, 1);

     LOG(INFO)<< n << " " << (unsigned int)data;

     if (n > 0 && data == 0x5a) {
     LOG(INFO)<< "Can read data";
     _RecieveData();
     break;
     }
     }*/
}

Controller::~Controller() {

}

bool Controller::SetAddr(Home home, Device device, uint8_t repeats) {
    uint8_t data[2] = { _GetHeader(repeats, HeaderType::ADDRESS, false), (uint8_t) ((uint8_t) home << 4
            | (uint8_t) device) };

    LOG(INFO)<< "Set address " << home << device;

    return _WriteWithConfirm(data, 2);
}

bool Controller::SendCommand(Home home, Command command, uint8_t repeats) {
    uint8_t data[2] = { _GetHeader(repeats, HeaderType::FUNCTION, false), (uint8_t) ((uint8_t) home << 4
            | (uint8_t) command) };

    LOG(INFO)<< "Send command " << home << " " << command;

    return _WriteWithConfirm(data, 2);
}

void Controller::SendOff(Home home, Device device) {
    SetAddr(home, device) && SendCommand(home, Command::OFF);
}

void Controller::SendOn(Home home, Device device) {
    SetAddr(home, device) && SendCommand(home, Command::ON);
}

bool Controller::_WriteWithConfirm(void* buffer, size_t length) {
    uint8_t dataCheckSum = 0;
    for (auto i = 0; i < length; ++i)
        dataCheckSum += ((uint8_t*) buffer)[i];

    for (auto i = 1; i <= 10; ++i) {
        auto n = write(fd, buffer, length);
        if (n != length) {
            LOG(INFO)<< "  Cannot write data (#" << i << ")";
            continue;
        }

        struct timeval timeout = { 10, 0 };
        fd_set rdfs;
        FD_SET(fd, &rdfs);
        auto res = select(fd + 1, &rdfs, NULL, NULL, &timeout);

        if (res < 0) {
            LOG(INFO)<< "  Select before checksum is failed";
            continue;
        } else if (res == 0) {
            LOG(INFO) << " Select before checksum has timeout";
            continue;
        }

        uint8_t checkSum;
        read(fd, &checkSum, 1);
        LOG(INFO)<< "  Checksum (#" << i << ") = " << static_cast<unsigned int>(checkSum);

        if (checkSum != dataCheckSum) {
            LOG(INFO)<< "  Invalid checksum (#" << i << "): expected " << static_cast<unsigned int>(dataCheckSum);
            continue;
        }

        uint8_t zero = 0;
        write(fd, &zero, 1);

        res = select(fd + 1, &rdfs, NULL, NULL, &timeout);

        if (res < 0) {
            LOG(INFO)<< "  Confirm select failed (#" << i << ")";
            continue;
        } else if (res == 0) {
            LOG(INFO) << "  Confirm select timeout (#" << i << ")";
            continue;
        }

        uint8_t confirm;
        read(fd, &confirm, 1);
        if (confirm != 0x55) {
            LOG(INFO)<< "  Invalid confirm (#" << i << ") (" << static_cast<unsigned int>(confirm) << ")";
            continue;
        }

        LOG(INFO)<< "  Data was writed (#" << i << ")";
        return true;
    }

    return false;
}

/* Call it on receiving 0x5a*/
void Controller::_RecieveData() {
    uint8_t confirm = 0xc3;
    write(fd, &confirm, 1);

    struct timeval timeout = { 10, 0 };
    fd_set rdfs;
    FD_SET(fd, &rdfs);
    auto res = select(fd + 1, &rdfs, NULL, NULL, &timeout);

    if (res < 0) {
        LOG(INFO)<< "Recieve data select failed";
        return;
    } else if (res == 0) {
        LOG(INFO) << "Recieve data select timeout";
        return;
    }

    unsigned short int buffer[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    auto n = read(fd, buffer, 10);

    res = select(fd + 1, &rdfs, NULL, NULL, &timeout);
    auto n1 = read(fd, buffer + 1, 10);

    vector<unsigned short int> tmp(buffer, buffer + sizeof(buffer) / sizeof(unsigned short int));
    LOG(INFO)<<"Recieved " << n << ": " << n1 << "| " << tmp;
}

}
/* namespace X10 */
