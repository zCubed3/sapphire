#include "String.h"

#include <cstring>

namespace AGE {
    //
    // Statics
    //
    const size_t String::INVALID_INDEX = -1;

    //
    // Constructors / Destructors
    //
    String::String(const char* c_str) {
        capacity = strlen(c_str);
        length = capacity;

        backing = reinterpret_cast<char*>(malloc(capacity + 1));
        backing[capacity] = '\0';

        memcpy(backing, c_str, capacity);
    }

    String::String(const char* c_str, size_t len) {
        backing = reinterpret_cast<char*>(malloc(len + 1));
        backing[len] = '\0';

        memcpy(backing, c_str, len);

        capacity = len;
        length = len;
    }

    String::~String() {
        free(backing);
    }

    //
    // String member functions
    //
    const char* String::c_str() {
        return backing;
    }

    char* String::data() {
        return backing;
    }

    // Returns the index of the first occurrence of a given char (searches front to back)
    // If this is invalid, size_t will equal String::INVALID_INDEX
    size_t String::find_first(char c) {
        size_t idx = 0;
        while (backing[idx] != c) {
            if (idx >= length)
                return INVALID_INDEX;

            idx++;
        }

        return idx;
    }

    // Returns the last occurrence of a given char (searches back to front)
    // If this is invalid, size_t will equal String::INVALID_INDEX
    size_t String::find_last(char c) {
        size_t idx = length - 1;
        while (backing[idx] != c) {
            if (idx == 0)
                return INVALID_INDEX;

            idx--;
        }

        return idx;
    }

    //
    // String functions
    //
    String String::concat(const String& a, const String& b) {
        size_t concat_len = a.length + b.length;

        char* concat = reinterpret_cast<char*>(malloc(concat_len + 1));
        concat[concat_len] = '\0';

        memcpy(concat, a.backing, a.length);
        memcpy(concat + a.capacity, b.backing, b.length);

        return {concat, concat_len};
    }

    String String::concat(const String& a, char b) {
        size_t concat_len = a.length + 1;

        char* concat = reinterpret_cast<char*>(malloc(concat_len + 1));
        concat[concat_len] = '\0';

        memcpy(concat, a.backing, a.capacity);
        concat[a.length] = b;

        return {concat, concat_len};
    }
}