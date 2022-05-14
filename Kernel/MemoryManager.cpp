/**
 * @file MemoryManger.cpp
 * @author Siddharth Mishra (brightprogrammer)
 * @date 05/08/22
 * @brief Virtual and Physical Memory Manager. (MM. and VMM).
 * @copyright MIT License 2022 Siddharth Mishra
 * */

#include "MemoryManager.hpp"
#include "Printf.hpp"
#include "String.hpp"

// virtual address where all address are mapped
constexpr u64 MEM_PHYS_OFFSET = 0xffff800000000000;

// size of one page of memory
constexpr u64 PAGE_SIZE = 4*KB;

// address mask where physical address is stored in a page table entry
constexpr u64 PAGE_PHYSICAL_ADDRESS_MASK = 0x000ffffffffff000;

// virtual address where kernel is mapped
constexpr u64 KERNEL_VIRT_BASE = 0xffffffff80000000;

// stores memory manager information
struct MemoryManager{
    bool is_initialized = false;

    u64 free_memory = 0;
    u64 used_memory = 0;
    u64 reserved_memory = 0;
    u64 total_memory = 0;

    // array to store addresses of free pages
    u64* page_stack = nullptr;
    u64 page_stack_top = 0;

    // total number of pages in memory
    u64 total_page_count = 0;
    u64 num_pages_used_by_stack = 0;
    u64 page_stack_size = 0;

    u64 mmap_entries_count = 0;
    stivale2_mmap_entry* mmap_entries = nullptr;


    // this is called pml4 because Moss uses
    // 4 level of paging. Some operating systems use 5 level paging.
    // More levels of paging means more addresses can be mapped.
    PageTable* pml4 = nullptr;
    u64 pml4_paddr = 0;
};

// single static instance of memory manager
static MemoryManager mm;

/* ------------------ ALGORITHM EXPLANATION --------------------
 *                STACK BASED MEMORY ALLOCATOR
 *
 * mm.page_stack here is an array that stores addresses of all pages
 * that can be allocated.
 * mm.used_page_stack here is an array that stores addresses of all pages
 * that are allocated.
 *
 * mm.total_page_count is the maximum number of pages that
 * can be allocated.
 *
 * mm.num_pages_used_by_stack is the number of pages used by
 * used_page_stack and page_stack array.
 *
 * When a new page is to be allocated, first we check whether
 * page_stack_top is non zero. The address is then searched
 * in used pages stack to make sure no double allocation happens.
 * If we can allocate pages then we return the page address from
 * the top of stack and then mm.page_stack_top is then decreased
 * and mm.used_page_stack_top is increased. Address of used page is
 * pushed to top of used pages stack.
 *
 * When a page is to be freed, the given page is first searched
 * in the used pages stack. If it is present in stack then the address
 * is pushed to top of free pages stack and the entry in which
 * the given page address was stored is set to 0 in used pages
 * stack.
 *
 * This makes sure that there is no double free or double allocation.
 *
 * <----------- mm.page_stack_top ---------->
 * ;----------------------------------------;--------------------;
 * ; free pages ||||||||||||||||||||||||||||;|||||||| used pages ;
 * ;----------------------------------------;--------------------;
 *
 *
 * A system with 1GiB total memory will have 0.5MiB sized page_stack
 * This means 1TiB memory will need only 512M stack size
 * Space complexity : O(n)
 * Time complexity : O(1)
 *
 * One of the cons of this algo is that this can cause fragmentation.
 * To solve this we can sort the used and free stacks after a certain
 * number of alloc and free.
 *
 * */

u64 PhysicalToVirtualAddress(u64 paddr){
    return paddr + MEM_PHYS_OFFSET;
}

u64 VirtualToPhysicalAddress(u64 vaddr){
    return vaddr - MEM_PHYS_OFFSET;
}

