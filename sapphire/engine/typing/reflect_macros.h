#ifndef SAPPHIRE_REFLECT_MACROS_H
#define SAPPHIRE_REFLECT_MACROS_H

#include <engine/typing/hash_functions.h>

// TODO: Multiple parents?
#define REFLECT_BASE(CLASS_NAME)                                                    \
public:                                                                             \
    const char* get_class_name() { return #CLASS_NAME; }                            \
    size_t get_class_hash() { return hash_cstr_djb2(#CLASS_NAME); }                 \


#define REFLECT_INHERITANCE_CHECKS(CLASS_NAME)                                      \
public:                                                                             \
    template<class TParent>                                                         \
    bool is_child_of() {                                                            \
        return ClassRegistry::is_child_of<CLASS_NAME, TParent>();                   \
    }                                                                               \


#define REFLECT_BASE_CLASS(CLASS_NAME)                                              \
    REFLECT_BASE(CLASS_NAME)                                                        \
    const char* get_parent_class_name() { return nullptr; }                         \
    size_t get_parent_class_hash() { return -1; }                                   \
                                                                                    \
    REFLECT_INHERITANCE_CHECKS(CLASS_NAME)                                          \


#define REFLECT_CLASS(CLASS_NAME, PARENT_NAME)                                      \
    REFLECT_BASE(CLASS_NAME)                                                        \
    const char* get_parent_class_name() { return #PARENT_NAME; }                    \
    size_t get_parent_class_hash() { return hash_cstr_djb2(#PARENT_NAME); }         \
                                                                                    \
    REFLECT_INHERITANCE_CHECKS(CLASS_NAME)                                          \

#endif
