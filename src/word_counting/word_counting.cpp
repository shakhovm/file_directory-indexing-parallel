#include "../../include/word_counting/word_counting.h"
#include <boost/locale.hpp>
#include <thread>
#include <boost/algorithm/string.hpp>
#include <boost/locale/boundary.hpp>

void sequential_indexing(synch_queue<std::string>& indexing_blocks,
                         synch_queue<word_map>& map_queue) {
    while (true) {
        std::string text = indexing_blocks.pop();
        if (text.empty()) {
            indexing_blocks.push("");
            break;
        }

        word_map wm;
        boost::locale::boundary::ssegment_index map(boost::locale::boundary::word,
                    text.begin(), text.end());
        map.rule(boost::locale::boundary::word_letters);
        for (const auto& x: map) {
            wm[boost::locale::fold_case(boost::locale::normalize(std::string(x)))]++;
        }
        indexing_blocks.increase_value();
        map_queue.push(std::move(wm));
    }

}