void InitializePhysicalMemoryManager(stivale2_struct_tag_memmap* mmap){
    if(mm.is_initialized){
        return;
    }

    mm.mmap_entries_count = mmap->entries;
    mm.mmap_entries = mmap->memmap;

    // First step is to find the largest claimable block
    // we'll keep our pages array at the beginning of the largest block
    // We also have to find the total available memory
    u64 largest_mem_block_base = 0, largest_mem_block_size = 0;
    for(size_t i = 0; i < mm.mmap_entries_count; i++){
        if(mm.mmap_entries[i].length > largest_mem_block_size){
            largest_mem_block_base = mm.mmap_entries[i].base;
            largest_mem_block_size = mm.mmap_entries[i].length;
        }

        // if memory is usable, then it's free
        if(mm.mmap_entries[i].type == STIVALE2_MMAP_USABLE){
            mm.free_memory += mm.mmap_entries[i].length;
        }else{
            mm.reserved_memory += mm.mmap_entries[i].length;
        }
    }

    // Second step is to calculate the total size needed for the stack
    // calculate the size of page stack
    // page stack size is the number of pages that can be allocated
    mm.total_page_count = mm.free_memory / PAGE_SIZE;
    // calculate required numer of pages to allocate for stack
    mm.num_pages_used_by_stack = ((mm.total_page_count * 8) / PAGE_SIZE) + 1;
    mm.page_stack_size = mm.num_pages_used_by_stack * PAGE_SIZE;
    // check if largest block can provide this much space or not
    if(largest_mem_block_size <= mm.page_stack_size){
        ColorPrintf(COLOR_RED, COLOR_BLACK, "[-] Insufficient memory to initialize PhysicalMemoryManager\n");
        Printf("\tLargest memory block size : %li KB\n", (largest_mem_block_size / KB));
        Printf("\tMemory required : %li KB\n", (mm.page_stack_size / KB));
        while(true)asm("hlt");
    }

    // set page stacks at the start of this memory region
    // create page_stack
    mm.page_stack = reinterpret_cast<u64*>(PhysicalToVirtualAddress(largest_mem_block_base));

    // mark this memory as used
    mm.free_memory -= mm.page_stack_size;
    mm.used_memory += mm.page_stack_size;

    // Find usable page frames in small blocks first.
    // If these pages are at the bottom of the stack then
    // there is less chance of them being allocated.
    for(size_t i = 0 ; i < mm.mmap_entries_count; i++){
        // if this block is usable and not the largest block
        if((mm.mmap_entries[i].type == STIVALE2_MMAP_USABLE) && // memory is usable
           (mm.mmap_entries[i].base != largest_mem_block_base) && // not the lagest block
           (mm.mmap_entries[i].length >= PAGE_SIZE)){ // has atleast 1 page

            // start getting pages
            u64 num_pages = mm.mmap_entries[i].length / PAGE_SIZE;
            for(size_t j = 0; j < num_pages ; j++){
                // if this page's limit is less than this block's limit then add it
                if((mm.mmap_entries[i].base + (j + 1) * PAGE_SIZE) <= mm.mmap_entries[i].base + mm.mmap_entries[i].length){
                    mm.page_stack[mm.page_stack_top] = PhysicalToVirtualAddress(mm.mmap_entries[i].base + (j * PAGE_SIZE));
                    mm.page_stack_top++;
                }
            }
        }
    }

    // Finally find available pages in largest memory block.
    // make sure to exclude pages used by stacks
    u64 start_address = largest_mem_block_base + mm.num_pages_used_by_stack * PAGE_SIZE;
    u64 max_available_pages =  (largest_mem_block_size - (mm.num_pages_used_by_stack * PAGE_SIZE)) / PAGE_SIZE;

    for(size_t i = 0; i < max_available_pages; i++){
        // check if page's limit is less than memory block's limit
        if((start_address + (i + 1) * PAGE_SIZE) <= (largest_mem_block_base + largest_mem_block_size)){
            // limine already maps addresses to higher half
            // so we have to add that offset to all page addresses
            mm.page_stack[mm.page_stack_top] = PhysicalToVirtualAddress(start_address + (i * PAGE_SIZE));
            mm.page_stack_top++;
        }
    }
}

u64 GetFreeMemory(){ return mm.free_memory; }
u64 GetUsedMemory(){ return mm.used_memory; }
u64 GetReservedMemory(){ return mm.reserved_memory; }
u64 GetTotalMemory(){ return mm.free_memory + mm.used_memory + mm.reserved_memory; }

// allocate's a single page
// this pops out the top element from stack and returns the value
u64 AllocatePage(){
    if(mm.page_stack_top == 0){
        Printf("Out Of Memory!");
        while(true)asm("hlt");
    }

    mm.free_memory -= PAGE_SIZE;
    mm.used_memory += PAGE_SIZE;

    // get page addr
    mm.page_stack_top--;
    u64 page_vaddr = mm.page_stack[mm.page_stack_top];

    return page_vaddr;
}

// allocate more than one pages at a time
// max allowed size to allocate at a time is 512 pages
// this is equivalent to 2MB memory at a time
u64* AllocatePages(size_t n){
    // n = 512 means 1 page
    if(n <= 512){
        // allocate memory for return array
        u64* page_frames = reinterpret_cast<u64*>(AllocatePage());
        // allocate requested number of pages and return the array
        for(size_t i = 0 ; i < n; i++){
            page_frames[i] = AllocatePage();
        }

        return page_frames;
    }

    return nullptr;
}

