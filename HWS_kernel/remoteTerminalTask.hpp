#ifndef _REMOTE_TERMINAL_TASK_HPP
#define _REMOTE_TERMINAL_TASK_HPP

#include <thread>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/socket.h>
#include <mutex>
#include <net/if.h>

#include "sh_memory.hpp"
#include "log.hpp"

#define TERMINAL_PORT 5153

class RemoteTerminalTask : public Logger {
    public:
        RemoteTerminalTask(std::mutex *sharedMemoryMut, SharedMemory *sharedMemory);
        ~RemoteTerminalTask();
        void runTask();
    private:
        void remoteTerminalTask();
        std::mutex *sharedMemoryMut;
        SharedMemory *sharedMemory;
        std::thread task;
};

#endif
