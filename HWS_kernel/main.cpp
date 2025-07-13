#include "poolDevicesTask.hpp"
#include "remoteTerminalTask.hpp"
#include "sqlDataBaseTask.hpp"

#include "sh_memory.hpp"
#include "log.hpp"

#include <sys/socket.h>
#include <sys/ioctl.h>

SharedMemory sharedMemory(false);
std::mutex sharedMemoryMut;

void findServerIp(char *ip_address);

int main()
{
    Logger log("smhomekernel", "/usr/local/sm_home/smhomekernel.log", "a");
    log.systemlog(LOG_INFO, "SmartHomeKernel has been succesfully started!");

    findServerIp(sharedMemory.shMemoryStruct.serverIP);
    if (sharedMemory.copyToSharedMemory()) {
        log.systemlog(LOG_ERR, "Error while copying data to shared memory!"); 
    }

    // Парсинг конфигурационного файла
    Parser configs_parser("/usr/local/sm_home/kernel_configs.ini");
    if (!configs_parser.getIsOpened()) {
        log.systemlog(LOG_ERR, "Unable to open config file!");
        return 1;
    }

    // Проверка того, как открылась разделяемая память
    if ((sharedMemory.shMemSem == SEM_FAILED) || (sharedMemory.shmFd < 0)) {
        log.systemlog(LOG_ERR, "Error while start to work with shared_memory!");
        return 1;
    }

    // Запуск основной задачи для опроса модулей
    PoolDevicesTask devicesTask(&sharedMemoryMut, &sharedMemory);
    devicesTask.runTask();

    // Запуск задачи - сервера для терминала
    RemoteTerminalTask remoteTerminalTask(&sharedMemoryMut, &sharedMemory);
    remoteTerminalTask.runTask();

    // Запуск задачи отправки данных в БД MYSQL
    SqlDataBaseTask sqlDataBaseTask(&sharedMemoryMut, &sharedMemory);
    sqlDataBaseTask.runTask();

    return 0;
}

void findServerIp(char *ip_address) {
    int fd;
    struct ifreq ifr;

    /*AF_INET - to define network interface IPv4*/
    /*Creating soket for it.*/
    fd = socket(AF_INET, SOCK_DGRAM, 0);

    /*AF_INET - to define IPv4 Address type.*/
    ifr.ifr_addr.sa_family = AF_INET;

    /*eth0 - define the ifr_name - port name
    where network attached.*/
    memcpy(ifr.ifr_name, "eth0", IFNAMSIZ - 1);

    /*Accessing network interface information by
    passing address using ioctl.*/
    ioctl(fd, SIOCGIFADDR, &ifr);
    /*closing fd*/
    close(fd);

    /*Extract IP Address*/
    strcpy((char*)ip_address, inet_ntoa(((struct sockaddr_in*)&ifr.ifr_addr)->sin_addr));
}
