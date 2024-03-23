#ifndef _MAIN_HPP
#define _MAIN_HPP

#include <map>
#include <vector>
#include <list>
#include <iostream>
#include <arpa/inet.h>
#include <stdio.h>
#include <thread>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include "device_lib.hpp" 
#include "sh_memory.hpp" 
#include "log.hpp" 
#include "graphic.hpp" 
#include <cstdint>

class NetMapBox {
    public:
        //NetMapBox(std::list<ConfigEntry>);
        std::map<std::string, std::pair<std::string, bool>> net_map;
    private:

};

#endif