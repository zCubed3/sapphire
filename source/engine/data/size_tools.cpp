/*
MIT License

Copyright (c) 2023 zCubed (Liam R.)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "size_tools.hpp"

using namespace Sapphire;

size_t SizeTools::bytes_to_kib(size_t bytes) {
    return bytes / 1024;
}

size_t SizeTools::bytes_to_mib(size_t bytes) {
    return bytes / 1048576;
}

size_t SizeTools::bytes_to_gib(size_t bytes) {
    return bytes / 1073741824;
}

size_t SizeTools::kib_to_bytes(size_t kib) {
    return kib * 1024;
}

size_t SizeTools::mib_to_bytes(size_t mib) {
    return mib * 1048576;
}

size_t SizeTools::gib_to_bytes(size_t gib) {
    return gib * 1073741824;
}