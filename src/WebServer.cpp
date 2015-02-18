#include "WebServer.h"
#include "json.hh"

#include <glog/logging.h>

bool charToX10Home(char home, X10::HomeID& result) {
    switch (home) {
    case 'A':
        result = X10::HomeID::A;
        break;
    case 'B':
        result = X10::HomeID::B;
        break;
    case 'C':
        result = X10::HomeID::C;
        break;
    case 'D':
        result = X10::HomeID::D;
        break;
    case 'E':
        result = X10::HomeID::E;
        break;
    case 'F':
        result = X10::HomeID::F;
        break;
    case 'G':
        result = X10::HomeID::G;
        break;
    case 'H':
        result = X10::HomeID::H;
        break;
    case 'I':
        result = X10::HomeID::I;
        break;
    case 'J':
        result = X10::HomeID::J;
        break;
    case 'K':
        result = X10::HomeID::K;
        break;
    case 'L':
        result = X10::HomeID::L;
        break;
    case 'M':
        result = X10::HomeID::M;
        break;
    case 'N':
        result = X10::HomeID::N;
        break;
    case 'O':
        result = X10::HomeID::O;
        break;
    case 'P':
        result = X10::HomeID::P;
        break;
    default:
        return false;
    }

    return true;
}

bool intToX10Device(uint8_t device, X10::DeviceID& result) {
    switch (device) {
    case 1:
        result = X10::DeviceID::D1;
        break;
    case 2:
        result = X10::DeviceID::D2;
        break;
    case 3:
        result = X10::DeviceID::D3;
        break;
    case 4:
        result = X10::DeviceID::D4;
        break;
    case 5:
        result = X10::DeviceID::D5;
        break;
    case 6:
        result = X10::DeviceID::D6;
        break;
    case 7:
        result = X10::DeviceID::D7;
        break;
    case 8:
        result = X10::DeviceID::D8;
        break;
    case 9:
        result = X10::DeviceID::D9;
        break;
    case 10:
        result = X10::DeviceID::D10;
        break;
    case 11:
        result = X10::DeviceID::D11;
        break;
    case 12:
        result = X10::DeviceID::D12;
        break;
    case 13:
        result = X10::DeviceID::D13;
        break;
    case 14:
        result = X10::DeviceID::D14;
        break;
    case 15:
        result = X10::DeviceID::D15;
        break;
    case 16:
        result = X10::DeviceID::D16;
        break;
    default:
        return false;
    }

    return true;
}

WebServer::WebServer(uint16_t port, string documentRoot, X10::Controller& X10_Controller, Torrent& torrent) :
        _X10_Controller(X10_Controller), _torrent(torrent) {

    _server = new SocketServer<WS>(port, 4, 5, 0, documentRoot);

    LOG(INFO)<< "Start server";
    _endPointDevices();
    _endPointTorrents();

}

WebServer::~WebServer() {
    if (_serverThread != NULL) {
        _server->stop();
        delete _serverThread;
    }
    delete _server;
}

bool WebServer::start() {
    _serverThread = new thread([this]() {
        _server->start();
    });
    return true;
}

void WebServer::join() {
    _serverThread->join();
}

