#include "string_tools.h"

std::vector<std::string> StringTools::split(const std::string& string, char delimiter) {
    std::string dupe = string;
    size_t iter = dupe.find_first_of(delimiter);

    std::vector<std::string> strings;
    while (iter != std::string::npos) {
        std::string substr = dupe.substr(0, iter);
        strings.push_back(substr);

        dupe = dupe.substr(iter + 1);
        iter = dupe.find_first_of(delimiter);
    }

    if (!dupe.empty()) {
        strings.push_back(dupe);
    }

    return strings;
}

bool StringTools::is_whitespace(char c) {
    return c == '\0' || c == '\n' || c == '\t' || c == ' ';
}

bool StringTools::is_number(char c) {
    return c >= '0' && c <= '9';
}

std::string StringTools::trim(const std::string &string) {
    std::string dupe = string;
    while (is_whitespace(dupe.back())) {
        dupe.pop_back();
    }

    while (is_whitespace(dupe.front())) {
        dupe.erase(dupe.begin());
    }

    return dupe;
}