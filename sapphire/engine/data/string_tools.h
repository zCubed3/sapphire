#ifndef SAPPHIRE_STRING_TOOLS_H
#define SAPPHIRE_STRING_TOOLS_H

#include <string>
#include <vector>

class StringTools {
public:
    StringTools() = delete;

    static std::vector<std::string> split(const std::string& string, char delimiter = ';');

    static bool is_whitespace(char c);
    static bool is_number(char c);

    static std::string trim(const std::string& string);
};

#endif
