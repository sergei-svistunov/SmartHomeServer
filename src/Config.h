#ifndef CONFIG_H_
#define CONFIG_H_

#include "json.hh"

#include <stdint.h>

#include <string>
#include <vector>

using namespace std;

class Config {
public:
    struct X10Device {
        string caption;
        string type;
        char home;
        uint8_t id;
    };

    Config(string filename);
    virtual ~Config();

    const string GetX10ControllerTTY();
    const vector<X10Device> GetX10Devices();
    const uint16_t GetWebInterfacePort();
    const string GetTorrentSavePath();
private:
    JSON::Value _json;
};

#endif /* CONFIG_H_ */
