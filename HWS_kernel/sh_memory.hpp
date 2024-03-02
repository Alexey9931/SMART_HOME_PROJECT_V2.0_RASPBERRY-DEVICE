#ifndef _SH_MEMORY_HPP
#define _SH_MEMORY_HPP

#define SHARED_MEMORY_NAME "/shmemory"

#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h> 
#include <sys/mman.h> 
#include <sys/types.h> 
#include "device_lib.hpp" 

typedef struct shMemory {
    Device device[5];
    uint8_t deviceCounter;
}__attribute__((packed)) shMemoryDef;

class SharedMemory {
    public:
        shMemoryDef shMemoryStruct;
        int shmFd;
        uint8_t openSharedMemory(bool isClient);
        void closeSharedMemory(void);
        uint8_t copyToSharedMemory(void);
        uint8_t copyFromSharedMemory(void);
};

#endif