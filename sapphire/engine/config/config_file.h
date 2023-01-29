#ifndef SAPPHIRE_CONFIG_FILE_H
#define SAPPHIRE_CONFIG_FILE_H

#include <string>
#include <vector>

class ConfigFile {
public:
    struct ConfigEntry {
        std::string name;
        std::string string_value;

        std::vector<std::string> get_string_list();
        int try_get_int(int fallback = 0);
        float try_get_float(float fallback = 0.0F);
    };

    struct ConfigSection {
        std::string name;
        std::vector<ConfigEntry> entries;

        void clear();
        void push_entry(const std::string &name, const std::string &string_value);

        std::string try_get_string(const std::string &name, const std::string &fallback = "");
        std::vector<std::string> try_get_string_list(const std::string &name, const std::vector<std::string> &fallback = {});
        int try_get_int(const std::string &name, int fallback = 0);
        float try_get_float(const std::string &name, float fallback = 0.0F);
    };

    ConfigSection global_section;
    std::vector<ConfigSection> sections;

    void read_from_path(const std::string &path);
    void read(const std::string &contents);

    std::string try_get_string(const std::string &name, const std::string &section, const std::string &fallback = "");
    std::vector<std::string> try_get_string_list(const std::string &name, const std::string &section, const std::vector<std::string> &fallback = {});
    int try_get_int(const std::string &name, const std::string &section = "", int fallback = 0);
    float try_get_float(const std::string &name, const std::string &section = "", float fallback = 0);
};


#endif
