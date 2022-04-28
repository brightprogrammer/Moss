/**
 * @file KernelEntry.cpp
 * @author Siddharth Mishra (brightprogrammer)
 * @date 04/27/22
 * @brief Contains kernel entry code.
 * @copyright MIT License 2022 Siddharth Mishra
 * */

// we can't include most of the headers
// only selected headers ans others require libc
// which we won't have when our kernel boots up
#include <stdint.h>
#include <stddef.h>

// stivale 2 header specification
#include "stivale2.h"
#include "Common.hpp"
#include "Renderer.hpp"
#include "Printf.hpp"

// placeholder for NULL value in uintptr_t instead of using 0 again and again
#define NULLADDR 0

// we cannot dynamically allocate anything now
// so will initialize stack as an array in .bss and tell stivale
// where our stack is
static uint8_t stack[8192]; // 8KB stack

// we need a framebuffer from stivale on bootup so we
// need to tell stivale that we need a framebuffer instead of
// CGA-compatible text mode.
static struct stivale2_header_tag_framebuffer framebuffer_hdr_tag = {
    .tag = {
        // which type of tag is this
        .identifier = STIVALE2_HEADER_TAG_FRAMEBUFFER_ID,
        // this must be a pointer address, NULL in this case
        .next = NULLADDR
    },
    // set all framebuffer specifics to 0 and let bootloader decide
    .framebuffer_width = 0,
    .framebuffer_height = 0,
    .framebuffer_bpp = 0
};

// The stivale2 specification says we need to define a "header structure".
// This structure needs to reside in the .stivale2hdr ELF section in order
// for the bootloader to find it. We use this __attribute__ directive to
// tell the compiler to put the following structure in said section.
__attribute__((section(".stivale2hdr"), used))
static struct stivale2_header stivale_hdr = {
    // The entry_point member is used to specify an alternative entry
    // point that the bootloader should jump to instead of the executable's
    // ELF entry point. We do not care about that so we leave it zeroed.
    .entry_point = NULLADDR,
    // Let's tell the bootloader where our stack is.
    // We need to add the sizeof(stack) since in x86(_64) the stack grows
    // downwards.
    .stack = (uintptr_t)stack + sizeof(stack),
    // Bit 1, if set, causes the bootloader to return to us pointers in the
    // higher half, which we likely want since this is a higher half kernel.
    // Bit 2, if set, tells the bootloader to enable protected memory ranges,
    // that is, to respect the ELF PHDR mandated permissions for the executable's
    // segments.
    // Bit 3, if set, enables fully virtual kernel mappings, which we want as
    // they allow the bootloader to pick whichever *physical* memory address is
    // available to load the kernel, rather than relying on us telling it where
    // to load it.
    // Bit 4 disables a deprecated feature and should always be set.
    .flags = (1 << 1) | (1 << 2) | (1 << 3) | (1 << 4),
    // This header structure is the root of the linked list of header tags and
    // points to the first one in the linked list.
    .tags = (uintptr_t)&framebuffer_hdr_tag
};


// We will now write a helper function which will allow us to scan for tags
// that we want FROM the bootloader (structure tags).
void *stivale2_get_tag(struct stivale2_struct *stivale2_struct, uint64_t id) {
    stivale2_tag *current_tag = (stivale2_tag*)stivale2_struct->tags;
    for (;;) {
        // If the tag pointer is NULL (end of linked list), we did not find
        // the tag. Return NULL to signal this.
        if (current_tag == NULL) {
            return NULL;
        }

        // Check whether the identifier matches. If it does, return a pointer
        // to the matching tag.
        if (current_tag->identifier == id) {
            return current_tag;
        }

        // Get a pointer to the next tag in the linked list and repeat.
        current_tag = (stivale2_tag*)current_tag->next;
    }
}

void  Halt(){
    while(true){
        asm("hlt");
    }
}

// The following will be our kernel's entry point.
extern "C" { // stop compiler from mangling function name
    void KernelEntry(struct stivale2_struct *stivale2_struct) {
        // Let's get the terminal structure tag from the bootloader.
        // notice this is struct_tag and previous one was header_tag
        // struct_tags are returned by bootloader and header_tags are used to request features from bootloader
        struct stivale2_struct_tag_framebuffer *framebuffer_tag;
        framebuffer_tag = (stivale2_struct_tag_framebuffer*)stivale2_get_tag(stivale2_struct, STIVALE2_STRUCT_TAG_FRAMEBUFFER_ID);

        // Check if the tag was actually found.
        if (framebuffer_tag == NULL) {
            // It wasn't found, just hang...
            Halt();
        }

        // load framebuffer info
        LoadFramebufferInfo(framebuffer_tag);

        uint32_t x = 0xcafebabe;
        Printf("Moss Operating System. \nHexadecimal and Printf check (this must print 0xcafebabe) : 0x%x", x);

        // We're done, just hang...
        Halt();
    }
}
