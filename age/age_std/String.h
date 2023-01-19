#ifndef AGE_STRING_H
#define AGE_STRING_H

#include <iostream>

namespace AGE {
    // Custom string type for AGE
    // TODO: Template string?
    // TODO: UTF-32 string?
    class String {
    protected:
        char* backing = nullptr;
        size_t capacity = 0;
        size_t length = 0;

        inline void validate_alloc() {
            if (length >= capacity) {
                char* prior = backing;
                size_t prior_capacity = capacity;

                // Covers special edge case insertions
                while (capacity < length)
                    capacity *= 2;

                backing = reinterpret_cast<char*>(malloc(capacity + 1));

                memcpy(backing, prior, prior_capacity);
                free(prior);
            }
        }

    public:
        //
        // Constants
        //

        // Let's hope we never come across a string this long!

        // -1, points to an invalid location!
        static const size_t INVALID_INDEX;

        //
        // Constructors / Destructors
        //
        String(const char* c_str);
        String(const char* c_str, size_t len);

        ~String();

        //
        // String member functions
        //

        // Returns the backing string as immutable (const)
        const char* c_str();

        // Returns the backing string as mutable
        char* data();

        // Returns the index of the first occurrence of a given char (searches front to back)
        // If this is invalid, size_t will equal String::INVALID_INDEX
        size_t find_first(char c);

        // Returns the last occurrence of a given char (searches back to front)
        // If this is invalid, size_t will equal String::INVALID_INDEX
        size_t find_last(char c);

        //
        // String functions
        //
        // Joins strings "a" and "b" together and returns the result
        static String concat(const String& a, const String& b);

        // Joins string "a" and char "b" together and returns the result
        static String concat(const String& a, char b);

        //
        // Operators
        //
        String operator+ (const String& rhs) const {
            return concat(*this, rhs);
        }

        String operator+ (char rhs) const {
            return concat(*this, rhs);
        }

        String& operator+= (const String& rhs) {
            size_t offset = length;
            length += rhs.length;

            validate_alloc();
            memcpy(backing + offset, rhs.backing, rhs.capacity);

            return *this;
        }

        String& operator+= (char rhs) {
            size_t offset = length;
            length++;

            validate_alloc();
            backing[offset] = rhs;

            return *this;
        }

        char operator[] (size_t index) {
            return backing[index];
        }

        friend std::ostream& operator<< (std::ostream& out, const String& str) {
            return out << str.backing;
        }
    };
}

#endif
