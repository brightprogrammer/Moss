/**
 * @file IDT.hpp
 * @author Siddharth Mishra (brightprogrammer)
 * @date 01/11/22
 * @copyright Copyright (c) MIT License 2022, Siddharth Mishra
 */

#ifndef IDT_HPP
#define IDT_HPP

#include "Common.hpp"
#include <cstdint>

// type attr field structure
// ;---;-----;---;----------;
// | 7 | 6,5 | 4 | 3,2,1,0  |
// ;---;-----;---;----------;
// | P | DPL | S | GateType |
// ;---;-----;---;----------;
// P must always be flagged if this gate is present!
#define IDT_TYPE_ATTR_INTERRUPT_GATE uint8_t(0b10001110)
#define IDT_TYPE_ATTR_CALL_GATE uint8_t(0b10001100)
#define IDT_TYPE_ATTR_TRAP_GATE uint8_t(0b10001111)

// idt entry structure
// interrupt gate and trap gate have same structure
// https://wiki.osdev.org/Interrupt_Descriptor_Table
struct IDTEntry{
    uint16_t offsetLow;
    uint16_t selector; // switch to the "selected" gdt or ldt segment segment
    uint8_t ist;

    /*
     --- examples of typeAttr value ---
     64-bit Interrupt Gate: 0x8E (p=1, dpl=0b00, type=0b1110 => type_attributes=0b1000_1110=0x8E)
     64-bit Trap Gate: 0x8F (p=1, dpl=0b00, type=0b1111 => type_attributes=1000_1111b=0x8F)
     --- examples of type values ---
     0b1110 or 0xE: 64-bit Interrupt Gate
     0b1111 or 0xF: 64-bit Trap Gate
     */
    uint8_t typeAttr;

    uint16_t offsetMiddle;
    uint32_t offsetHigh;

    // this is reserved for future use
    uint32_t reserved;

    void SetOffset(uint64_t offset);
    uint64_t GetOffset();
} __attribute__((packed));

// idtr register structure
struct IDTR{
    uint16_t limit;
    uint64_t offset;
} __attribute__((packed));

// helper method to set an interrupt descriptor in IDT
// entry must be the id of interrupt to be handled
// isr must be pointer to function that will handle the interrupt
// flags must be a valid flag to define the type of interrupt descriptor
void SetInterruptDescriptor(uint8_t entry, void* isr, uint8_t flags);

// you know what this does!
void InstallIDT();

#endif // IDT_HPP
