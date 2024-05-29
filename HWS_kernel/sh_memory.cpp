#include "sh_memory.hpp"

SharedMemory::SharedMemory(bool isClient) {
    if (isClient == false) {
        shm_unlink(SHARED_MEMORY_NAME);
        shMemSem = sem_open(SHMEM_SEM_NAME, O_CREAT, 0777, 1);
    } else {
        shMemSem = sem_open(SHMEM_SEM_NAME, 1);
    }
    shmFd = shm_open(SHARED_MEMORY_NAME, O_CREAT | O_RDWR, 0777);
}

SharedMemory::~SharedMemory() {
    close(shmFd);
    sem_close(shMemSem);
}

uint8_t SharedMemory::copyToSharedMemory(void) {
    sem_wait(shMemSem);
    ftruncate(shmFd, sizeof(shMemory));
    uint8_t *msg_ptr = (uint8_t*)mmap(0, sizeof(shMemory), PROT_READ | PROT_WRITE, MAP_SHARED, shmFd, 0);
    if (msg_ptr == (uint8_t*)-1 ) {
        return 1;
    }
    memcpy(msg_ptr, &shMemoryStruct, sizeof(shMemory));
    munmap(msg_ptr, sizeof(shMemory));
    sem_post(shMemSem);

    return 0;
}

uint8_t SharedMemory::copyFromSharedMemory(void) {
    sem_wait(shMemSem);
    ftruncate(shmFd, sizeof(shMemory));
    uint8_t *msg_ptr = (uint8_t*)mmap(0, sizeof(shMemory), PROT_READ | PROT_WRITE, MAP_SHARED, shmFd, 0);
    if (msg_ptr == (uint8_t*)-1 ) {
        return 1;
    }
    memcpy(&shMemoryStruct, msg_ptr, sizeof(shMemory));
    munmap(msg_ptr, sizeof(shMemory));
    sem_post(shMemSem);

    return 0;
}