#ifndef SAPPHIRE_REFLECT_MACROS_H
#define SAPPHIRE_REFLECT_MACROS_H

#include <engine/typing/hash_functions.h>

// Inspired by Godot's RTTI system

// TODO: Multiple parents?
#define REFLECT_BASE(CLASS_NAME)                                                            \
private:                                                                                    \
    friend class ClassRegistry;                                                             \
                                                                                            \
protected:                                                                                  \
    static const char* get_class_name_static() { return #CLASS_NAME; }                      \
    static size_t get_class_hash_static() { return hash_cstr_djb2(#CLASS_NAME); }           \


#define REFLECT_INHERITANCE_CHECKS(CLASS_NAME)                                              \
public:                                                                                     \
    template<class TParent>                                                                 \
    bool is_child_of() {                                                                    \
        return ClassRegistry::is_child_of<CLASS_NAME, TParent>();                           \
    }                                                                                       \


#define REFLECT_BASE_CLASS(CLASS_NAME)                                                      \
    REFLECT_BASE(CLASS_NAME)                                                                \
    static const char* get_parent_class_name_static() { return nullptr; }                   \
    static size_t get_parent_class_hash_static() { return -1; }                             \
                                                                                            \
    REFLECT_INHERITANCE_CHECKS(CLASS_NAME)                                                  \


#define REFLECT_CLASS(CLASS_NAME, PARENT_NAME)                                              \
    REFLECT_BASE(CLASS_NAME)                                                                \
    static const char* get_parent_class_name_static() { return #PARENT_NAME; }              \
    static size_t get_parent_class_hash_static() { return hash_cstr_djb2(#PARENT_NAME); }   \
                                                                                            \
    REFLECT_INHERITANCE_CHECKS(CLASS_NAME)                                                  \

#endif
