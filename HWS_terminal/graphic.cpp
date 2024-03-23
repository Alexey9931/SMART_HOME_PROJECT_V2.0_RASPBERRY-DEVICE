#include "graphic.hpp"
#include "sh_memory.hpp"

extern SharedMemory sharedMemory;

TerminalGraphic::TerminalGraphic() {

}

std::string TerminalGraphic::convertIntToHex(int toConvert) {
    std::string result;
    std::stringstream ss;
    ss << std::hex << toConvert;
    ss >> result;
    
    return result;
}

void TerminalGraphic::printMainMenu(Logger &log) {
    findServerIp();
    WINDOW *menuWindow = newwin(11, 27, 1, 2);
    box(menuWindow, 0, 0);
    refresh();
    mvwprintw(menuWindow, 0, 5, "УСТРОЙСТВА В СЕТИ");
    keypad(menuWindow, true);
    for (uint8_t i = 0; i < sizeof(sharedMemory.shMemoryStruct)/sizeof(sharedMemory.shMemoryStruct.device[0]); i++) {
       mvwprintw(menuWindow, 1 + i, 1, ("-" + std::to_string(i+1) + "- " + stringsMainMenu[i]).c_str()); 
    }
    wrefresh(menuWindow);
    int highlight = 0;
    std::thread buttonHandler([&]() {
        buttonListener(menuWindow, highlight);
    });
    while (1) {
        if (sharedMemory.copyFromSharedMemory()) {
            log.systemlog(LOG_ERR, "Error to copy data from shared memory!");
        }
        for (uint8_t i = 0; i < sizeof(sharedMemory.shMemoryStruct)/sizeof(sharedMemory.shMemoryStruct.device[0]); i++) {
            if (i == highlight) {
                wattron(menuWindow, A_STANDOUT);
            }
            if (sharedMemory.shMemoryStruct.device[i].isInit == true) {
                stringsMainMenu[i].assign((char*)sharedMemory.shMemoryStruct.device[i].devRegsSpace.deviceName, sizeof(sharedMemory.shMemoryStruct.device[i].devRegsSpace.deviceName));
            } else {                
                stringsMainMenu[i] = "ПУСТО        ";
                memset(&sharedMemory.shMemoryStruct.device[i].devRegsSpace, 0, sizeof(sharedMemory.shMemoryStruct.device[i].devRegsSpace));
                if (sharedMemory.copyToSharedMemory()) {
                    log.systemlog(LOG_ERR, "Error to copy data to shared memory!");
                }
            }
            mvwprintw(menuWindow, 1 + i, 1, ("-" + std::to_string(i+1) + "- " + stringsMainMenu[i]).c_str()); 
            wattroff(menuWindow, A_STANDOUT);
            wrefresh(menuWindow);
        } 
        printDeviceInfoWindow(sharedMemory.shMemoryStruct.device[highlight]);
        printDeviceDataWindow(sharedMemory.shMemoryStruct.device[highlight]);   
        printServerInfoWindow();
        usleep(100000);
    } 
    buttonHandler.join();  
}

void TerminalGraphic::buttonListener(WINDOW *window, int &highlight) {
    while(1) {
        switch (wgetch(window)) {
            case KEY_DOWN:
                highlight++;
                if (highlight == 5)
                    highlight = 4;
                break;
            case KEY_UP:
                highlight--;
                if (highlight == -1)
                    highlight = 0;
                break;
            // case 10:    /*Enter*/
            //     printDeviceInfoWindow(sharedMemory.shMemoryStruct.device[highlight]);
            //     printDeviceDataWindow(sharedMemory.shMemoryStruct.device[highlight]);
            //     break;
        } 
    }     
}

