#include "main.hpp"

#define PORT 5151

extern SharedMemory sharedMemory;

std::mutex poolDeviceMut;

int main()
{
    Logger log("smhomekernel", "/usr/local/sm_home/smhomekernel.log", "a");
    log.systemlog(LOG_INFO, "SmartHomeKernel has been succesfully started!");
    // Парсинг конфигурационного файла
    Parser configs_parser("/usr/local/sm_home/kernel_configs.ini");
    if (!configs_parser.getIsOpened()) {
        log.systemlog(LOG_ERR, "Unable to open config file!");
        return 1;
    }
    //NetMapBox net_map_box(configs_parser.parsed_configs);
    // Запуск основной задачи для опроса модулей
    std::thread devTask([&]() {
        devicesTask(log, configs_parser.parsed_configs.back().second);
    });
    
    devTask.join();
    return 0;
}

void devicesTask(Logger & log, std::list<std::pair<std::string, std::string>> devicesConfigs) {
    std::string thisDevIP;
    std::string devIP;
    std::vector <std::thread> devicesTaskThread;

    // Открываем разделяемую память
    if (sharedMemory.openSharedMemory(false)) {
        log.systemlog(LOG_ERR, "Error to open shared memory!");
    }

    for(auto deviceConfigs : devicesConfigs) {
         if (deviceConfigs.first == "THISDEVICE_IP") {
            thisDevIP = deviceConfigs.second;
         } else {
            devIP = deviceConfigs.second;
            // Запуск потока для опроса каждого модуля
            devicesTaskThread.push_back(std::thread([&log, thisDevIP, devIP](){
                poolingDevice(log, thisDevIP, devIP);
            }));
         }
    }
    for (auto &thread : devicesTaskThread) {
        thread.join();
    }

    // Закрываем разделяемую память
    if (sharedMemory.closeSharedMemory()) {
        log.systemlog(LOG_ERR, "Error to close shared memory!");
    }
}

void poolingDevice(Logger & log, std::string srcAddr, std::string devAddr) {
    poolDeviceMut.lock();
    uint8_t deviceID = sharedMemory.shMemoryStruct.deviceCounter;
    sharedMemory.shMemoryStruct.deviceCounter++;
    poolDeviceMut.unlock();

    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr.s_addr = inet_addr(devAddr.c_str()); 

    memcpy(sharedMemory.shMemoryStruct.device[deviceID].sourceIpAddr, srcAddr.c_str(), srcAddr.length());
    memcpy(sharedMemory.shMemoryStruct.device[deviceID].deviceIpAddr, devAddr.c_str(), devAddr.length());

    if ((sharedMemory.shMemoryStruct.device[deviceID].socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        log.systemlog(LOG_ERR, "[%s]: Socket creation error!", devAddr.c_str());
    }

    signal(SIGPIPE, SIG_IGN);

    struct timeval tv;
    tv.tv_sec = 11;

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
                    poolDeviceMut.lock();
                    if (sharedMemory.copyToSharedMemory()) {
                        log.systemlog(LOG_ERR, "[%s]: Error while copying data to shared memory!", devAddr.c_str()); 
                    }
                    poolDeviceMut.unlock();
                    break;
                case TX_ERROR: case RX_ERROR:
                    log.systemlog(LOG_ERR, "[%s]: Type cmd is failed! Connection error! Attemp to reconnect... ", devAddr.c_str());
                    sharedMemory.shMemoryStruct.device[deviceID].isInit = false;  
                    poolDeviceMut.lock();
                    if (sharedMemory.copyToSharedMemory()) {
                        log.systemlog(LOG_ERR, "[%s]: Error while copying data to shared memory!", devAddr.c_str()); 
                    }
                    poolDeviceMut.unlock();
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
                    poolDeviceMut.lock();
                    if (sharedMemory.copyToSharedMemory()) {
                        log.systemlog(LOG_ERR, "[%s]: Error while copying data to shared memory!", devAddr.c_str()); 
                    }
                    poolDeviceMut.unlock();
                    break;
            }   
        } else {
            // читаем все регистры устройства
            if (strstr((const char*)sharedMemory.shMemoryStruct.device[deviceID].deviceName, CONTROL_PANEL_NAME) != NULL) {
                cmdResult = sharedMemory.shMemoryStruct.device[deviceID].readCmd(0, CONTROL_PANEL_REGS_SIZE, 
                (unsigned char*)&sharedMemory.shMemoryStruct.device[deviceID].deviceRegs);
            }
            else if (strstr((const char*)sharedMemory.shMemoryStruct.device[deviceID].deviceName, GAS_BOILER_CONTROLLER_NAME)!= NULL) {
                cmdResult = sharedMemory.shMemoryStruct.device[deviceID].readCmd(0, GAS_BOILER_CONTROLLER_REGS_SIZE, 
                (unsigned char*)&sharedMemory.shMemoryStruct.device[deviceID].deviceRegs);
            }
            else if (strstr((const char*)sharedMemory.shMemoryStruct.device[deviceID].deviceName, WEATHER_STATION_NAME)!= NULL) {
                cmdResult = sharedMemory.shMemoryStruct.device[deviceID].readCmd(0, WEATH_STATION_REGS_SIZE, 
                (unsigned char*)&sharedMemory.shMemoryStruct.device[deviceID].deviceRegs);
            }

            if (cmdResult == NO_ERROR) {
                poolDeviceMut.lock();
                if (sharedMemory.copyToSharedMemory()) {
                    log.systemlog(LOG_ERR, "[%s]: Error while copying data to shared memory!", devAddr.c_str()); 
                }
                poolDeviceMut.unlock();
            } else {
                log.systemlog(LOG_ERR, "[%s]: Read cmd is failed!", devAddr.c_str());
            }
        } 
        sleep(1);
    }  
    if (close(sharedMemory.shMemoryStruct.device[deviceID].socket_fd)) {
        log.systemlog(LOG_ERR, "[%s]: Error to close socket!", devAddr.c_str());
    }
}
