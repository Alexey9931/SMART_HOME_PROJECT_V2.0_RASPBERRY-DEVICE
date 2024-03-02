#ifndef _GRAPHIC_HPP
#define _GRAPHIC_HPP

#include <unistd.h>
#include <locale.h>
#include <ncurses.h>
#include <vector>
#include <string>
#include "device_lib.hpp"
#include <sstream>

class TerminalGraphic {
    public:
        TerminalGraphic();
        std::vector<std::string> stringsMainMenu {"ПУСТО", "ПУСТО", "ПУСТО", "ПУСТО", "ПУСТО"};
        void printMainMenu(void);
        void printDeviceInfoWindow(Device device);
        void printBackgroundWindow(void);
    private:
        std::string convertIntToHex(int toConvert);
};

#endif