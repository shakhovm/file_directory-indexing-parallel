#ifndef MAP_MERGING_H
#define MAP_MERGING_H

//
// Created by shakhov on 4/5/20.
//

#include <unordered_map>
#include "../conc_queue/synch_queue.h"
#include <iostream>
typedef std::unordered_map<std::string, size_t> word_map;
typedef std::pair<word_map, word_map> pair_map;

void map_merging(word_map& m1, word_map& m2);
void infinite_merging(synch_queue<word_map> &map_queue);
void map_queue_handler(synch_queue<word_map>& map_queue, synch_queue<pair_map>& pair_queue);

#endif // MAP_MERGING_H
