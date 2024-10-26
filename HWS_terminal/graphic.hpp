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
#include <signal.h>
#include <cstdint>
#include "log.hpp"
#include "sh_memory.hpp"

class TerminalGraphic : public Logger {
    public:
        TerminalGraphic();
        TerminalGraphic(char *serverIP);
        ~TerminalGraphic() {
            getch();
            endwin();
        }
        shMemoryDef sharedMemStruct;
        std::vector<std::string> stringsMainMenu {
            "ПУСТО        ",
            "ПУСТО        ", 
            "ПУСТО        ", 
            "ПУСТО        ", 
            "ПУСТО        "};
        void printMainMenu(void);
        void printDeviceInfoWindow(Device device);
        void printDeviceDataWindow(Device device);
        void printBackgroundWindow(void);
        void printServerInfoWindow(char *serverIP);
        void buttonListener(WINDOW *window, int &highlight);
        void getSharedStructTask(bool remote, void *obj);
    private:
        std::string convertIntToHex(int toConvert);
        void findServerIp(void);
};

#endif