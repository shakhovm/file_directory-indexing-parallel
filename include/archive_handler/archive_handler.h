#ifndef ARCHIVE_HANDLER_H
#define ARCHIVE_HANDLER_H
#include <sstream>
#include <archive.h>
#include <archive_entry.h>
#include <fstream>
#include "../conc_queue/synch_queue.h"

#include "../../include/word_counting/map_merging.h"
#include <boost/locale.hpp>
#include <thread>
#include <boost/algorithm/string.hpp>
#include <boost/locale/boundary.hpp>

void archive_handler(const std::string& buffer,
                     synch_queue<std::string>& string_queue);

void archive_queue_handler(synch_queue<std::string>& file_queue,
                            synch_queue<word_map>& map_queue);

#endif // ARCHIVE_HANDLER_H
