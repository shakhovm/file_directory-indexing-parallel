//
// Created by shakhov on 4/22/20.
//

#ifndef INDEXING_FILES_READER_H
#define INDEXING_FILES_READER_H

#include <filesystem>
#include "../conc_queue/synch_queue.h"

void directory_iter(const std::string& directory_path,
                    synch_queue<std::string>& file_queue);


void file_reader(synch_queue<std::string>& file_queue,
                 synch_queue<std::string>& raw_file_queue,
                 size_t thread_number);

#endif //INDEXING_FILES_READER_H
