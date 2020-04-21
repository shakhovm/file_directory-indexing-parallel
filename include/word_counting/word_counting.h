#ifndef WORD_COUNTING_H
#define WORD_COUNTING_H

#include "../conc_queue/synch_queue.h"
#include "../word_counting/map_merging.h"
#include <vector>


typedef std::string::const_iterator str_itr;

void sequential_indexing(synch_queue<std::string>& indexing_blocks,
                         synch_queue<word_map>& map_queue);
#endif // WORD_COUNTING_H
