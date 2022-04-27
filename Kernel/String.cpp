#include "String.hpp"

// get length of string
size_t StringLength(const char* str){
    // initially 0
    size_t sz = 0;

    // count characters before null termination
    while(*(str + sz)){
        sz++;
    }

    return sz;
}
