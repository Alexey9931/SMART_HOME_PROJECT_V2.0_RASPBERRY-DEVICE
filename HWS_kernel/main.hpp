#ifndef _MAIN_HPP
#define _MAIN_HPP

#include "config_parser.hpp"
#include "sh_memory.hpp"
#include <map>
#include <vector>
#include <iostream>
#include <arpa/inet.h>
#include <stdio.h>
#include <thread>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstdint>
#include "log.hpp"

void devicesTask(Logger &log, std::list<std::pair<std::string, std::string>> devicesConfigs);
void poolingDevice(Logger &log, std::string srcAddr, std::string devAddr);

#endif