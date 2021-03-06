/**
 * @file MemoryManger.hpp
 * @author Siddharth Mishra (brightprogrammer)
 * @date 05/08/22
 * @brief Virtual and Physical Memory Manager. (PMM and VMM).
 * @copyright MIT License 2022 Siddharth Mishra
 * */

#ifndef MEMORYMANAGER_H_
#define MEMORYMANAGER_H_

#include <cstdint>
#include <cstddef>

#include "stivale2.hpp"
#include "Common.hpp"

extern const u64 MEM_PHYS_OFFSET;
extern const u64 PAGE_SIZE;

/**
 * @brief Initialize memory manager.
 * This will initialize both PMM and VMM.
 *
 * @param memmap Memory map provided by bootloader.
 * */
void InitializeMemoryManager(stivale2_struct_tag_memmap* memmap);

/******************** Allocation Functions ********************/

/**
 * @brief Allocate a single page of size=PAGE_SIZE.
 *
 * @return Virtual address of newly allocated page.
 * */
[[nodiscard]] u64 AllocatePage();

/**
 * @brief Allocate more than once pages at once.
 * Allocates max PAGE_SIZE number of pages at once.
 *
 * @return Array of addresses of allocated pages.
 * */
[[nodiscard]] u64* AllocatePages();

/******************** Freeing Functions ********************/

/**
 * @brief Free a single page of size=PAGE_SIZE.
 *
 * @param vaddr Virtual address of newly allocated page.
 * */
void FreePage(u64 vaddr);

/**
 * @brief Free more than once pages at once.
 * Free max PAGE_SIZE number of pages at once.
 *
 * @param pages Array of addresses of allocated pages.
 * */
void FreePages(u64* pages);

/******************** Conversion Functions ********************/

/**
 * @brief Convert virtual address to physical address.
 *
 * @return Valid physical address for a valid virtual address.
 * */
u64 VirtualToPhysicalAddress(u64 addr);

/**
 * @brief Convert physical address to virtual address.
 *
 * @return Valid virtual address for a valid physical address.
 * */
u64 PhysicalToVirtualAddress(u64 addr);

/******************** Statistics Functions ********************/

/**
 * @brief Get total free memory.
 *
 * @return Number of bytes free.
 * */
u64 GetFreeMemory();

/**
 * @brief Get total used memory.
 *
 * @return Number of bytes of memory used.
 * */
u64 GetUsedMemory();

/**
 * @brief Get total reserved memory.
 *
 * @return Number of bytes of reserved memory.
 * */
u64 GetReservedMemory();

/**
 * @brief Get total memory of system.
 *
 * @return Number of bytes of total memory.
 * */
u64 GetTotalMemory();

/**
 * @brief Display memory statistics.
 * */
void ShowMemoryStatistics();

/**
 * @brief Different page flags that can be used while mapping
 * a physical address to new virtual address.
 * */
enum PageFlags {
    MAP_PRESENT = 1 << 0,
    MAP_READ_WRITE = 1 << 1,
    MAP_SUPERVISOR_ONLY = 1 << 2,
    MAP_WRITE_THROUGH = 1 << 3,
    MAP_CACHE_DISABLED = 1 << 4,
    MAP_ACCESSED = 1 << 5,
    MAP_LARGER_PAGES =  1 << 7,
    MAP_CUSTOM0 = 1 << 9,
    MAP_CUSTOM1 = 1 << 10,
    MAP_CUSTOM2 = 1 << 11,
    MAP_NO_EXECUTE = uint64_t(1) << 63 // only if supported
};

/**
 * @brief This represents a single entry (node) in a page table level.
 * */
struct Page {
    uint64_t value; //
    // set given flags to true
    void SetFlags(uint64_t flags);
    // set given flags to false
    void UnsetFlags(uint64_t flags);
    // if given flags are true then true is returned
    bool GetFlags(uint64_t flags);
    // set physical address
    void SetAddress(uint64_t address);
    // get physical address (4kb aligned always)
    uint64_t GetAddress();
};


// page table and page map level 4 use the same structure
struct PageTable {
    Page entries[512];
} __attribute__((aligned(0x1000)));


/**
 * @brief Map a physical address to given virtual address.
 *
 * @param vaddr Virtual address to map to.
 * @param paddr Physical address to map to.
 * @param flags Flags of mapped address.
 * */
void MapMemory(u64 vaddr, u64 paddr, u64 flags);

/**
 * @brief Unmap memory mapped for given virtual address.
 *
 * @param vaddr Virtual address of mapped memory.
 * */
void UnmapMemory(u64 vaddr);

#endif // MEMORYMANAGER_H_
