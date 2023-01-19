#ifndef AGE_VECTOR_H
#define AGE_VECTOR_H

#include <cstring>

namespace AGE {
    // Replacement Vector type
    template<typename T>
    class Vector {
    protected:
        T* backing;
        size_t capacity = 0;
        size_t length = 0;

        void validate_alloc() {
            if (length >= capacity) {
                T* prior = backing;

                capacity *= 2;
                backing = new T[capacity];

                memcpy(backing, prior, sizeof(T) * length);
                delete[] prior;
            }
        }

    public:
        //
        // Constructors / Destructors
        //
        Vector() {
            length = 0;
            capacity = 4;
            backing = new T[capacity];
        }

        Vector(size_t capacity) {
            length = 0;
            backing = new T[capacity];
            this->capacity = capacity;
        }

        ~Vector() {
            // TODO: Free pointers within the vector?
            delete[] backing;
        }

        //
        // Vector functions
        //

        // Returns the current number of occupied elements
        size_t size() {
            return length;
        }

        // Pushes a new item to the top of the vector
        // Returns the new length
        size_t push_back(T item) {
            validate_alloc();

            backing[length++] = item;
            return item;
        }

        // TODO: Emplace?

        // TODO: Safety
        T pop_back() {
            validate_alloc();
            return backing[--length];
        }

        //
        // Operators
        //
        T operator[] (size_t index) {
            return backing[index];
        }
    };
}

#endif