void TerminalGraphic::printDeviceInfoWindow(Device device) {
    WINDOW *deviceInfoWindow = newwin(11, 40, 1, 30);
    box(deviceInfoWindow, 0, 0);
    refresh();
    mvwprintw(deviceInfoWindow, 0, 2, "ИНФОРМАЦИЯ ОБ УСТРОЙСТВЕ");
    mvwprintw(deviceInfoWindow, 1, 1, "Описание: ");
    if(strstr((char*)device.devRegsSpace.deviceName, "Control Panel")) {
        mvwprintw(deviceInfoWindow, 1, 15, "Панель управления");
        mvwprintw(deviceInfoWindow, 2, 15, "системой \"УМНЫЙ ДОМ\"");
    } else {

    }
    mvwprintw(deviceInfoWindow, 3, 1, "IP адрес у-ва: ");
    mvwprintw(deviceInfoWindow, 3, 20, 
        (std::to_string(device.devRegsSpace.ipAddr1[0]) + "." + 
        std::to_string(device.devRegsSpace.ipAddr1[1]) + "." + 
        std::to_string(device.devRegsSpace.ipAddr1[2]) + "." +
        std::to_string(device.devRegsSpace.ipAddr1[3])).c_str());
    mvwprintw(deviceInfoWindow, 4, 1, "MAC адрес у-ва: ");
    mvwprintw(deviceInfoWindow, 4, 20, 
        (convertIntToHex(device.devRegsSpace.macAddr1[0]) + "." + 
        convertIntToHex(device.devRegsSpace.macAddr1[1]) + "." + 
        convertIntToHex(device.devRegsSpace.macAddr1[2]) + "." +
        convertIntToHex(device.devRegsSpace.macAddr1[3]) + "." +
        convertIntToHex(device.devRegsSpace.macAddr1[4]) + "." +
        convertIntToHex(device.devRegsSpace.macAddr1[5])).c_str());
    mvwprintw(deviceInfoWindow, 5, 1, "IP адрес маршр-ра: ");
    mvwprintw(deviceInfoWindow, 5, 20, 
        (std::to_string(device.devRegsSpace.ipGate[0]) + "." + 
        std::to_string(device.devRegsSpace.ipGate[1]) + "." + 
        std::to_string(device.devRegsSpace.ipGate[2]) + "." +
        std::to_string(device.devRegsSpace.ipGate[3])).c_str());
    mvwprintw(deviceInfoWindow, 6, 1, "Маска подсети: ");
    mvwprintw(deviceInfoWindow, 6, 20, 
        (std::to_string(device.devRegsSpace.ipMask[0]) + "." + 
        std::to_string(device.devRegsSpace.ipMask[1]) + "." + 
        std::to_string(device.devRegsSpace.ipMask[2]) + "." +
        std::to_string(device.devRegsSpace.ipMask[3])).c_str());
    mvwprintw(deviceInfoWindow, 7, 1, "Порт соединения: ");
    mvwprintw(deviceInfoWindow, 7, 20, std::to_string(device.devRegsSpace.localPort).c_str());
    mvwprintw(deviceInfoWindow, 8, 1, "Кол-во отправленных пакетов: ");
    mvwprintw(deviceInfoWindow, 8, 30, std::to_string(device.devRegsSpace.numTxPack).c_str());
    mvwprintw(deviceInfoWindow, 9, 1, "Кол-во принятых пакетов: ");
    mvwprintw(deviceInfoWindow, 9, 30, std::to_string(device.devRegsSpace.numRxPack).c_str());
    wrefresh(deviceInfoWindow);
}

void TerminalGraphic::printDeviceDataWindow(Device device) {
    WINDOW *deviceInfoWindow = newwin(6, 40, 12, 30);
    box(deviceInfoWindow, 0, 0);
    refresh();
    mvwprintw(deviceInfoWindow, 0, 2, "ДАННЫЕ УСТРОЙСТВА");
    mvwprintw(deviceInfoWindow, 1, 1, "Системное время:");
    char date[20];
    sprintf(date, "%02d:%02d:%02d %02d/%02d/20%02d", device.devRegsSpace.sysTime.hour, device.devRegsSpace.sysTime.minutes, 
        device.devRegsSpace.sysTime.seconds, device.devRegsSpace.sysTime.dayOfMonth, device.devRegsSpace.sysTime.month, 
        device.devRegsSpace.sysTime.year);
    mvwprintw(deviceInfoWindow, 1, 18, date);
    mvwprintw(deviceInfoWindow, 2, 1, "Температура:");
    mvwprintw(deviceInfoWindow, 2, 15, (std::to_string(device.devRegsSpace.temperature) + "°C").c_str());
    mvwprintw(deviceInfoWindow, 3, 1, "Влажность:");
    mvwprintw(deviceInfoWindow, 3, 15, (std::to_string(device.devRegsSpace.humidity) + "%%").c_str());
    mvwprintw(deviceInfoWindow, 4, 1, "Атм.давление:");
    mvwprintw(deviceInfoWindow, 4, 15, (std::to_string(device.devRegsSpace.pressure) + "мм.рт.ст.").c_str());

    wrefresh(deviceInfoWindow);
}

void TerminalGraphic::printServerInfoWindow(void) {
    char date[20];
    time_t rawtime;
    struct tm * timeinfo;
    WINDOW *serverInfoWindow = newwin(6, 27, 12, 2);
    box(serverInfoWindow, 0, 0);
    refresh();
    mvwprintw(serverInfoWindow, 0, 2, "ИНФОРМАЦИЯ О СЕРВЕРЕ");
    mvwprintw(serverInfoWindow, 1, 1, "Системное время:");
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(date, 20, "%H:%M:%S %d/%m/%Y", timeinfo); // форматируем строку времени
    mvwprintw(serverInfoWindow, 2, 1, date);
    mvwprintw(serverInfoWindow, 3, 1, "IP адрес сервера: ");
    mvwprintw(serverInfoWindow, 4, 1, (char*)ip_address);

    wrefresh(serverInfoWindow);
}

void TerminalGraphic::printBackgroundWindow(void) {
    setlocale(LC_ALL,"");
    initscr(); 
    noecho();
    cbreak();   
    curs_set(0);    
    // Измеряем размер экрана в рядах и колонках
    // int height, width;
    // getmaxyx(stdscr, height, width);

    // WINDOW *main_win = newwin(height, width, 0, 0);
    WINDOW *main_win = newwin(19, 72, 0, 0);
    box(main_win, 0, 0);
    refresh();
    // move and print in window
    mvwprintw(main_win, 0, 25, "ПАНЕЛЬ УПРАВЛЕНИЯ СИСТЕМОЙ \"УМНЫЙ ДОМ\"");
    wrefresh(main_win);
}

void TerminalGraphic::findServerIp(void) {
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