#ifndef _POOL_DEVICES_TASK_HPP
#define _POOL_DEVICES_TASK_HPP

#include <thread>
#include <stdio.h>
#include <map>
#include <vector>
#include <list>
#include <iostream>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/socket.h>
#include <mutex>
#include <net/if.h>

#include "sh_memory.hpp"
#include "log.hpp"
#include "config_parser.hpp"

#define KERNEL_PORT 5151

class PoolDevicesTask : public Logger, Parser {
    public:
        PoolDevicesTask(std::mutex *sharedMemoryMut, SharedMemory *sharedMemory);
        ~PoolDevicesTask();
        void runTask();
    private:
        void devicesTask(std::list<std::pair<std::string, std::string>> devicesConfigs);
        void poolDevice(std::string srcAddr, std::string devAddr);
        std::mutex *sharedMemoryMut;
        SharedMemory *sharedMemory;
        std::thread task;
};

#endif
