 /**
  * @file IDT.cpp
  * @author Siddharth Mishra (brightprogrammer)
  * @date 01/11/22
  * @copyright Copyright (c) MIT License 2022, Siddharth Mishra
  */

#include "IDT.hpp"
#include "MemoryManager.hpp"
#include "Interrupts.hpp"
#include "Printf.hpp"

#define IDT_ENTRY_OFFSET_LOW_MASK uint64_t(0xffff)
#define IDT_ENTRY_OFFSET_MIDDLE_MASK uint64_t(0xffff0000)
#define IDT_ENTRY_OFFSET_HIGH_MASK uint64_t(0xffffffff00000000)

static IDTR idtr;

// set offset in this idt entry
void IDTEntry::SetOffset(uint64_t offset){
    offsetLow = uint16_t(offset & IDT_ENTRY_OFFSET_LOW_MASK);
    offsetMiddle = uint16_t((offset & IDT_ENTRY_OFFSET_MIDDLE_MASK) >> 16);
    offsetHigh = uint32_t((offset & IDT_ENTRY_OFFSET_HIGH_MASK) >> 32);
}

// get offset from this idt entry
uint64_t IDTEntry::GetOffset(){
    uint64_t offset = 0;

    offset = (uint64_t(offsetHigh) << 32) |
        (uint64_t(offsetMiddle) << 16) |
        uint64_t(offsetLow);

    return offset;
}

// helper function to set interrupt descriptor
void SetInterruptDescriptor(uint8_t entry, uint64_t isr, uint8_t flags){
    IDTEntry* gatedesc = reinterpret_cast<IDTEntry*>(idtr.offset + entry * sizeof(IDTEntry));
    gatedesc->SetOffset(isr);
    gatedesc->typeAttr = flags;
    gatedesc->selector = 0x08; // offset of kernelCode in GDT
}

void InstallIDT(){
    idtr.limit = PAGE_SIZE - 1;
    // allocate page always returns virtual address
    // and if paging is enabled then offset must be the virtual address
    idtr.offset = AllocatePage();

    // divide by zero
    SetInterruptDescriptor(0x00, reinterpret_cast<uint64_t>(DefaultInterruptHandlerNoError), IDT_TYPE_ATTR_TRAP_GATE);
    // debug
    SetInterruptDescriptor(0x01, reinterpret_cast<uint64_t>(DefaultInterruptHandlerNoError), IDT_TYPE_ATTR_TRAP_GATE);
    // NMI
    SetInterruptDescriptor(0x02, reinterpret_cast<uint64_t>(DefaultInterruptHandlerNoError), IDT_TYPE_ATTR_TRAP_GATE);
    // breakpoint
    SetInterruptDescriptor(0x03, reinterpret_cast<uint64_t>(DefaultInterruptHandlerNoError), IDT_TYPE_ATTR_TRAP_GATE);
    // overflow
    SetInterruptDescriptor(0x04, reinterpret_cast<uint64_t>(DefaultInterruptHandlerNoError), IDT_TYPE_ATTR_TRAP_GATE);
    // bound range exceeded
    SetInterruptDescriptor(0x05, reinterpret_cast<uint64_t>(DefaultInterruptHandlerNoError), IDT_TYPE_ATTR_TRAP_GATE);
    // invalid opcode
    SetInterruptDescriptor(0x06, reinterpret_cast<uint64_t>(DefaultInterruptHandlerNoError), IDT_TYPE_ATTR_TRAP_GATE);
    // device not avaiable
    SetInterruptDescriptor(0x07, reinterpret_cast<uint64_t>(DefaultInterruptHandlerNoError), IDT_TYPE_ATTR_TRAP_GATE);

    // create double fault handler
    SetInterruptDescriptor(0x08, reinterpret_cast<uint64_t>(DoubleFaultHandler), IDT_TYPE_ATTR_INTERRUPT_GATE);

    // co-processor segment overrun
    SetInterruptDescriptor(0x09, reinterpret_cast<uint64_t>(DefaultInterruptHandlerWithError), IDT_TYPE_ATTR_TRAP_GATE);
    // invalid tss
    SetInterruptDescriptor(0x0a, reinterpret_cast<uint64_t>(DefaultInterruptHandlerWithError), IDT_TYPE_ATTR_TRAP_GATE);
    // segment not present
    SetInterruptDescriptor(0x0b, reinterpret_cast<uint64_t>(DefaultInterruptHandlerWithError), IDT_TYPE_ATTR_TRAP_GATE);
    // stack segment fault
    SetInterruptDescriptor(0x0c, reinterpret_cast<uint64_t>(DefaultInterruptHandlerWithError), IDT_TYPE_ATTR_TRAP_GATE);

    // create general protection fault handler
    SetInterruptDescriptor(0x0d, reinterpret_cast<uint64_t>(GeneralProtectionFaultHandler), IDT_TYPE_ATTR_INTERRUPT_GATE);
    // create page fault handler
    SetInterruptDescriptor(0x0e, reinterpret_cast<uint64_t>(PageFaultHandler), IDT_TYPE_ATTR_INTERRUPT_GATE);

    // 0x0f is reserved

    // floating point error
    SetInterruptDescriptor(0x10, reinterpret_cast<uint64_t>(DefaultInterruptHandlerNoError), IDT_TYPE_ATTR_TRAP_GATE);
    // alignment cehck
    SetInterruptDescriptor(0x11, reinterpret_cast<uint64_t>(DefaultInterruptHandlerWithError), IDT_TYPE_ATTR_TRAP_GATE);
    // machine check
    SetInterruptDescriptor(0x12, reinterpret_cast<uint64_t>(DefaultInterruptHandlerNoError), IDT_TYPE_ATTR_TRAP_GATE);
    // simd floating point exception
    SetInterruptDescriptor(0x13, reinterpret_cast<uint64_t>(DefaultInterruptHandlerNoError), IDT_TYPE_ATTR_TRAP_GATE);
    // virtualization exception
    SetInterruptDescriptor(0x14, reinterpret_cast<uint64_t>(DefaultInterruptHandlerNoError), IDT_TYPE_ATTR_TRAP_GATE);
    // control protection exception
    SetInterruptDescriptor(0x15, reinterpret_cast<uint64_t>(DefaultInterruptHandlerWithError), IDT_TYPE_ATTR_TRAP_GATE);
    // rest up until 0x1f is reserved

    // first interrupt is mapped to 0x20
    // and keyboard interrupt is the second one
    // so offset = 0x21
    SetInterruptDescriptor(0x21, reinterpret_cast<uint64_t>(KeyboardInterruptHandler), IDT_TYPE_ATTR_INTERRUPT_GATE);

    // load the idtr strucg in idtr register
    asm volatile ("lidt %0"
                  :
                  : "m"(idtr));
}
