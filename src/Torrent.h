#ifndef TORRENT_H_
#define TORRENT_H_

#include <libtorrent/session.hpp>

#include "json.hh"

#include <thread>
#include <string>

using namespace std;

class Torrent {
public:
    Torrent();
    virtual ~Torrent();

    bool start();
    void join();

    bool AddFile(string filename);
    bool AddString(string& buffer);

    bool StartTorrent(string id);
    bool PauseTorrent(string id);
    bool DeleteTorrent(string id);

    JSON::Object GetInfo();

    function<void(JSON::Object info)> onUpdate = NULL;
private:
    libtorrent::session* _session;
    thread* _serverThread = NULL;
};

#endif /* TORRENT_H_ */
