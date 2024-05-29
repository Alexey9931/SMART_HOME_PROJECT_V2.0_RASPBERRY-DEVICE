#ifndef _GRAPHIC_HPP
#define _GRAPHIC_HPP

#include <unistd.h>
#include <stdio.h>
#include <locale.h>
#include <ncurses.h>
#include <vector>
#include <string>
#include "device_lib.hpp"
#include <sstream>
#include <thread>
#include <ctime>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <cstdint>
#include "log.hpp"

class TerminalGraphic {
    public:
        TerminalGraphic();
        uint8_t ip_address[15];
        std::vector<std::string> stringsMainMenu {
            "ПУСТО        ",
            "ПУСТО        ", 
            "ПУСТО        ", 
            "ПУСТО        ", 
            "ПУСТО        "};
        void printMainMenu(Logger &log);
        void printDeviceInfoWindow(Device device);
        void printDeviceDataWindow(Device device);
        void printBackgroundWindow(void);
        void printServerInfoWindow(void);
        void buttonListener(WINDOW *window, int &highlight);
    private:
        std::string convertIntToHex(int toConvert);
        void findServerIp(void);
};

#endif