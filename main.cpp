#include <iostream>
#include "include/conc_queue/synch_queue.h"
#include <boost/locale.hpp>
#include "include/conf_handler/confhandler.h"
#include "include/word_counting/map_merging.h"
#include <vector>
#include <thread>
#include "include/files_reader/files_reader.h"
#include "include/archive_handler/archive_handler.h"

int main(int argc, char* argv[])
{
    std::string conf_file_name;
    if (argc > 1) {
        conf_file_name = argv[1];
    } else {
        conf_file_name = "conf.dat";
    }

    ConfHandler conf_handler;
    try {
        conf_handler = ConfHandler(conf_file_name);
    } catch (std::runtime_error& e ) {
        std::cout << "Cannot Handler Configuration file!" << std::endl;
        return 1;
    }
    auto params = conf_handler.getConfParams();
    boost::locale::generator gen;
    std::locale loc = gen("en_US.UTF-8");
    std::locale::global(loc);


    std::string file_data;
    std::vector<std::thread> thread_vector;
    std::vector<std::thread> map_thread_vector;

    // Queue creator
    synch_queue<std::string> file_queue(30);
    synch_queue<std::string> raw_file_queue(10);
    synch_queue<word_map> map_queue(10);
    // -----------------------------

    auto start = std::chrono::high_resolution_clock::now();
    std::thread directory_handler_thread(directory_iter, std::cref(params.input_directory),
                std::ref(file_queue));

    std::thread file_reader_thread(file_reader, std::ref(file_queue),
                                   std::ref(raw_file_queue), params.indexing_threads);

    for (size_t i = 0; i < params.indexing_threads; ++i ) {
        thread_vector.emplace_back(archive_queue_handler, std::ref(raw_file_queue),
                                   std::ref(map_queue));
    }

    for (size_t i = 0; i < params.merging_threads; ++i) {
        map_thread_vector.emplace_back(infinite_merging, std::ref(map_queue));
    }

    directory_handler_thread.join();

    file_queue.push_back("");
    file_reader_thread.join();

    for (auto &x : thread_vector) x.join();
    map_queue.push_back(word_map());

    for (auto &x : map_thread_vector) x.join();

    word_map wm = map_queue.pop();
    auto end = std::chrono::high_resolution_clock::now();
    std::cout <<
        std::chrono::duration_cast<std::chrono::seconds>(end - start).count() << std::endl;
    std::vector<std::pair<std::string, size_t>> v(wm.begin(), wm.end());
    std::sort(v.begin(), v.end(),
              [](const std::pair<std::string, size_t>& a,
                 const std::pair<std::string, size_t>& b){return a.second > b.second; });
    std::ofstream file_out(params.out_by_n);

    for (const auto& x: v) {
        file_out << x.first << " : " << x.second << std::endl;
    }
    std::ofstream file_out_by_a(params.out_by_a);
    std::sort(v.begin(), v.end(),
              [](const std::pair<std::string, size_t>& a,
                 const std::pair<std::string, size_t>& b){return a.first < b.first; });
    for (const auto& x: v) {
        file_out_by_a << x.first << " : " << x.second << std::endl;
    }


    return 0;
}

