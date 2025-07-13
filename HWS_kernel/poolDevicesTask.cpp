#include "poolDevicesTask.hpp"

PoolDevicesTask::PoolDevicesTask(std::mutex *sharedMemoryMut, SharedMemory *sharedMemory) :
            Logger("smhomekernel", "/usr/local/sm_home/smhomekernel.log", "a"),
            Parser("/usr/local/sm_home/kernel_configs.ini"),
            sharedMemoryMut{sharedMemoryMut}, sharedMemory{sharedMemory} {}

PoolDevicesTask::~PoolDevicesTask() {
    task.join();
}

void PoolDevicesTask::runTask() {
    task = std::thread([&]() {
        devicesTask(Parser::parsed_configs.back().second);
    });
}

void PoolDevicesTask::devicesTask(std::list<std::pair<std::string, std::string>> devicesConfigs) {
    std::string thisDevIP;
    std::string devIP;
    std::vector <std::thread> devicesTaskThread;

    for(auto deviceConfigs : devicesConfigs) {
         if (deviceConfigs.first == "THISDEVICE_IP") {
            thisDevIP = deviceConfigs.second;
         } else {
            devIP = deviceConfigs.second;
            // Запуск потока для опроса каждого модуля
            Logger::systemlog(LOG_INFO, "[%s]: Start pooling this device!", devIP.c_str()); 
            devicesTaskThread.push_back(std::thread([this, thisDevIP, devIP](){
                poolDevice(thisDevIP, devIP);
            }));
         }
    }
    for (auto &thread : devicesTaskThread) {
        thread.join();
    }
}

