#include "main.hpp"

#define PORT 5151

extern SharedMemory sharedMemory;

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

    for(auto deviceConfigs : devicesConfigs) {
         if (deviceConfigs.first == "THISDEVICE_IP") {
            thisDevIP = deviceConfigs.second;
         } else {
            devIP = deviceConfigs.second;
            // Запуск потока для опроса каждого модуля
            devicesTaskThread.push_back(std::thread(std::thread([&](){
                poolingDevice(log, thisDevIP, devIP);
            })));
         }
    }
    for (auto &thread : devicesTaskThread) {
        thread.join();
    }
}

void poolingDevice(Logger & log, std::string srcAddr, std::string devAddr) {
    if (sharedMemory.openSharedMemory(false)) {
        log.systemlog(LOG_ERR, "Error to open shared memory!");
    }

    uint8_t deviceID = sharedMemory.shMemoryStruct.deviceCounter;
    sharedMemory.shMemoryStruct.deviceCounter++;

    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr.s_addr = inet_addr(devAddr.c_str()); 

    memcpy(sharedMemory.shMemoryStruct.device[deviceID].sourceIpAddr, srcAddr.c_str(), srcAddr.length());
    memcpy(sharedMemory.shMemoryStruct.device[deviceID].deviceIpAddr, devAddr.c_str(), devAddr.length());

    struct timeval tv;
    tv.tv_sec = 5;

    if ((sharedMemory.shMemoryStruct.device[deviceID].socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        log.systemlog(LOG_ERR, "Socket creation error!");
    }

    setsockopt(sharedMemory.shMemoryStruct.device[deviceID].socket_fd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv));

    if (connect(sharedMemory.shMemoryStruct.device[deviceID].socket_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        log.systemlog(LOG_ERR, "Error connection!");
    }

    while (true)
    {
        uint8_t cmdResult = sharedMemory.shMemoryStruct.device[deviceID].typeCmd();
        switch (cmdResult) {
            case NO_ERROR:
                sharedMemory.shMemoryStruct.device[deviceID].isInit = true;
                if (sharedMemory.copyToSharedMemory()) {
                   log.systemlog(LOG_ERR, "Error while copying data to shared memory!"); 
                }
                break;
            case TX_ERROR: case RX_ERROR:
                log.systemlog(LOG_ERR, "Type cmd is failed! Connection error! Attemp to reconnect... ");
                sharedMemory.shMemoryStruct.device[deviceID].isInit = false;  
                if (sharedMemory.copyToSharedMemory()) {
                   log.systemlog(LOG_ERR, "Error while copying data to shared memory!"); 
                }
                if (close(sharedMemory.shMemoryStruct.device[deviceID].socket_fd)) {
                    log.systemlog(LOG_ERR, "Error to close shared memory!");
                }
                if ((sharedMemory.shMemoryStruct.device[deviceID].socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
                    log.systemlog(LOG_ERR, "Socket creation error!");
                }
                setsockopt(sharedMemory.shMemoryStruct.device[deviceID].socket_fd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv));
                if (connect(sharedMemory.shMemoryStruct.device[deviceID].socket_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
                    log.systemlog(LOG_ERR, "Error connection!");
                }             
                break;
            case PACK_ERROR:
                log.systemlog(LOG_ERR, "Type cmd is failed! Pack error!");
                sharedMemory.shMemoryStruct.device[deviceID].isInit = false;
                if (sharedMemory.copyToSharedMemory()) {
                   log.systemlog(LOG_ERR, "Error while copying data to shared memory!"); 
                }
                break;
        }      
        sleep(1);
    }  
    if (close(sharedMemory.shMemoryStruct.device[deviceID].socket_fd)) {
        log.systemlog(LOG_ERR, "Error to close socket!");
    }
    if (sharedMemory.closeSharedMemory()) {
        log.systemlog(LOG_ERR, "Error to close shared memory!");
    }
}
