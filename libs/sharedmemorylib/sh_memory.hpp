#ifndef _SH_MEMORY_HPP
#define _SH_MEMORY_HPP

#define SHARED_MEMORY_NAME "/shmemory"
#define SHMEM_SEM_NAME "/shared_memory_sem"

#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h> 
#include <sys/mman.h> 
#include <sys/types.h> 
#include "device_lib.hpp" 
#include "log.hpp" 
#include <cstdint>
#include <semaphore.h>
#include <fcntl.h>

typedef struct shMemory {
    Device device[5];
    uint8_t deviceCounter;
}__attribute__((packed)) shMemoryDef;

class SharedMemory {
    public:
        shMemoryDef shMemoryStruct;
        int shmFd;
        sem_t *shMemSem;
        SharedMemory(bool isClient);
        ~SharedMemory();
        uint8_t copyToSharedMemory(void);
        uint8_t copyFromSharedMemory(void);
};

#endif