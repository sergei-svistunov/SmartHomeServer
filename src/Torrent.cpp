#include <Torrent.h>

#include <glog/logging.h>
#include <libtorrent/session.hpp>
#include <libtorrent/alert_types.hpp>
#include <libtorrent/torrent_handle.hpp>
#include <libtorrent/peer_info.hpp>
/*
#include <libtorrent/upnp.hpp>
#include <libtorrent/natpmp.hpp>
#include <libtorrent/extensions/smart_ban.hpp>
#include <libtorrent/extensions/ut_metadata.hpp>
#include <libtorrent/extensions/ut_pex.hpp>
*/

#include <stdexcept>
#include <vector>
#include <sstream>

Torrent::Torrent() {
    libtorrent::error_code ec;

    _session = new libtorrent::session(libtorrent::fingerprint("HS", 1, 0, 0, 0), make_pair(16881, 16889), "0.0.0.0", 0,
            libtorrent::alert::tracker_notification | libtorrent::alert::error_notification);

//    _session->set_alert_mask(libtorrent::alert::all_categories & ~libtorrent::alert::stats_notification);
//    _session->set_alert_mask(libtorrent::alert::error_notification);

    auto settings = _session->settings();
    settings.user_agent = "HomeServer/1.0.0 libtorrent/0.16.13";
    settings.num_want = 10000;
    settings.announce_to_all_trackers = true;
    settings.announce_to_all_tiers = true;
    _session->set_settings(settings);

//    auto upnp = _session->start_upnp();
//    upnp->add_mapping(libtorrent::upnp::tcp, 6881, 6889);

//    _session->listen_on(make_pair(16881, 16889), ec, "0.0.0.0", libtorrent::session::listen_no_system_port);
//    if (ec)
//        throw runtime_error("failed to open listen socket: " + ec.message());

//    addFile("/home/svistunov/Загрузки/ubuntu-14.04.1-desktop-amd64.iso.torrent");
//    addFile("/home/svistunov/Загрузки/2014-12-24-wheezy-raspbian.zip.torrent");

//    _session->start_dht();
//    _session->add_dht_router(make_pair(string("router.bittorrent.com"), 6881));
//    _session->add_dht_router(make_pair(string("router.utorrent.com"), 6881));
//    _session->add_dht_router(make_pair(string("dht.transmissionbt.com"), 6881));
//    _session->add_dht_router(make_pair(string("dht.aelitis.com"), 6881));
//
//    _session->start_lsd();
//    auto natpmp = _session->start_natpmp();
//    natpmp->add_mapping(libtorrent::natpmp::tcp, 6881, 6889);

    /*
     _session->add_extension(&libtorrent::create_smart_ban_plugin);
     _session->add_extension(&libtorrent::create_ut_metadata_plugin);
     _session->add_extension(&libtorrent::create_ut_pex_plugin);
     */
}

Torrent::~Torrent() {
    cerr << "destroyed" << endl;
}

bool Torrent::start() {
    _serverThread = new thread([this]() {
        while (true)
            if (_session->wait_for_alert(libtorrent::time_duration(1000000))) { // 1 sec
                auto alert = _session->pop_alert();
                LOG(INFO) << alert.get()->what() << "\t" << alert.get()->message();
            } else {
                if (onUpdate)
                    onUpdate(GetInfo());
            }
        });

    return true;
}

void Torrent::join() {
    _serverThread->join();
}

bool Torrent::AddFile(string filename) {
    libtorrent::add_torrent_params addParams;
    memchr(&addParams, 0, sizeof(libtorrent::add_torrent_params));
    libtorrent::error_code ec;

    addParams.save_path = "/tmp/";
    addParams.auto_managed = true;
    addParams.paused = false;
    addParams.ti = new libtorrent::torrent_info(filename, ec);
    if (ec) {
        LOG(INFO)<< "Cannot load torrent: " << ec.message().c_str();
        return false;
    }

    _session->add_torrent(addParams, ec);
    if (ec) {
        LOG(INFO)<< "Cannot add torrent: " << ec.message().c_str();
        return false;
    }

    return true;
}

