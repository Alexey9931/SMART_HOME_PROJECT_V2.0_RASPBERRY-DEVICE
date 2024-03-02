#include "sh_memory.hpp"

SharedMemory sharedMemory;

uint8_t SharedMemory::openSharedMemory(bool isClient) {
    if (isClient == false) {
        shm_unlink(SHARED_MEMORY_NAME);
    }
    shmFd = shm_open(SHARED_MEMORY_NAME, O_CREAT | O_RDWR, 0777);
    if (shmFd < 0) {
        return 1;
    }
    return 0;
}

void SharedMemory::closeSharedMemory(void) {
    close(shmFd);
}

uint8_t SharedMemory::copyToSharedMemory(void) {
    ftruncate(shmFd, sizeof(shMemory));
    uint8_t *msg_ptr = (uint8_t*)mmap(0, sizeof(shMemory), PROT_READ | PROT_WRITE, MAP_SHARED, shmFd, 0);
    if (msg_ptr == (uint8_t*)-1 ) {
        return 1;
    }
    memcpy(msg_ptr, &shMemoryStruct, sizeof(shMemory));
    munmap(msg_ptr, sizeof(shMemory));

    return 0;
}

uint8_t SharedMemory::copyFromSharedMemory(void) {
    ftruncate(shmFd, sizeof(shMemory));
    uint8_t *msg_ptr = (uint8_t*)mmap(0, sizeof(shMemory), PROT_READ | PROT_WRITE, MAP_SHARED, shmFd, 0);
    if (msg_ptr == (uint8_t*)-1 ) {
        return 1;
    }
    memcpy(&shMemoryStruct, msg_ptr, sizeof(shMemory));
    munmap(msg_ptr, sizeof(shMemory));

    return 0;
}