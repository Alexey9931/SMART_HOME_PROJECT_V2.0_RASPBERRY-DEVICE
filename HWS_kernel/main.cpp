#include "main.hpp"

#define KERNEL_PORT 5151
#define TERMINAL_PORT 5153

SharedMemory sharedMemory(false);

std::mutex sharedMemoryMut;

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
    std::thread devTask([&]() {
        devicesTask(log, configs_parser.parsed_configs.back().second);
    });

    // Запуск задачи - сервера для терминала
    std::thread terminalTask([&]() {
        terminalRemoteTask(log);
    });

    devTask.join();
    terminalTask.join();

    return 0;
}

void devicesTask(Logger & log, std::list<std::pair<std::string, std::string>> devicesConfigs) {
    std::string thisDevIP;
    std::string devIP;
    std::vector <std::thread> devicesTaskThread;

    for(auto deviceConfigs : devicesConfigs) {
         if (deviceConfigs.first == "THISDEVICE_IP") {
            thisDevIP = deviceConfigs.second;
         } else {
            devIP = deviceConfigs.second;
            // Запуск потока для опроса каждого модуля
            log.systemlog(LOG_INFO, "[%s]:[%s]: Start pooling this device!", __FUNCTION__, devIP.c_str()); 
            devicesTaskThread.push_back(std::thread([&log, thisDevIP, devIP](){
                poolingDevice(log, thisDevIP, devIP);
            }));
         }
    }
    for (auto &thread : devicesTaskThread) {
        thread.join();
    }
}