// free a single page
void FreePage(u64 page_vaddr){
    // check that such memory actually exists
    bool freeable = false;

    // first search whether this page was already allocated or not
    u64 posn = 0;
    for(size_t i = mm.page_stack_top; i < mm.page_stack_size; i++){
        if(mm.page_stack[i] == page_vaddr){
            posn = i;
            freeable = true;
        }
    }

    // free memory
    if(freeable){
        /*
         *  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% BEFORE FREEING %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
         *
         *                  unfreed memory
         *                        |
         *                        | /----------------> page stack top
         *                        |/
         *                        v
         * ;---------------------;-------;
         * ; | | | | | | | | | | |%|%|%|%|
         * ;---------------------;-------;
         *                            ^
         *                            |
         *                            |
         *                   memory to be freed
         *
         *  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% AFTER FREEING %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
         *
         *                   freed memory
         *                        |
         *                        | ;----------------> new stack top
         *                        | |
         *                        v v
         * ;-----------------------;-----;
         * ; | | | | | | | | | | | |%|%|%|
         * ;-----------------------;-----;
         *                            ^
         *                            |
         *                            |
         *                    swapped memory
         *
         *
         *
         * */

        // swap freed memory with non freed memory
        u64 temp = mm.page_stack[posn];
        mm.page_stack[posn] = mm.page_stack[mm.page_stack_top];
        mm.page_stack[mm.page_stack_top] = temp;

        // mm.page_stack[mm.page_stack_top] = page_vaddr;
        mm.page_stack_top++;
        mm.used_memory -= PAGE_SIZE;
        mm.free_memory += PAGE_SIZE;
    }else{
        Printf("Attemt to free a reserved page! : Address = %lx\n", PhysicalToVirtualAddress(page_vaddr));
    }
}

// free pages at addresses stored at the value provided in the element of
// this pages array
void FreePages(u64 *pages, size_t n){
    if(n <= 512){
        for(size_t i = 0; i < n; i++){
            FreePage(pages[i]);
        }
    }else{
        Printf("Call to free pages with size greater than max allowed size");
    }
}

// print memmoy statistics
void ShowMemoryStatistics(){
    ColorPrintf(COLOR_YELLOW, COLOR_BLACK, "[+] Memory Stats : \n");
    Printf("\tFree Memory : %lu KB\n", (mm.free_memory/KB));
    Printf("\tUsed Memory : %lu KB\n", (mm.used_memory/KB));
    Printf("\tReserved Memory : %lu KB\n", (mm.reserved_memory/KB));
    Printf("\tFree Pages : %lu pages\n", (mm.page_stack_top));
    Printf("\tTotal Pages : %lu pages\n", (mm.total_memory));
}

// turn on given flags
void Page::SetFlags(u64 flags){
    value |= flags;
}

// turn off given flags
void Page::UnsetFlags(u64 flags){
    value &= (~flags | PAGE_PHYSICAL_ADDRESS_MASK);
}

// get state of given flags
bool Page::GetFlags(u64 flags){
    return (value & flags) > 0 ? true : false;
}

// get address of this mapped page
u64 Page::GetAddress(){
    // physical address is 40 bits only
    return (value & PAGE_PHYSICAL_ADDRESS_MASK) >> 12;
}

// set address of this page
void Page::SetAddress(u64 address){
    // unset address first
    value &= ~PAGE_PHYSICAL_ADDRESS_MASK;
    // set address
    value |= (address << 12) & PAGE_PHYSICAL_ADDRESS_MASK;
}

// get next level of paging
PageTable* GetNextLevel(PageTable* ptable, u64 entry_index, bool allocate){
    Page* pte = &ptable->entries[entry_index];
    PageTable* pt = nullptr;

    // if page directory entry is not present and allocation is allowed, then allocate it
    if(!pte->GetFlags(MAP_PRESENT)){
        // if allocation isn't allowed then return nullptr
        if(!allocate){
            return nullptr;
        }

        // create page directory pointer
        u64 vaddr = AllocatePage();
        u64 paddr = VirtualToPhysicalAddress(vaddr);
        pt = reinterpret_cast<PageTable*>(vaddr);
        memset(reinterpret_cast<void*>(pt), 0, PAGE_SIZE);

        // shift by 12 biits to align it to 0x1000 boundary
        pte->SetAddress(paddr >> 12);
        pte->SetFlags(MAP_PRESENT | MAP_READ_WRITE);
    }else{
        u64 paddr = pte->GetAddress() << 12;
        u64 vaddr = PhysicalToVirtualAddress(paddr);
        pt = reinterpret_cast<PageTable*>(vaddr);
    }

    return pt;
}