bool Torrent::AddString(string& buffer) {
    libtorrent::add_torrent_params addParams;
    memchr(&addParams, 0, sizeof(libtorrent::add_torrent_params));
    libtorrent::error_code ec;

    addParams.save_path = "/tmp/";
    addParams.auto_managed = false;
    addParams.paused = false;
    addParams.ti = new libtorrent::torrent_info(buffer.c_str(), buffer.length(), ec);
    if (ec) {
        LOG(INFO)<< "Cannot load torrent: " << ec.message().c_str();
        return false;
    }

    _session->add_torrent(addParams, ec);
    if (ec) {
        LOG(INFO)<< "Cannot add torrent: " << ec.message().c_str();
        return false;
    }

    return true;
}

bool Torrent::StartTorrent(string id) {
    stringstream id_ss;
    id_ss << id;
    libtorrent::sha1_hash hash;
    id_ss >> hash;
    const auto& th = _session->find_torrent(hash);

    if (!th.is_valid())
        return false;

    th.resume();

    return true;
}

bool Torrent::PauseTorrent(string id) {
    stringstream id_ss;
    id_ss << id;
    libtorrent::sha1_hash hash;
    id_ss >> hash;
    const auto& th = _session->find_torrent(hash);

    if (!th.is_valid())
        return false;

    th.pause(libtorrent::torrent_handle::graceful_pause);

    return true;
}

bool Torrent::DeleteTorrent(string id) {
    stringstream id_ss;
    id_ss << id;
    libtorrent::sha1_hash hash;
    id_ss >> hash;
    const auto& th = _session->find_torrent(hash);

    if (!th.is_valid())
        return false;

    _session->remove_torrent(th);

    return true;
}

JSON::Object Torrent::GetInfo() {
    JSON::Object info;

    JSON::Array torrentsJSON;
    for (auto& torrent : _session->get_torrents()) {
        JSON::Object torrentJSON;
        torrentJSON["name"] = torrent.name();

        stringstream id_ss;
        id_ss << torrent.info_hash();
        string id_s;
        id_ss >> id_s;
        torrentJSON["id"] = id_s;

        auto status = torrent.status();
        torrentJSON["progress"] = int(status.progress * 100.0f + 0.5f);
        torrentJSON["all_time_download"] = status.all_time_download;
        torrentJSON["all_time_upload"] = status.all_time_upload;
        torrentJSON["download_rate"] = status.download_rate;
        torrentJSON["upload_rate"] = status.upload_rate;
        torrentJSON["error"] = status.error;
        torrentJSON["is_finished"] = status.is_finished;
        torrentJSON["paused"] = status.paused;
        torrentJSON["queue_position"] = status.queue_position;

        switch (status.state) {
        case libtorrent::torrent_status::queued_for_checking:
            torrentJSON["state"] = "Queued for checking";
            break;
        case libtorrent::torrent_status::checking_files:
            torrentJSON["state"] = "Checking files";
            break;
        case libtorrent::torrent_status::downloading_metadata:
            torrentJSON["state"] = "Downloading metadata";
            break;
        case libtorrent::torrent_status::downloading:
            torrentJSON["state"] = "Downloading";
            break;
        case libtorrent::torrent_status::finished:
            torrentJSON["state"] = "Finished";
            break;
        case libtorrent::torrent_status::seeding:
            torrentJSON["state"] = "Seeding";
            break;
        case libtorrent::torrent_status::allocating:
            torrentJSON["state"] = "Allocating";
            break;
        case libtorrent::torrent_status::checking_resume_data:
            torrentJSON["state"] = "Checking resume data";
            break;
        default:
            torrentJSON["state"] = "Unknown";
        };

        JSON::Array peersJSON;
        vector<libtorrent::peer_info> peersVector;
        torrent.get_peer_info(peersVector);
        for (auto& peer: peersVector) {
            JSON::Object peerJSON;
            peerJSON["client"] = peer.client;
            peerJSON["downloaded"] = peer.total_download;
            peerJSON["uploaded"] = peer.total_upload;
            peerJSON["down_speed"] = peer.down_speed;
            peerJSON["up_speed"] = peer.up_speed;
            peerJSON["progress"] = int(peer.progress * 100.0f + 0.5f);
            peerJSON["ip"] = peer.ip.address().to_string();

            peersJSON.push_back(peerJSON);
        }
        torrentJSON["peers"] = peersJSON;

        torrentsJSON.push_back(torrentJSON);
    }

    info["torrents"] = torrentsJSON;

    return info;
}
