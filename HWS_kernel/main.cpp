#include "main.hpp"

#define PORT 5151

extern SharedMemory sharedMemory;

int main()
{
    // Парсинг конфигурационного файла
    Parser configs_parser("configs.ini");
    configs_parser.getIsOpened();
    //NetMapBox net_map_box(configs_parser.parsed_configs);

    // Запуск основной задачи для опроса модулей
    std::thread devTask([&]() {
        devicesTask(configs_parser.parsed_configs.back().second);
    });
    
    devTask.join();
    return 0;
}

void devicesTask(std::list<std::pair<std::string, std::string>> devicesConfigs) {
    std::string thisDevIP;
    std::string devIP;
    std::vector <std::thread> devicesTaskThread;
    std::string delimeter = ".";

    for(auto deviceConfigs : devicesConfigs) {
         if (deviceConfigs.first == "THISDEVICE_IP") {
            thisDevIP = deviceConfigs.second;
         } else {
            devIP = deviceConfigs.second;
            // Запуск потока для опроса каждого модуля
            devicesTaskThread.push_back(std::thread(std::thread([&](){
                poolingDevice(thisDevIP, devIP);
            })));
         }
    }
    for (auto &thread : devicesTaskThread) {
        thread.join();
    }
}

void poolingDevice(std::string srcAddr, std::string devAddr) {
    sharedMemory.openSharedMemory(false);

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
        std::cout << "Socket creation error!" << std::endl;
    }

    setsockopt(sharedMemory.shMemoryStruct.device[deviceID].socket_fd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv));

    if (connect(sharedMemory.shMemoryStruct.device[deviceID].socket_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cout << "Error connection!" << std::endl;
    }

    while (true)
    {
        if (sharedMemory.shMemoryStruct.device[deviceID].typeCmd() != 1) {
            std::cout << "Type cmd is correct! Device's name is " << sharedMemory.shMemoryStruct.device[deviceID].devRegsSpace.deviceName << std::endl;
            sharedMemory.shMemoryStruct.device[deviceID].isInit = true;
        } else {
            std::cout << "Type cmd is failed!" << std::endl;
            sharedMemory.shMemoryStruct.device[deviceID].isInit = false;
            close(sharedMemory.shMemoryStruct.device[deviceID].socket_fd);
            break;
        }
        sharedMemory.copyToSharedMemory();
        
        sleep(1);
    }  
    close(sharedMemory.shMemoryStruct.device[deviceID].socket_fd);
    sharedMemory.closeSharedMemory();
}