void PoolDevicesTask::poolDevice(std::string srcAddr, std::string devAddr) {
    sharedMemoryMut->lock();
    uint8_t deviceID = sharedMemory->shMemoryStruct.deviceCounter;
    sharedMemory->shMemoryStruct.deviceCounter++;
    sharedMemoryMut->unlock();

    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(KERNEL_PORT);
    serv_addr.sin_addr.s_addr = inet_addr(devAddr.c_str()); 

    memcpy(sharedMemory->shMemoryStruct.device[deviceID].sourceIpAddr, srcAddr.c_str(), srcAddr.length());
    memcpy(sharedMemory->shMemoryStruct.device[deviceID].deviceIpAddr, devAddr.c_str(), devAddr.length());

    if ((sharedMemory->shMemoryStruct.device[deviceID].socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        Logger::systemlog(LOG_ERR, "[%s]: Socket creation error!", devAddr.c_str());
    }

    signal(SIGPIPE, SIG_IGN);

    struct timeval tv;
    tv.tv_sec = 22;

    if (connect(sharedMemory->shMemoryStruct.device[deviceID].socket_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        Logger::systemlog(LOG_ERR, "[%s]: Error connection!", devAddr.c_str());
    }

    setsockopt(sharedMemory->shMemoryStruct.device[deviceID].socket_fd, SOL_SOCKET, SO_SNDTIMEO, (const char*)&tv, sizeof(tv));
    setsockopt(sharedMemory->shMemoryStruct.device[deviceID].socket_fd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv));

    uint8_t cmdResult;
    while (true)
    {
        if (sharedMemory->shMemoryStruct.device[deviceID].isInit != true) {
            cmdResult = sharedMemory->shMemoryStruct.device[deviceID].typeCmd();
            switch (cmdResult) {
                case NO_ERROR:
                    sharedMemory->shMemoryStruct.device[deviceID].isInit = true;
                    sharedMemoryMut->lock();
                    if (sharedMemory->copyToSharedMemory()) {
                        Logger::systemlog(LOG_ERR, "[%s]: Error while copying data to shared memory!", devAddr.c_str()); 
                    }
                    sharedMemoryMut->unlock();
                    break;
                case TX_ERROR: case RX_ERROR:
                    Logger::systemlog(LOG_ERR, "[%s]: Type cmd is failed! Connection error! Attemp to reconnect... ", devAddr.c_str());
                    sharedMemory->shMemoryStruct.device[deviceID].isInit = false;
                    memset(&sharedMemory->shMemoryStruct.device[deviceID].deviceRegs, 0,
                        sizeof(sharedMemory->shMemoryStruct.device[deviceID].deviceRegs));
                    sharedMemoryMut->lock();
                    if (sharedMemory->copyToSharedMemory()) {
                        Logger::systemlog(LOG_ERR, "[%s]: Error while copying data to shared memory!", devAddr.c_str()); 
                    }
                    sharedMemoryMut->unlock();
                    if (close(sharedMemory->shMemoryStruct.device[deviceID].socket_fd)) {
                        Logger::systemlog(LOG_ERR, "[%s]: Error to close shared memory!", devAddr.c_str());
                    }
                    if ((sharedMemory->shMemoryStruct.device[deviceID].socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
                        Logger::systemlog(LOG_ERR, "[%s]: Socket creation error!", devAddr.c_str());
                    }
                    setsockopt(sharedMemory->shMemoryStruct.device[deviceID].socket_fd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv));
                    if (connect(sharedMemory->shMemoryStruct.device[deviceID].socket_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
                        Logger::systemlog(LOG_ERR, "[%s]: Error connection!", devAddr.c_str());
                    }
                    break;
                case PACK_ERROR:
                    Logger::systemlog(LOG_ERR, "[%s]: Type cmd is failed! Pack error!", devAddr.c_str());
                    sharedMemory->shMemoryStruct.device[deviceID].isInit = false;
                    memset(&sharedMemory->shMemoryStruct.device[deviceID].deviceRegs, 0,
                        sizeof(sharedMemory->shMemoryStruct.device[deviceID].deviceRegs));
                    sharedMemoryMut->lock();
                    if (sharedMemory->copyToSharedMemory()) {
                        Logger::systemlog(LOG_ERR, "[%s]: Error while copying data to shared memory!", devAddr.c_str()); 
                    }
                    sharedMemoryMut->unlock();
                    break;
            }   
        } else {
            sharedMemoryMut->lock();
            if (sharedMemory->copyFromSharedMemory()) {
                Logger::systemlog(LOG_ERR, "[%s]: Error while copying data from shared memory!", devAddr.c_str()); 
            }
            sharedMemoryMut->unlock();
            // записываем регистры, если есть, что записывать
            if (sharedMemory->shMemoryStruct.device[deviceID].isWriteLock == true) {
                if (strstr((const char*)sharedMemory->shMemoryStruct.device[deviceID].deviceName, CONTROL_PANEL_NAME) != NULL) {
                    cmdResult = sharedMemory->shMemoryStruct.device[deviceID].writeCmd
                        (0, (unsigned char*)&sharedMemory->shMemoryStruct.device[deviceID].deviceRegs, CONTROL_PANEL_REGS_SIZE);
                } else if (strstr((const char*)sharedMemory->shMemoryStruct.device[deviceID].deviceName, GAS_BOILER_CONTROLLER_NAME)!= NULL) {
                    cmdResult = sharedMemory->shMemoryStruct.device[deviceID].writeCmd
                    (0, (unsigned char*)&sharedMemory->shMemoryStruct.device[deviceID].deviceRegs, GAS_BOILER_CONTROLLER_REGS_SIZE);
                } else if (strstr((const char*)sharedMemory->shMemoryStruct.device[deviceID].deviceName, WEATHER_STATION_NAME)!= NULL) {
                    cmdResult = sharedMemory->shMemoryStruct.device[deviceID].writeCmd
                    (0, (unsigned char*)&sharedMemory->shMemoryStruct.device[deviceID].deviceRegs, WEATH_STATION_REGS_SIZE);
                }
                sharedMemory->shMemoryStruct.device[deviceID].isWriteLock = false;
                if (cmdResult != NO_ERROR) {
                    sharedMemory->shMemoryStruct.device[deviceID].isInit = false;
                    memset(&sharedMemory->shMemoryStruct.device[deviceID].deviceRegs, 0,
                            sizeof(sharedMemory->shMemoryStruct.device[deviceID].deviceRegs));
                    Logger::systemlog(LOG_ERR, "[%s]: Write cmd is failed!", devAddr.c_str());
                }
            }
            // читаем все регистры устройства
            if (strstr((const char*)sharedMemory->shMemoryStruct.device[deviceID].deviceName, CONTROL_PANEL_NAME) != NULL) {
                cmdResult = sharedMemory->shMemoryStruct.device[deviceID].readCmd(0, CONTROL_PANEL_REGS_SIZE, 
                    (unsigned char*)&sharedMemory->shMemoryStruct.device[deviceID].deviceRegs);
            } else if (strstr((const char*)sharedMemory->shMemoryStruct.device[deviceID].deviceName, GAS_BOILER_CONTROLLER_NAME)!= NULL) {
                cmdResult = sharedMemory->shMemoryStruct.device[deviceID].readCmd(0, GAS_BOILER_CONTROLLER_REGS_SIZE, 
                    (unsigned char*)&sharedMemory->shMemoryStruct.device[deviceID].deviceRegs);
            } else if (strstr((const char*)sharedMemory->shMemoryStruct.device[deviceID].deviceName, WEATHER_STATION_NAME)!= NULL) {
                cmdResult = sharedMemory->shMemoryStruct.device[deviceID].readCmd(0, WEATH_STATION_REGS_SIZE, 
                    (unsigned char*)&sharedMemory->shMemoryStruct.device[deviceID].deviceRegs);
            }

            if (cmdResult != NO_ERROR) {
                sharedMemory->shMemoryStruct.device[deviceID].isInit = false;
                memset(&sharedMemory->shMemoryStruct.device[deviceID].deviceRegs, 0,
                        sizeof(sharedMemory->shMemoryStruct.device[deviceID].deviceRegs));
                Logger::systemlog(LOG_ERR, "[%s]: Read cmd is failed!", devAddr.c_str());
            }
            
            sharedMemoryMut->lock();
            if (sharedMemory->copyToSharedMemory()) {
                Logger::systemlog(LOG_ERR, "[%s]: Error while copying data to shared memory!", devAddr.c_str()); 
            }
            sharedMemoryMut->unlock();
        } 
        sleep(1);
    }
    if (close(sharedMemory->shMemoryStruct.device[deviceID].socket_fd)) {
        Logger::systemlog(LOG_ERR, "[%s]: Error to close socket!", devAddr.c_str());
    }
}
