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

bool StringTools::is_letter(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

char StringTools::to_lower(char c) {
    if (c >= 'A' && c <= 'Z') {
        c += 32;
    }

    return c;
}

char StringTools::to_upper(char c) {
    if (c >= 'a' && c <= 'z') {
        c -= 32;
    }

    return c;
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

std::string StringTools::to_lower(const std::string &string) {
    std::string lower = string;

    for (char& c : lower) {
        c = to_lower(c);
    }

    return lower;
}

std::string StringTools::to_upper(const std::string &string) {
    std::string upper = string;

    for (char& c : upper) {
        c = to_upper(c);
    }

    return upper;
}

bool StringTools::compare(const std::string &lhs, const std::string &rhs, bool caseless) {
    size_t size = std::min(lhs.size(), rhs.size());

    for (size_t c = 0; c < size; c++) {
        char lhs_c = lhs[c];
        char rhs_c = rhs[c];

        if (lhs_c == rhs_c) {
            continue;
        }

        if (caseless && to_lower(lhs_c) == to_lower(rhs_c)) {
            continue;
        }

        return false;
    }

    return true;
}
