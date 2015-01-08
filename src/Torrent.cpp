#include "Torrent.h"

#define lt_cacheline_aligned __attribute__((__aligned__(128)))

#include <torrent/utils/thread_base.h>
#include <torrent/torrent.h>
#include <torrent/poll_epoll.h>
#include <torrent/data/file_manager.h>
#include <torrent/download/resource_manager.h>
#include <torrent/object.h>
#include <torrent/object_stream.h>
#include <torrent/http.h>

#include <unistd.h>
#include <functional>
#include <fstream>
#include <stdexcept>

torrent::Poll* create_poll() {
    return torrent::PollEPoll::create(sysconf(_SC_OPEN_MAX));
}

Torrent::Torrent() {
    torrent::Poll::slot_create_poll() = bind(create_poll);
    torrent::initialize();

//    torrent::FileManager * fm = torrent::file_manager();
//    torrent::DList downloadList;
//    torrent::download_list(downloadList);
}

Torrent::~Torrent() {
    if (_serverThread != NULL)
        delete _serverThread;

    torrent::cleanup();
}

bool Torrent::start() {
    _serverThread = new thread([this]() {
        torrent::thread_base::event_loop(torrent::main_thread());
    });

    fstream stream("/home/svistunov/Загрузки/[rutracker.org].t3745024.torrent", ios::in | ios::binary);

    if (!stream.is_open())
      throw runtime_error("Cannot open torrent");

    torrent::Object *torrentObj = new torrent::Object();
    stream >> *torrentObj;

    torrent::Download d = torrent::download_add(torrentObj);

    return true;
}

void Torrent::join() {
    _serverThread->join();
}

