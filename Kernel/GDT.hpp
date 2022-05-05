/**
 * @file GDT.hpp
 * @author Siddharth Mishra
 * @date 05/06/22
 * @brief global descriptor table function definitions
 * @copyright Copyright (c) 2022, Siddharth Mishra. All rights reserved.
*/

#ifndef GDT_HPP
#define GDT_HPP

#include <cstdint>
#include "Common.hpp"

// struct to represent gdtr
struct GDTR{
    uint16_t table_limit;
    uint64_t table_base_address;
} __attribute__((packed));

// implementation of a general segment descriptor
struct GDTEntry {
    uint16_t segment_limit_low;
    uint16_t base_address_low;
    uint8_t base_address_middle;
    uint8_t access_flags;
    uint8_t attributes;
    uint8_t base_address_high;
} __attribute__((packed));

struct GDT{
    GDTEntry null;
    GDTEntry kernelCode;
    GDTEntry kernelData;
    GDTEntry userCode;
    GDTEntry userData;
} __attribute__((packed)) __attribute__((aligned(0x1000)));

// install kernel's global descriptor table in gdtr
void InstallGDT();

#endif // GDT_HPP
