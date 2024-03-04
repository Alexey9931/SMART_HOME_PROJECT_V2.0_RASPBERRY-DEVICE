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

void TerminalGraphic::printMainMenu(void) {
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
    while (1) {
        sharedMemory.copyFromSharedMemory();
        for (uint8_t i = 0; i < sizeof(sharedMemory.shMemoryStruct)/sizeof(sharedMemory.shMemoryStruct.device[0]); i++) {
            if (i == highlight) {
                wattron(menuWindow, A_STANDOUT);
            }
            if (sharedMemory.shMemoryStruct.device[i].isInit == true) {
                stringsMainMenu[i].assign((char*)sharedMemory.shMemoryStruct.device[i].devRegsSpace.deviceName, 
                    sizeof(sharedMemory.shMemoryStruct.device[i].devRegsSpace.deviceName));
            } else {                
                stringsMainMenu[i] = "ПУСТО        ";
                memset(&sharedMemory.shMemoryStruct.device[i].devRegsSpace, 0, sizeof(sharedMemory.shMemoryStruct.device[i].devRegsSpace));
                sharedMemory.copyToSharedMemory();
            }
            mvwprintw(menuWindow, 1 + i, 1, ("-" + std::to_string(i+1) + "- " + stringsMainMenu[i]).c_str()); 
            wattroff(menuWindow, A_STANDOUT);
            refresh();
        }
        switch (wgetch(menuWindow)) {
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
            case 10:    /*Enter*/
                printDeviceInfoWindow(sharedMemory.shMemoryStruct.device[highlight]);
                printDeviceDataWindow(sharedMemory.shMemoryStruct.device[highlight]);
                break;
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
        (std::to_string(device.devRegsSpace.ipAddr[0]) + "." + 
        std::to_string(device.devRegsSpace.ipAddr[1]) + "." + 
        std::to_string(device.devRegsSpace.ipAddr[2]) + "." +
        std::to_string(device.devRegsSpace.ipAddr[3])).c_str());
    mvwprintw(deviceInfoWindow, 4, 1, "MAC адрес у-ва: ");
    mvwprintw(deviceInfoWindow, 4, 20, 
        (convertIntToHex(device.devRegsSpace.macAddr[0]) + "." + 
        convertIntToHex(device.devRegsSpace.macAddr[1]) + "." + 
        convertIntToHex(device.devRegsSpace.macAddr[2]) + "." +
        convertIntToHex(device.devRegsSpace.macAddr[3]) + "." +
        convertIntToHex(device.devRegsSpace.macAddr[4]) + "." +
        convertIntToHex(device.devRegsSpace.macAddr[5])).c_str());
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
    mvwprintw(deviceInfoWindow, 1, 18, (std::to_string(device.devRegsSpace.sysTime.hour) + ":" + 
        std::to_string(device.devRegsSpace.sysTime.minutes) + ":" + 
        std::to_string(device.devRegsSpace.sysTime.seconds) + " " +
        std::to_string(device.devRegsSpace.sysTime.dayOfMonth) + "/" +
        std::to_string(device.devRegsSpace.sysTime.month) + "/20" +
        std::to_string(device.devRegsSpace.sysTime.year)).c_str());
    mvwprintw(deviceInfoWindow, 2, 1, "Температура:");
    mvwprintw(deviceInfoWindow, 2, 15, (std::to_string(device.devRegsSpace.temperature) + "°C").c_str());
    mvwprintw(deviceInfoWindow, 3, 1, "Влажность:");
    mvwprintw(deviceInfoWindow, 3, 15, (std::to_string(device.devRegsSpace.humidity) + "%%").c_str());
    mvwprintw(deviceInfoWindow, 4, 1, "Атм.давление:");
    mvwprintw(deviceInfoWindow, 4, 15, (std::to_string(device.devRegsSpace.pressure) + "мм.рт.ст.").c_str());

    wrefresh(deviceInfoWindow);
}

void TerminalGraphic::printBackgroundWindow(void) {
    setlocale(LC_ALL,"");
    initscr(); 
    noecho();
    cbreak();   
    curs_set(0);    
    // Измеряем размер экрана в рядах и колонках
    int height, width;
    getmaxyx(stdscr, height, width);

    WINDOW *main_win = newwin(height, width, 0, 0);
    box(main_win, 0, 0);
    refresh();
    // move and print in window
    mvwprintw(main_win, 0, 25, "ПАНЕЛЬ УПРАВЛЕНИЯ СИСТЕМОЙ \"УМНЫЙ ДОМ\"");
    wrefresh(main_win);
}