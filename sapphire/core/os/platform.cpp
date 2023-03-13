#include "platform.hpp"

Platform *Platform::singleton = nullptr;

const Platform *Platform::get_singleton() {
    return singleton;
}