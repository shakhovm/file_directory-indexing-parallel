//
// Created by shakhov on 4/4/20.
//

#include "../../include/conf_handler/confhandler.h"
#include <sstream>
#include <regex>
ConfHandler::ConfHandler(const std::string& filename) {
    std::ifstream conf_file(filename);
    if (!conf_file.is_open()) {
        throw std::runtime_error("File " + filename +  " is not found!");
    }

    content = dynamic_cast<std::ostringstream&>(
            std::ostringstream{} << conf_file.rdbuf()).str();
    conf_file_handler();
}

std::string ConfHandler::file_pattern(const std::string& pattern) {
    std::smatch match;
    std::regex r(pattern);

    regex_search(content, match, r);
    if (match.empty()) {
        throw std::runtime_error("Cannot handle conf file!");
    }
    std::string new_string = match.str();;
    return new_string.substr(new_string.find_first_of('=') + 1);
}

void ConfHandler::conf_file_handler() {

    conf_params.infile = file_pattern(R"(infile=.+\..+)");
    conf_params.out_by_a = file_pattern(R"(out_by_a=.+\..+)");
    conf_params.out_by_n = file_pattern(R"(out_by_n=.+\..+)");
    std::stringstream(file_pattern( R"(threads=\d+)")) >> conf_params.thread_number;
}
