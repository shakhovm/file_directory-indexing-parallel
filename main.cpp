#include <iostream>
#include <filesystem>
#include "include/conc_queue/synch_queue.h"
#include "include/archive_handler/archive_handler.h"
#include "include/conf_handler/confhandler.h"
#include "include/word_counting/word_counting.h"
#include <boost/locale.hpp>

#include <vector>
#include <thread>
#include <fstream>
void directory_iter(const std::string& directory_path,
                    synch_queue<std::string>& file_queue) {
    auto directory = std::filesystem::directory_iterator(directory_path);
    for (const auto& entry: directory) {
        if (std::filesystem::is_directory(entry.path())) {
            directory_iter(entry.path(), file_queue);
        }

        file_queue.push(entry.path());
    }
}

void file_reader(synch_queue<std::string>& file_queue,
                 synch_queue<std::string>& raw_file_queue,
                 size_t thread_number) {

    while (true) {
        std::string file_name = file_queue.pop();
        if (file_name.empty()) {
            for (size_t i = 0; i < thread_number; ++i) {
                raw_file_queue.push("");
            }

            break;
        }

        std::ifstream raw_file(file_name, std::ios::binary);

        raw_file_queue.push(dynamic_cast<std::ostringstream&>(
                std::ostringstream{} << raw_file.rdbuf()).str());
    }

}

int main()
{

    boost::locale::generator gen;
    std::locale loc = gen("en_US.UTF-8");
    std::locale::global(loc);

    size_t thread_number = 4;
    size_t map_threads = 4;

    std::string file_data;

    std::string path = "../ETEXT02";

    std::vector<std::thread> thread_vector;
    std::vector<std::thread> map_thread_vector;

    synch_queue<std::string> file_queue;
    synch_queue<std::string> raw_file_queue;
    synch_queue<word_map> map_queue;
    synch_queue<pair_map> pair_map_queue;
    auto start = std::chrono::high_resolution_clock::now();
    std::thread directory_handler_thread(directory_iter, std::cref(path),
                std::ref(file_queue));

    std::thread file_reader_thread(file_reader, std::ref(file_queue),
                                   std::ref(raw_file_queue), thread_number);


    std::thread map_handler_thread(map_queue_handler,
                                   std::ref(map_queue), std::ref(pair_map_queue));

    for (size_t i = 0; i < thread_number; ++i ) {
        thread_vector.emplace_back(archive_queue_handler, std::ref(raw_file_queue),
                                   std::ref(map_queue));
    }

    for (size_t i = 0; i < map_threads; ++i) {
        map_thread_vector.emplace_back(infinite_merging, std::ref(map_queue),
                                       std::ref(pair_map_queue));
    }

    directory_handler_thread.join();

    file_queue.push("");
    file_reader_thread.join();

    for (auto &x : thread_vector) x.join();
    map_handler_thread.join();

    for (auto &x : map_thread_vector) x.join();
    word_map wm = map_queue.pop();
    auto end = std::chrono::high_resolution_clock::now();
    std::cout <<
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << std::endl;
    std::vector<std::pair<std::string, size_t>> v(wm.begin(), wm.end());
    std::sort(v.begin(), v.end(),
              [](const std::pair<std::string, size_t>& a,
                 const std::pair<std::string, size_t>& b){return a.second < b.second; });
    std::ofstream file_out("../result.txt");

    for (const auto& x: v) {
        file_out << x.first << " : " << x.second << std::endl;
    }


    return 0;
}

// a : 258427
