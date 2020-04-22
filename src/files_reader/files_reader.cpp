#include "../../include/files_reader/files_reader.h"
#include <fstream>
#include "../../include/archive_handler/archive_handler.h"

void directory_iter(const std::string& directory_path,
                    synch_queue<std::string>& file_queue) {
    auto directory = std::filesystem::recursive_directory_iterator(directory_path);
    for (const auto& entry: directory) {
        std::string file_name_entry = entry.path().string();

        if (std::filesystem::is_directory(file_name_entry)) {
            continue;
        }

//        size_t file_size = std::filesystem::file_size(file_name_entry);

        file_queue.push_back(file_name_entry);
    }
}

void file_reader(synch_queue<std::string>& file_queue,
                 synch_queue<std::string>& raw_file_queue,
                 size_t thread_number) {

    while (true) {
        std::string file_name = file_queue.pop();
        if (file_name.empty()) {
            for (size_t i = 0; i < thread_number; ++i) {
                raw_file_queue.push_back("");
            }
            break;
        }
        std::ifstream raw_file(file_name, std::ios::binary);
        std::string raw_data = dynamic_cast<std::ostringstream&>(
                std::ostringstream{} << raw_file.rdbuf()).str();

        if (raw_data.empty()) {
            continue;
        }

        archive_handler(raw_data, raw_file_queue);


//        raw_file_queue.push_back(std::move(raw_data));
    }
}