// get's you a single page corresponding to the given virtual address:w
Page* GetPage(u64 vaddr, bool allocate){
    // cache this value
    u64 virtualAddr = vaddr;

    // get page index
    vaddr >>= 12;
    u64 pageIndex = vaddr & 0x1ff; // 0x1ff = 511 or 512 - 1

    // find table index
    vaddr >>= 9;
    u64 pml1Index = vaddr & 0x1ff;

    // find page directory index
    vaddr >>= 9;
    u64 pml2Index = vaddr & 0x1ff;

    // find page directory pointer index
    vaddr >>= 9;
    u64 pml3Index = vaddr & 0x1ff;

    // get page directory pointer from PML4
    PageTable* pml3 = GetNextLevel(mm.pml4, pml3Index, allocate);
    if(pml3 == nullptr){
        Printf("[-] PML3 for vaddr(%lx) doesn't exists or failed to allocate\n", virtualAddr);
        return nullptr;
    }

    // get page directory from page directory pointer
    PageTable* pml2 = GetNextLevel(pml3, pml2Index, allocate);
    if(pml2 == nullptr){
        Printf("[-] PML2 for vaddr(%lx) doesn't exists or failed to allocate\n", virtualAddr);
        return nullptr;
    }


    // get page table from page directory
    PageTable* pml1 = GetNextLevel(pml2, pml1Index, allocate);
    if(pml1 == nullptr){
        Printf("[-] PML1 for vaddr(%lx) doesn't exists or failed to allocate\n", virtualAddr);
        return nullptr;
    }


    // get page from page table
    Page *pte = &pml1->entries[pageIndex];
    if(pte == nullptr){
        Printf("[-] Page Table Entry for vaddr(%lx) doesn't exists or failed to allocate\n", virtualAddr);
        return nullptr;
    }

    return pte;
}

// map given physical memory to virtual memory wiht given flags
void MapMemory(u64 vaddr, u64 paddr, u64 flags){
    // get page table entry
    Page* pte = GetPage(vaddr, true);
    if(pte == nullptr) return;

    // map physical address 4kb aligned
    pte->SetAddress(paddr >> 12);
    pte->SetFlags(flags);
}

// create page map table by allocating a new array for it.
void CreatePageMap(){
    if(mm.pml4 == nullptr){
        // create new page map
        u64 pml4_vaddr = AllocatePage();
        mm.pml4_paddr = VirtualToPhysicalAddress(pml4_vaddr);
        mm.pml4 = reinterpret_cast<PageTable*>(pml4_vaddr);

        // and set all elements to 0
        memset(mm.pml4, 0, PAGE_SIZE);
    }else{
        Printf("[!] Attempt to recreate prexisting root level page map!\n");
    }}

// load page table in cr3 constrol register.
void LoadPageTable(){
    asm volatile("mov %0, %%cr3"
                 :
                 : "r" (mm.pml4_paddr)); // map takes
}

void InitializeVirtualMemoryManager(stivale2_struct_tag_memmap* mmap){
    // create's page table root entry
    CreatePageMap();

    stivale2_mmap_entry* mmap_entry = mmap->memmap;
    u64 mmap_count = mmap->entries;

    // map first 4 gb of memory to higher half
    // this memeory
    // for(u64 p = 0; p < 4*GB; p += PAGE_SIZE){
    //     MapMemory(MEM_PHYS_OFFSET + p, p, MAP_PRESENT | MAP_READ_WRITE);
    // }

    for(size_t i = 0; i < mmap_count; i++){
        if(mmap_entry[i].type != STIVALE2_MMAP_KERNEL_AND_MODULES){
            for(size_t p = 0; p < mmap_entry[i].length; p += PAGE_SIZE){
                u64 paddr = mmap_entry[i].base + p;
                u64 vaddr = PhysicalToVirtualAddress(paddr);
                MapMemory(vaddr, paddr, MAP_PRESENT | MAP_READ_WRITE);
            }
        }else{
            for(size_t p = 0; p < mmap_entry[i].length; p += PAGE_SIZE){
                u64 paddr = mmap_entry[i].base + p;
                u64 vaddr = KERNEL_VIRT_BASE + p;
                MapMemory(vaddr, paddr, MAP_PRESENT | MAP_READ_WRITE);
            }
        }
    }

    // u64 krnlPhysBase = BootInfo::GetKernelPhysicalBase();
    // for (uintptr_t p = 0; p < 2*GB; p += PAGE_SIZE){
    //     u64 paddr = krnlPhysBase + p;
    //     u64 vaddr = KERNEL_VIRT_BASE + p;
    //     MapMemory(vaddr, paddr, MAP_PRESENT | MAP_READ_WRITE);
    // }

    // load page table into cr3 register
    LoadPageTable();
}

// initialize memory manager
void InitializeMemoryManager(stivale2_struct_tag_memmap* mmap){
    InitializePhysicalMemoryManager(mmap);
    InitializeVirtualMemoryManager(mmap);
}
