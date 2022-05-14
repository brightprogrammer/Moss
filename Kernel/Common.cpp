#include "Common.hpp"

void InfiniteHalt(){
    while(true){
        asm("hlt");
    }
}