void poolingDevice(Logger & log, std::string srcAddr, std::string devAddr) {
    sharedMemoryMut.lock();
    uint8_t deviceID = sharedMemory.shMemoryStruct.deviceCounter;
    sharedMemory.shMemoryStruct.deviceCounter++;
    sharedMemoryMut.unlock();

    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(KERNEL_PORT);
    serv_addr.sin_addr.s_addr = inet_addr(devAddr.c_str()); 

    memcpy(sharedMemory.shMemoryStruct.device[deviceID].sourceIpAddr, srcAddr.c_str(), srcAddr.length());
    memcpy(sharedMemory.shMemoryStruct.device[deviceID].deviceIpAddr, devAddr.c_str(), devAddr.length());

    if ((sharedMemory.shMemoryStruct.device[deviceID].socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        log.systemlog(LOG_ERR, "[%s]: Socket creation error!", devAddr.c_str());
    }

    signal(SIGPIPE, SIG_IGN);

    struct timeval tv;
    tv.tv_sec = 22;

    if (connect(sharedMemory.shMemoryStruct.device[deviceID].socket_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        log.systemlog(LOG_ERR, "[%s]: Error connection!", devAddr.c_str());
    }

    setsockopt(sharedMemory.shMemoryStruct.device[deviceID].socket_fd, SOL_SOCKET, SO_SNDTIMEO, (const char*)&tv, sizeof(tv));
    setsockopt(sharedMemory.shMemoryStruct.device[deviceID].socket_fd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv));

    uint8_t cmdResult;
    while (true)
    {
        if (sharedMemory.shMemoryStruct.device[deviceID].isInit != true) {
            cmdResult = sharedMemory.shMemoryStruct.device[deviceID].typeCmd();
            switch (cmdResult) {
                case NO_ERROR:
                    sharedMemory.shMemoryStruct.device[deviceID].isInit = true;
                    sharedMemoryMut.lock();
                    if (sharedMemory.copyToSharedMemory()) {
                        log.systemlog(LOG_ERR, "[%s]: Error while copying data to shared memory!", devAddr.c_str()); 
                    }
                    sharedMemoryMut.unlock();
                    break;
                case TX_ERROR: case RX_ERROR:
                    log.systemlog(LOG_ERR, "[%s]: Type cmd is failed! Connection error! Attemp to reconnect... ", devAddr.c_str());
                    sharedMemory.shMemoryStruct.device[deviceID].isInit = false;
                    memset(&sharedMemory.shMemoryStruct.device[deviceID].deviceRegs, 0,
                        sizeof(sharedMemory.shMemoryStruct.device[deviceID].deviceRegs));
                    sharedMemoryMut.lock();
                    if (sharedMemory.copyToSharedMemory()) {
                        log.systemlog(LOG_ERR, "[%s]: Error while copying data to shared memory!", devAddr.c_str()); 
                    }
                    sharedMemoryMut.unlock();
                    if (close(sharedMemory.shMemoryStruct.device[deviceID].socket_fd)) {
                        log.systemlog(LOG_ERR, "[%s]: Error to close shared memory!", devAddr.c_str());
                    }
                    if ((sharedMemory.shMemoryStruct.device[deviceID].socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
                        log.systemlog(LOG_ERR, "[%s]: Socket creation error!", devAddr.c_str());
                    }
                    setsockopt(sharedMemory.shMemoryStruct.device[deviceID].socket_fd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv));
                    if (connect(sharedMemory.shMemoryStruct.device[deviceID].socket_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
                        log.systemlog(LOG_ERR, "[%s]: Error connection!", devAddr.c_str());
                    }
                    break;
                case PACK_ERROR:
                    log.systemlog(LOG_ERR, "[%s]: Type cmd is failed! Pack error!", devAddr.c_str());
                    sharedMemory.shMemoryStruct.device[deviceID].isInit = false;
                    memset(&sharedMemory.shMemoryStruct.device[deviceID].deviceRegs, 0,
                        sizeof(sharedMemory.shMemoryStruct.device[deviceID].deviceRegs));
                    sharedMemoryMut.lock();
                    if (sharedMemory.copyToSharedMemory()) {
                        log.systemlog(LOG_ERR, "[%s]: Error while copying data to shared memory!", devAddr.c_str()); 
                    }
                    sharedMemoryMut.unlock();
                    break;
            }   
        } else {
            sharedMemoryMut.lock();
            if (sharedMemory.copyFromSharedMemory()) {
                log.systemlog(LOG_ERR, "[%s]: Error while copying data from shared memory!", devAddr.c_str()); 
            }
            sharedMemoryMut.unlock();
            // записываем регистры, если есть, что записывать
            if (sharedMemory.shMemoryStruct.device[deviceID].isWriteLock == true) {
                if (strstr((const char*)sharedMemory.shMemoryStruct.device[deviceID].deviceName, CONTROL_PANEL_NAME) != NULL) {
                    cmdResult = sharedMemory.shMemoryStruct.device[deviceID].writeCmd
                        (0, (unsigned char*)&sharedMemory.shMemoryStruct.device[deviceID].deviceRegs, CONTROL_PANEL_REGS_SIZE);
                } else if (strstr((const char*)sharedMemory.shMemoryStruct.device[deviceID].deviceName, GAS_BOILER_CONTROLLER_NAME)!= NULL) {
                    cmdResult = sharedMemory.shMemoryStruct.device[deviceID].writeCmd
                    (0, (unsigned char*)&sharedMemory.shMemoryStruct.device[deviceID].deviceRegs, GAS_BOILER_CONTROLLER_REGS_SIZE);
                } else if (strstr((const char*)sharedMemory.shMemoryStruct.device[deviceID].deviceName, WEATHER_STATION_NAME)!= NULL) {
                    cmdResult = sharedMemory.shMemoryStruct.device[deviceID].writeCmd
                    (0, (unsigned char*)&sharedMemory.shMemoryStruct.device[deviceID].deviceRegs, WEATH_STATION_REGS_SIZE);
                }
                sharedMemory.shMemoryStruct.device[deviceID].isWriteLock = false;
                if (cmdResult != NO_ERROR) {
                    sharedMemory.shMemoryStruct.device[deviceID].isInit = false;
                    memset(&sharedMemory.shMemoryStruct.device[deviceID].deviceRegs, 0,
                            sizeof(sharedMemory.shMemoryStruct.device[deviceID].deviceRegs));
                    log.systemlog(LOG_ERR, "[%s]: Write cmd is failed!", devAddr.c_str());
                }
            }
            // читаем все регистры устройства
            if (strstr((const char*)sharedMemory.shMemoryStruct.device[deviceID].deviceName, CONTROL_PANEL_NAME) != NULL) {
                cmdResult = sharedMemory.shMemoryStruct.device[deviceID].readCmd(0, CONTROL_PANEL_REGS_SIZE, 
                    (unsigned char*)&sharedMemory.shMemoryStruct.device[deviceID].deviceRegs);
            } else if (strstr((const char*)sharedMemory.shMemoryStruct.device[deviceID].deviceName, GAS_BOILER_CONTROLLER_NAME)!= NULL) {
                cmdResult = sharedMemory.shMemoryStruct.device[deviceID].readCmd(0, GAS_BOILER_CONTROLLER_REGS_SIZE, 
                    (unsigned char*)&sharedMemory.shMemoryStruct.device[deviceID].deviceRegs);
            } else if (strstr((const char*)sharedMemory.shMemoryStruct.device[deviceID].deviceName, WEATHER_STATION_NAME)!= NULL) {
                cmdResult = sharedMemory.shMemoryStruct.device[deviceID].readCmd(0, WEATH_STATION_REGS_SIZE, 
                    (unsigned char*)&sharedMemory.shMemoryStruct.device[deviceID].deviceRegs);
            }

            if (cmdResult != NO_ERROR) {
                sharedMemory.shMemoryStruct.device[deviceID].isInit = false;
                memset(&sharedMemory.shMemoryStruct.device[deviceID].deviceRegs, 0,
                        sizeof(sharedMemory.shMemoryStruct.device[deviceID].deviceRegs));
                log.systemlog(LOG_ERR, "[%s]: Read cmd is failed!", devAddr.c_str());
            }
            
            sharedMemoryMut.lock();
            if (sharedMemory.copyToSharedMemory()) {
                log.systemlog(LOG_ERR, "[%s]: Error while copying data to shared memory!", devAddr.c_str()); 
            }
            sharedMemoryMut.unlock();
        } 
        sleep(1);
    }
    if (close(sharedMemory.shMemoryStruct.device[deviceID].socket_fd)) {
        log.systemlog(LOG_ERR, "[%s]: Error to close socket!", devAddr.c_str());
    }
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

void terminalRemoteTask(Logger &log) {
    int sock, listener;
    struct sockaddr_in addr;

    listener = socket(AF_INET, SOCK_STREAM, 0);
    if(listener < 0) {
        log.systemlog(LOG_ERR, "[%s]:Error to open socket!", __FUNCTION__);
        return;
    }
    signal(SIGPIPE, SIG_IGN);

    struct timeval rxtimeout;
    rxtimeout.tv_sec = 5;
    rxtimeout.tv_usec = 0;
    if (setsockopt(listener, SOL_SOCKET, SO_RCVTIMEO, &rxtimeout, sizeof(rxtimeout)) < 0) {
        log.systemlog(LOG_ERR, "[%s]:Error to set socket options!", __FUNCTION__);
    }
    
    addr.sin_family = AF_INET;
    addr.sin_port = htons(TERMINAL_PORT);
    sharedMemoryMut.lock();
    addr.sin_addr.s_addr = inet_addr(sharedMemory.shMemoryStruct.serverIP);
    sharedMemoryMut.unlock();
    
    bind(listener, (struct sockaddr *)&addr, sizeof(addr));

    if (listen(listener, 1) < 0) {
        log.systemlog(LOG_ERR, "[%s]:Error to listen!", __FUNCTION__);
        return;
    }
    
    log.systemlog(LOG_INFO, "[%s]:Server for remote Terminal has been succesfully started!", __FUNCTION__);

    while(1) {
        sock = accept(listener, NULL, NULL);
        if(sock < 0) {
            close(sock);
            continue;
        }
        log.systemlog(LOG_INFO, "[%s]:Remote Terminal client has been connected!Listening REQUESTS...", __FUNCTION__);
        while(1) {
            uint8_t *buf = (uint8_t*)malloc(sizeof("READ"));
            if (recv(sock, buf, sizeof("READ"), 0) != sizeof("READ")) {
                log.systemlog(LOG_INFO, "[%s]:Remote Terminal client has been disconnected!", __FUNCTION__);
                break;
            }
            free(buf);
            sharedMemoryMut.lock();
            if (sharedMemory.copyFromSharedMemory()) {
                log.systemlog(LOG_ERR, "[%s]: Error while copying data from shared memory!", __FUNCTION__); 
            }
            if (send(sock, &sharedMemory.shMemoryStruct, sizeof(sharedMemory.shMemoryStruct), 0)
                != sizeof(sharedMemory.shMemoryStruct)) {
                log.systemlog(LOG_INFO, "[%s]:Remote Terminal client has been disconnected!", __FUNCTION__);
                sharedMemoryMut.unlock();
                break;
            }
            sharedMemoryMut.unlock();
        }
        sleep(1);
    }

    close(sock);
    close(listener);
}

