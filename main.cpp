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
    auto directory = std::filesystem::recursive_directory_iterator(directory_path);
    for (const auto& entry: directory) {
        std::string file_name_entry = entry.path().string();

        if (std::filesystem::is_directory(file_name_entry)) {
            std::cout << file_name_entry << std::endl;
//            directory_iter(file_name_entry, file_queue);
            continue;
        }

        size_t file_size = std::filesystem::file_size(file_name_entry);
        if (file_size > 10000000) {
            std::cout << file_name_entry << std::endl;
//            continue;
        }

        file_queue.push(file_name_entry);
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
//        std::cout << file_name << std::endl;
        std::ifstream raw_file(file_name, std::ios::binary);
        std::string raw_data = dynamic_cast<std::ostringstream&>(
                        std::ostringstream{} << raw_file.rdbuf()).str();
        if (raw_data.empty()) {
            continue;
        }
//        raw_file_queue.increase_size(raw_data.size());
//        std::cout << raw_file_queue.byte_size << std::endl;
        raw_file_queue.push(std::move(raw_data));
//        std::cout << raw_file_queue.size() << std::endl;

    }

}

int main()
{

    boost::locale::generator gen;
    std::locale loc = gen("en_US.UTF-8");
    std::locale::global(loc);

    size_t thread_number = 4;
    size_t map_threads = 3;

    std::string file_data;

    std::string path = "/home/shakhov/guten";
//    std::string path = "../ETEXT02";
//    std::string path = "../files";
    std::vector<std::thread> thread_vector;
    std::vector<std::thread> map_thread_vector;

    // Queue creator
    synch_queue<std::string> file_queue(40);
    synch_queue<std::string> raw_file_queue(10);
    synch_queue<word_map> map_queue(20);
    synch_queue<pair_map> pair_map_queue(20);
    // -----------------------------
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
        std::chrono::duration_cast<std::chrono::seconds>(end - start).count() << std::endl;
    std::vector<std::pair<std::string, size_t>> v(wm.begin(), wm.end());
    std::sort(v.begin(), v.end(),
              [](const std::pair<std::string, size_t>& a,
                 const std::pair<std::string, size_t>& b){return a.second > b.second; });
    std::ofstream file_out("../result.txt");

    for (const auto& x: v) {
        file_out << x.first << " : " << x.second << std::endl;
    }
    std::ofstream file_out_by_a("../result_by_a.txt");
    std::sort(v.begin(), v.end(),
              [](const std::pair<std::string, size_t>& a,
                 const std::pair<std::string, size_t>& b){return a.first < b.first; });
    for (const auto& x: v) {
        file_out_by_a << x.first << " : " << x.second << std::endl;
    }


    return 0;
}

// a : 258427
