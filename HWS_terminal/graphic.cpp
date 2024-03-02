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
    WINDOW *menuWindow = newwin(14, 27, 1, 2);
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
        for (uint8_t i = 0; i < sizeof(sharedMemory.shMemoryStruct)/sizeof(sharedMemory.shMemoryStruct.device[0]); i++) {
            if (i == highlight) {
                wattron(menuWindow, A_STANDOUT);
            }
            if (sharedMemory.shMemoryStruct.device[i].isInit == true) {
                stringsMainMenu[i].assign((char*)sharedMemory.shMemoryStruct.device[i].devRegsSpace.deviceName, 
                    sizeof(sharedMemory.shMemoryStruct.device[i].devRegsSpace.deviceName));
            } else {
                stringsMainMenu[i] = "ПУСТО";
            }
            mvwprintw(menuWindow, 1 + i, 1, ("-" + std::to_string(i+1) + "- " + stringsMainMenu[i]).c_str()); 
            wattroff(menuWindow, A_STANDOUT);
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
                sharedMemory.copyFromSharedMemory();
                printDeviceInfoWindow(sharedMemory.shMemoryStruct.device[highlight]);
                break;
        }
    }
}

void TerminalGraphic::printDeviceInfoWindow(Device device) {
    WINDOW *deviceInfoWindow = newwin(20, 40, 1, 30);
    box(deviceInfoWindow, 0, 0);
    refresh();
    mvwprintw(deviceInfoWindow, 0, 2, "ИНФОРМАЦИЯ ОБ УСТРОЙСТВЕ");
    mvwprintw(deviceInfoWindow, 1, 1, "Описание: ");
    mvwprintw(deviceInfoWindow, 1, 15, (char*)device.devRegsSpace.deviceName);
    mvwprintw(deviceInfoWindow, 2, 1, "IP адрес у-ва: ");
    mvwprintw(deviceInfoWindow, 2, 20, 
        (std::to_string(device.devRegsSpace.ipAddr[0]) + "." + 
        std::to_string(device.devRegsSpace.ipAddr[1]) + "." + 
        std::to_string(device.devRegsSpace.ipAddr[2]) + "." +
        std::to_string(device.devRegsSpace.ipAddr[3])).c_str());
    mvwprintw(deviceInfoWindow, 3, 1, "MAC адрес у-ва: ");
    mvwprintw(deviceInfoWindow, 3, 20, 
        (convertIntToHex(device.devRegsSpace.macAddr[0]) + "." + 
        convertIntToHex(device.devRegsSpace.macAddr[1]) + "." + 
        convertIntToHex(device.devRegsSpace.macAddr[2]) + "." +
        convertIntToHex(device.devRegsSpace.macAddr[3]) + "." +
        convertIntToHex(device.devRegsSpace.macAddr[4]) + "." +
        convertIntToHex(device.devRegsSpace.macAddr[5])).c_str());
    mvwprintw(deviceInfoWindow, 4, 1, "IP адрес маршр-ра: ");
    mvwprintw(deviceInfoWindow, 4, 20, 
        (std::to_string(device.devRegsSpace.ipGate[0]) + "." + 
        std::to_string(device.devRegsSpace.ipGate[1]) + "." + 
        std::to_string(device.devRegsSpace.ipGate[2]) + "." +
        std::to_string(device.devRegsSpace.ipGate[3])).c_str());
    mvwprintw(deviceInfoWindow, 5, 1, "Маска подсети: ");
    mvwprintw(deviceInfoWindow, 5, 20, 
        (std::to_string(device.devRegsSpace.ipMask[0]) + "." + 
        std::to_string(device.devRegsSpace.ipMask[1]) + "." + 
        std::to_string(device.devRegsSpace.ipMask[2]) + "." +
        std::to_string(device.devRegsSpace.ipMask[3])).c_str());
    mvwprintw(deviceInfoWindow, 6, 1, "Порт соединения: ");
    mvwprintw(deviceInfoWindow, 6, 20, std::to_string(device.devRegsSpace.localPort).c_str());
    mvwprintw(deviceInfoWindow, 7, 1, "Кол-во отправленных пакетов: ");
    mvwprintw(deviceInfoWindow, 7, 30, std::to_string(device.devRegsSpace.numTxPack).c_str());
    mvwprintw(deviceInfoWindow, 8, 1, "Кол-во принятых пакетов: ");
    mvwprintw(deviceInfoWindow, 8, 30, std::to_string(device.devRegsSpace.numRxPack).c_str());
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