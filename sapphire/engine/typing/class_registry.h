#ifndef SAPPHIRE_CLASS_REGISTRY_H
#define SAPPHIRE_CLASS_REGISTRY_H

#include <engine/typing/hash_functions.h>
#include <engine/typing/reflect_macros.h>

#include <unordered_map>

class ClassRegistry {
public:
    enum ClassRegisterStatus {
        CLASS_REGISTER_SUCCESS,
        CLASS_REGISTER_NULLPTR
    };

    struct ClassEntry {
        const char* name = nullptr;
        const char* parent_name = nullptr;

        size_t hash = static_cast<size_t>(-1);
        size_t parent_hash = static_cast<size_t>(-1);
    };

protected:
    static std::unordered_map<size_t, ClassEntry> class_map;

public:
    // TODO: Assertions?
    template<class T>
    static ClassRegisterStatus register_class() {
        // We have to construct a temporary instance of T
        const char* name = T::get_class_name_static();
        size_t hash = T::get_class_hash_static();

        if (name == nullptr) {
            return CLASS_REGISTER_NULLPTR;
        }

        ClassEntry entry {};

        entry.name = name;
        entry.hash = hash;

        // Check if the class has a parent
        const char* parent_name = T::get_parent_class_name_static();

        if (parent_name != nullptr) {
            entry.parent_name = parent_name;
            entry.parent_hash = T::get_parent_class_hash_static();
        }

        class_map.emplace(entry.hash, entry);
        return CLASS_REGISTER_SUCCESS;
    }

    template<class TChild, class TParent>
    static bool is_child_of(TChild* instance) {
        // We need to traverse up the type tree if there isn't a match initially
        size_t hash = TChild::get_class_hash();
        size_t parent_hash = TParent::get_class_hash();

        while (true) {
            if (hash == -1 || parent_hash == -1) {
                return false;
            }

            ClassEntry entry = class_map[hash];
            if (entry.parent_hash != parent_hash) {
                hash = entry.parent_hash;
            } else {
                return true;
            }
        }
    }

    template<class TParent, class TChild>
    static TParent* as(TChild* p_child) {
        // Verify the types are related
        if (is_child_of<TChild, TParent>()) {
            return reinterpret_cast<TParent*>(p_child);
        } else {
            return nullptr;
        }
    }
};

#endif
