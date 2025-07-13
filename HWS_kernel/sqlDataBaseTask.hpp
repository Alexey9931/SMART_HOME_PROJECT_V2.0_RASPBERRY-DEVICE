#ifndef _SQL_DATABASE_TASK_HPP
#define _SQL_DATABASE_TASK_HPP

#include <thread>
#include <mutex>

#include "sh_memory.hpp"
#include "log.hpp"

#define MYSQL_SERVER_NAME   "localhost"
#define MYSQL_USER          "root"
#define MYSQL_PASSWD        "root"
#define MYSQL_DB_NAME       "SmartHomeProject"

class SqlDataBaseTask : public Logger {
    public:
        SqlDataBaseTask(std::mutex *sharedMemoryMut, SharedMemory *sharedMemory);
        ~SqlDataBaseTask();
        void runTask();
    private:
        void sqlDataBaseTask();
        std::mutex *sharedMemoryMut;
        SharedMemory *sharedMemory;
        std::thread task;
};

#endif
