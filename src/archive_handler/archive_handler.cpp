#include "../../include/archive_handler/archive_handler.h"
#include <algorithm>
#include <iostream>

std::string archive_handler(const std::string& buffer) {
    archive *a;
    archive_entry *entry;
    a = archive_read_new();
    int r;
    r = archive_read_support_filter_all(a);
    if (r != ARCHIVE_OK) {
        throw std::runtime_error("Error when using archive filter all");
    }

    r = archive_read_support_format_raw(a);
    if (r != ARCHIVE_OK) {
        throw std::runtime_error("Error when using archive format raw");
    }

    r = archive_read_support_format_all(a);
    if (r != ARCHIVE_OK) {
        throw std::runtime_error("Error when using archive support format");
    }

    r = archive_read_open_memory(a, buffer.c_str(), buffer.size()); // Note 1
    if (r != ARCHIVE_OK) {
        throw std::runtime_error("Error in read archive from memory");
    }

    size_t size;
    const void* buff;
    la_int64_t offset;



    while (true) {
        r = archive_read_next_header(a, &entry);
        if (r != ARCHIVE_OK) {
            throw std::runtime_error("Error when reading next header");
        }
        std::string file_name = archive_entry_pathname(entry);
        if (file_name == "data") {
            break;
        }

        if (file_name.substr(file_name.size() - 4, 4) != ".txt" ||
            archive_entry_size(entry) > 10000000) {
            std::cout << file_name << " : " << archive_entry_size(entry) << std::endl;
            continue;
        }
        break;
    }

    r = -1;

    std::string ss;

    while (r != ARCHIVE_EOF) {
        r = archive_read_data_block(a, &buff, &size, &offset);
        ss.append(std::string(static_cast<const char*>(buff), size));
        ss += " ";
    }
    archive_read_close(a);
    archive_read_free(a);
    return ss;
}


void archive_queue_handler(synch_queue<std::string>& raw_file_queue,
                            synch_queue<word_map>& map_queue) {
    while (true) {

        std::string raw_data = raw_file_queue.pop();

        if (raw_data.empty()) {

            if (raw_file_queue.empty()) {
                map_queue.set_mul(raw_file_queue.get_counter() - 1);
            }
            break;
        }

//        raw_file_queue.descrease_size(raw_data.size());

//        std::cout << raw_data.size() << std::endl;
//        std::cout << raw_file_queue.size() << std::endl;

        std::string text;
        try {
            text = archive_handler(raw_data);

        }
        catch (std::runtime_error& e){

            continue;

        }

        word_map wm;
        boost::locale::boundary::ssegment_index map(boost::locale::boundary::word,
                    text.begin(), text.end());

        map.rule(boost::locale::boundary::word_letters);
        for (const auto& x: map) {
            wm[boost::locale::fold_case(boost::locale::normalize(std::string(x)))]++;
        }

        if (wm.empty())
            continue;

        raw_file_queue.increase_value();

        map_queue.push(std::move(wm));



    }
}

