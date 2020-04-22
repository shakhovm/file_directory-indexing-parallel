#include "../../include/word_counting/map_merging.h"
void map_merging(word_map& m1, word_map& m2) {
    for (const auto& x: m2) {
        m1[x.first] += x.second;
    }
}

void infinite_merging(synch_queue<word_map> &map_queue) {
    while (true) {

        pair_map pm = map_queue.pop2();
        if (pm.second.empty()) {
            map_queue.push_front(std::move(pm.first));
            map_queue.push_back(pm.second);

            break;
        }
        map_merging(pm.first, pm.second);
        map_queue.push_front(std::move(pm.first));
    }
}

void map_queue_handler(synch_queue<word_map>& map_queue, synch_queue<pair_map>& pair_queue) {
    while (true) {

        word_map wm1 = map_queue.pop();

        if (map_queue.get_counter() == map_queue.get_mul())
        {
            pair_queue.emplace(word_map(), word_map());
            map_queue.push_back(std::move(wm1));
            break;
        }
        word_map wm2 = map_queue.pop();

        pair_queue.emplace(std::move(wm1), std::move(wm2));
    }
}
