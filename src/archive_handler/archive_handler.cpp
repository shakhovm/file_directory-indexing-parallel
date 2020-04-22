#include "../../include/archive_handler/archive_handler.h"
#include "../../include/word_counting/map_merging.h"
#include <iostream>

#define FILE_SIZE_BORDER 10000000

void archive_handler(const std::string& buffer, synch_queue<std::string>& string_queue ) {

    archive *a;
    archive_entry *entry;
    a = archive_read_new();
    int r;
    r = archive_read_support_filter_all(a);
    if (r != ARCHIVE_OK) {
//        throw std::runtime_error("Error when using archive filter all");
        return;
    }

    r = archive_read_support_format_raw(a);
    if (r != ARCHIVE_OK) {
//        throw std::runtime_error("Error when using archive format raw");
        return;
    }

    r = archive_read_support_format_all(a);
    if (r != ARCHIVE_OK) {
//        throw std::runtime_error("Error when using archive support format");
        return;
    }

    r = archive_read_open_memory(a, buffer.c_str(), buffer.size());
    if (r != ARCHIVE_OK) {
//        throw std::runtime_error("Error in read archive from memory");
        return;
    }

    size_t size;
    const void* buff;
    la_int64_t offset;

    while (true) {
        r = archive_read_next_header(a, &entry);
        if (r == ARCHIVE_EOF)
            break;
        if (r != ARCHIVE_OK) {
//            throw std::runtime_error("Error when reading next header");
            return;
        }
        std::string file_name = archive_entry_pathname(entry);
//        std::cout << file_name << std::endl;
        if (file_name == "data") {
            string_queue.push_back(buffer);
            break;
        }

        if (file_name.substr(file_name.size() - 4, 4) != ".txt" ||
                archive_entry_size(entry) >= FILE_SIZE_BORDER){
            continue;
        }
        r = -1;

        std::string ss;

        while (r != ARCHIVE_EOF) {
            r = archive_read_data_block(a, &buff, &size, &offset);
            ss.append(std::string(static_cast<const char*>(buff), size));
            ss += " ";
        }
        if (ss.empty()) continue;
        string_queue.push_back(std::move(ss));

    }
//    archive_read_close(a);
//    archive_read_free(a);
}


void archive_queue_handler(synch_queue<std::string>& raw_file_queue,
                            synch_queue<word_map>& map_queue) {
    while (true) {
        std::string text = raw_file_queue.pop();

        if (text.empty()) {
            break;
        }

//        std::string text;
//        try {
//            text = archive_handler(raw_data);
//        }
//        catch (std::runtime_error& e){
//            continue;
//        }

        word_map wm;
        boost::locale::boundary::ssegment_index map(boost::locale::boundary::word,
                    text.begin(), text.end());

        map.rule(boost::locale::boundary::word_letters);
        for (const auto& x: map) {
            wm[boost::locale::fold_case(boost::locale::normalize(std::string(x)))]++;
        }

        if (wm.empty())
            continue;
        map_queue.push_front(std::move(wm));
    }
}

