#include<stdio.h>
#include<libtorrent/create_torrent.hpp>
#include<libtorrent/file.hpp>
// #include<libtorrent/storage.hpp>
#include<libtorrent/file_storage.hpp>
#include<fstream>
#include<iostream>
#include <libtorrent/session.hpp>
#include <libtorrent/session_params.hpp>
#include <libtorrent/add_torrent_params.hpp>
#include <libtorrent/torrent_handle.hpp>
#include <libtorrent/alert_types.hpp>
#include <libtorrent/bencode.hpp>
#include <libtorrent/torrent_status.hpp>
#include <libtorrent/read_resume_data.hpp>
#include <libtorrent/write_resume_data.hpp>
#include <libtorrent/error_code.hpp>
#include <libtorrent/magnet_uri.hpp>
#include <libtorrent/bencode.hpp>
#include <libtorrent/torrent_info.hpp>
#include <time.h>

using namespace std;

string tracker_url = "http://192.168.242.166:8096/announce";
string config_file = ".config";

void build_torrent(string dir_path)
{
    libtorrent::file_storage fs;
    string dir_path_str = "./" + dir_path;
    // recursively adds files in directories
    libtorrent::add_files(fs, dir_path_str.c_str());

    libtorrent::create_torrent t(fs);
    t.add_tracker(tracker_url);
    t.set_creator("device1");

    // reads the files and calculates the hashes
    libtorrent::set_piece_hashes(t, ".");

    const string torrent_filename = "my_torrent.torrent";
    ofstream torrent_file("my_torrent.torrent", std::ios_base::binary);
    libtorrent::bencode(std::ostream_iterator<char>(torrent_file), t.generate());
    torrent_file.close();

    libtorrent::error_code ec;
    libtorrent::torrent_info torr_info(torrent_filename);
    string magnet_uri = libtorrent::make_magnet_uri(torr_info);
    cout << "Magnet uri:: " << magnet_uri << "\n";

    auto proxies = libtorrent::proxy_settings();
    // proxies.type = libtorrent::proxy_settings::pro("http");
    proxies.hostname = "test.xyz";
    proxies.port = 8096;

    libtorrent::session torrent_session;
    const pair<int, int> ports = {8080,8095};
    torrent_session.listen_on(ports, ec);
    libtorrent::add_torrent_params atp = libtorrent::parse_magnet_uri(magnet_uri);
    atp.save_path = ".";
    atp.flags |= lt::add_torrent_params::flag_seed_mode;
    torrent_session.set_proxy(proxies);
    torrent_session.set_web_seed_proxy(proxies);
    libtorrent::torrent_handle handler = torrent_session.add_torrent(atp);

    while(true)
    {
        if(handler.is_seed())
        {
            auto torrent_status = handler.status();
            cout << "...\n";
            // printf("%f complete (down: %.1f kb/s up: %.1f kB/s peers: %d) %s", torrent_status.progress * 100, torrent_status.download_rate / 1000, torrent_status.upload_rate / 1000, torrent_status.num_peers);
        }


    }
}

void update_config()
{
    const char* filepath = realpath(config_file.c_str(), NULL);
    if(filepath)
    {
        fstream config_stream(config_file);
        string line;
        bool ip_found = false;
        while(getline(config_stream, line))
        {
            if(line == "ips")
            {
                ip_found = true;
                cout << "Current Recipients: ";
                continue;
            }
            if(ip_found)
            {
                cout << line << "\n";
            }
        }
        config_stream.close();
        ofstream config_stream2(config_file, ios::app);
        cout << "\nEnter more Ip Addresses, enter 'done' to terminate\n";
        getline(cin, line);
        while(line != "done")
        {
            config_stream2 << line << "\n";
            getline(cin, line);
        }
        config_stream2.close();
    }
    else
    {
        ofstream config_stream(config_file);
        config_stream << "ips\n";
        config_stream.close();
    }
}

int main(int argc, char **argv)
{
    cout << argc << "\n";
    cout << "Data: " << argv[1] << "\n";
    if(argc < 2)
    {
        cout << "Supply data files to be transferred\n";
        return 0;
    }
    update_config();
    build_torrent(argv[1]);


    return 0;
}