void WebServer::_endPointDevices() {
    string epReStr = "^/devices/?$";
    auto& devices = _server->endpoint[epReStr];

    devices.onmessage =
            [this](auto connection, auto message) {
                string strData(istreambuf_iterator<char>(message->data), istreambuf_iterator<char>());

                LOG(INFO) << "Server: Message received: \"" << strData << "\" from " << (size_t)connection.get();

                JSON::Value data;
                try {
                    data = parse_string(strData);
                } catch (runtime_error& e) {
                    LOG(INFO) << e.what();
                    return;
                }

                if (data.type() != JSON::OBJECT || data["device"].type() != JSON::STRING || data["device"].as_string().length() < 2
                        || data["command"].type() != JSON::STRING) {
                    LOG(INFO) << "  Invalid JSON";
                    return;
                }

                auto deviceAddr = data["device"].as_string();
                char homeId = deviceAddr.c_str()[0];
                uint8_t deviceId = atoi(deviceAddr.c_str() + 1);

                X10::HomeID X10_Home;
                if (!charToX10Home(homeId, X10_Home)) {
                    LOG(INFO) << "Invalid home id: " << homeId;
                    return;
                }

                X10::DeviceID X10_Device;
                if (!intToX10Device(deviceId, X10_Device)) {
                    LOG(INFO) << "Invalid device id: " << deviceId;
                    return;
                }

                auto command = data["command"].as_string();
                for (auto& c : command)
                c = toupper(c);

                if (command == "ON")
                _X10_Controller.SendOn(X10_Home, X10_Device);
                else if (command == "OFF")
                _X10_Controller.SendOff(X10_Home, X10_Device);
                else if (command == "DIM")
                _X10_Controller.SendDim(X10_Home, X10_Device, data["volume"].as_int());
                else if (command == "PRESET_DIM")
                _X10_Controller.SendPresetDim(X10_Home, X10_Device, data["volume"].as_int());
                else {
                    LOG(INFO) << "Invalid command: " << command;
                }
            };

    devices.onopen = [this](auto connection) {
        JSON::Array devicesJSON;

        auto X10_Devices = _X10_Controller.GetDevices();
        for (auto it=X10_Devices.cbegin(); it!=X10_Devices.cend(); ++it)
        devicesJSON.push_back((*it)->GetInfo());

        JSON::Object messageJSON;
        messageJSON["type"] = "devicesList";
        messageJSON["devices"] = devicesJSON;

        LOG(INFO) << "Server: Opened connection " << (size_t)connection.get();
        stringstream data_ss;
        data_ss << messageJSON;
        _server->send(connection, data_ss);
    };

    devices.onclose = [](auto connection, int status, const string& reason) {
        LOG(INFO) << "Server: Closed connection " << (size_t)connection.get() << " with status code " << status;
    };

    devices.onerror = [](auto connection, const boost::system::error_code& ec) {
        LOG(INFO) << "Server: Error in connection " << (size_t)connection.get() << ". " <<
        "Error: " << ec << ", error message: " << ec.message();
    };

    regex re(epReStr);
    _X10_Controller.onUpdate = [this, re](X10::Address address, X10::Command command, vector<uint8_t>& data) {
        JSON::Object messageJSON;
        messageJSON["type"] = "updateDevice";
        messageJSON["device"] = _X10_Controller.GetDeviceByAddr(address)->GetInfo();
        for (auto& connection: _server->get_connections()) {
            if (!regex_match(connection.get()->path, re))
                continue;

            stringstream data_ss;
            data_ss << messageJSON;
            _server->send(connection, data_ss);
        }
    };
}

void WebServer::_endPointTorrents() {
    string epReStr = "^/torrents/?$";
    auto& torrents = _server->endpoint[epReStr];

    torrents.onmessage = [this](auto connection, auto message) {
        string strData(istreambuf_iterator<char>(message->data), istreambuf_iterator<char>());

        if (strData[0] != '{') {
            LOG(INFO) << "Server: File received from " << (size_t)connection.get();
            _torrent.AddString(strData);
            return;
        }

        LOG(INFO) << "Server: Message received: \"" << strData << "\" from " << (size_t)connection.get();

        JSON::Value data;
        try {
            data = parse_string(strData);
        } catch (runtime_error& e) {
            LOG(INFO) << e.what();
            return;
        }

        if (data.type() != JSON::OBJECT || data["command"].type() != JSON::STRING) {
            LOG(INFO) << "  Invalid JSON";
            return;
        }

        auto command = data["command"].as_string();
        for (auto& c : command)
        c = toupper(c);

        if (command == "START_TORRENT") {
            if (data["id"].type() == JSON::STRING)
                _torrent.StartTorrent(data["id"].as_string());
        } else if (command == "PAUSE_TORRENT") {
            if (data["id"].type() == JSON::STRING)
                _torrent.PauseTorrent(data["id"].as_string());
        } else if (command == "DELETE_TORRENT") {
            if (data["id"].type() == JSON::STRING)
                _torrent.DeleteTorrent(data["id"].as_string());
        } else {
            LOG(INFO) << "Invalid command: " << command;
        }
    };

    torrents.onopen = [this](auto connection) {
        LOG(INFO) << "Server: Opened connection " << (size_t)connection.get();
        stringstream data_ss;
        data_ss << _torrent.GetInfo();
        _server->send(connection, data_ss);
    };

    torrents.onclose = [](auto connection, int status, const string& reason) {
        LOG(INFO) << "Server: Closed connection " << (size_t)connection.get() << " with status code " << status;
    };

    torrents.onerror = [](auto connection, const boost::system::error_code& ec) {
        LOG(INFO) << "Server: Error in connection " << (size_t)connection.get() << ". " <<
        "Error: " << ec << ", error message: " << ec.message();
    };

    regex re(epReStr);
    _torrent.onUpdate = [this, re](JSON::Object infoJSON) {
        for (auto& connection: _server->get_connections()) {
            if (!regex_match(connection.get()->path, re))
                continue;

            stringstream data_ss;
            data_ss << infoJSON;
            _server->send(connection, data_ss);
        }
    };

}
