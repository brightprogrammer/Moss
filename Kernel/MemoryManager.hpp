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

#include "stivale2.h"
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
void ShowStatistics();

#endif // MEMORYMANAGER_H_
