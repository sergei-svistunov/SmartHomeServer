#ifndef TORRENT_H_
#define TORRENT_H_

#include <thread>

using namespace std;

class Torrent {
public:
    Torrent();
    virtual ~Torrent();

    bool start();
    void join();

private:
    thread* _serverThread = NULL;
};

#endif /* TORRENT_H_ */
