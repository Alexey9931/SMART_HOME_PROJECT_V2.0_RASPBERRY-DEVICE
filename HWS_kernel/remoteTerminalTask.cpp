#include "remoteTerminalTask.hpp"

RemoteTerminalTask::RemoteTerminalTask(std::mutex *sharedMemoryMut, SharedMemory *sharedMemory) :
            Logger("smhomekernel", "/usr/local/sm_home/smhomekernel.log", "a"),
            sharedMemoryMut{sharedMemoryMut}, sharedMemory{sharedMemory} {}

RemoteTerminalTask::~RemoteTerminalTask() {
    task.join();
}

void RemoteTerminalTask::runTask() {
    task = std::thread([&]() {
        remoteTerminalTask();
    });
}

void RemoteTerminalTask::remoteTerminalTask() {
    int sock, listener;
    struct sockaddr_in addr;

    listener = socket(AF_INET, SOCK_STREAM, 0);
    if(listener < 0) {
        Logger::systemlog(LOG_ERR, "[%s]:Error to open socket!", __FUNCTION__);
        return;
    }

    struct timeval tv;
    tv.tv_sec = 5;
    tv.tv_usec = 0;
    if (setsockopt(listener, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
        Logger::systemlog(LOG_ERR, "[%s]:Error to set socket options!", __FUNCTION__);
    }
    if (setsockopt(listener, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv)) < 0) {
        Logger::systemlog(LOG_ERR, "[%s]:Error to set socket options!", __FUNCTION__);
    }
    
    addr.sin_family = AF_INET;
    addr.sin_port = htons(TERMINAL_PORT);
    sharedMemoryMut->lock();
    addr.sin_addr.s_addr = inet_addr(sharedMemory->shMemoryStruct.serverIP);
    sharedMemoryMut->unlock();
    
    bind(listener, (struct sockaddr *)&addr, sizeof(addr));

    if (listen(listener, 1) < 0) {
        Logger::systemlog(LOG_ERR, "[%s]:Error to listen!", __FUNCTION__);
        return;
    }
    
    Logger::systemlog(LOG_INFO, "[%s]:Server for remote Terminal has been succesfully started!", __FUNCTION__);

    while(1) {
        sock = accept(listener, NULL, NULL);
        if(sock < 0) {
            close(sock);
            continue;
        }
        Logger::systemlog(LOG_INFO, "[%s]:Remote Terminal client has been connected!Listening REQUESTS...", __FUNCTION__);
        while(1) {
            uint8_t *buf = (uint8_t*)malloc(sizeof("READ"));
            if (recv(sock, buf, sizeof("READ"), 0) != sizeof("READ")) {
                Logger::systemlog(LOG_INFO, "[%s]:Remote Terminal client has been disconnected!", __FUNCTION__);
                break;
            }
            free(buf);
            sharedMemoryMut->lock();
            if (sharedMemory->copyFromSharedMemory()) {
                Logger::systemlog(LOG_ERR, "[%s]: Error while copying data from shared memory!", __FUNCTION__); 
            }
            if (send(sock, &sharedMemory->shMemoryStruct, sizeof(sharedMemory->shMemoryStruct), 0)
                != sizeof(sharedMemory->shMemoryStruct)) {
                Logger::systemlog(LOG_INFO, "[%s]:Remote Terminal client has been disconnected!", __FUNCTION__);
                sharedMemoryMut->unlock();
                break;
            }
            sharedMemoryMut->unlock();
        }
        sleep(1);
    }

    close(sock);
    close(listener);
}
