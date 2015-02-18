#include "Config.h"

Config::Config(string filename) {
    _json = parse_file(filename.c_str());
}

Config::~Config() {

}

const string Config::GetX10ControllerTTY() {
    return _json["X10"]["controller"]["tty"].as_string();
}

const vector<Config::X10Device> Config::GetX10Devices() {
    JSON::Array jsonDevices = _json["X10"]["devices"];
    vector<Config::X10Device> res;

    res.reserve(jsonDevices.size());
    for (auto& jsonDevice : jsonDevices) {
        res.push_back({
            jsonDevice["caption"].as_string(),
            jsonDevice["type"].as_string(),
            jsonDevice["home"].as_string()[0],
            (uint8_t) jsonDevice["id"].as_int()
        });
    }

    return res;
}

const uint16_t Config::GetWebInterfacePort() {
    return _json["webserver"]["port"].as_int();
}

const string Config::GetTorrentSavePath() {
    return _json["torrent"]["save_path"].as_string();
}
