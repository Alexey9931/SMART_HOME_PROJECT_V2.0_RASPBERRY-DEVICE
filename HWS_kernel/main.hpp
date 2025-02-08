#ifndef _MAIN_HPP
#define _MAIN_HPP

#include "config_parser.hpp"
#include <map>
#include <vector>
#include <iostream>
#include <arpa/inet.h>
#include <stdio.h>
#include <thread>
#include <string.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <cstdint>
#include <errno.h>
#include <signal.h>
#include <mutex>
#include <net/if.h>
#include "sh_memory.hpp"
#include "log.hpp"
#include "sql_db.hpp"

void devicesTask(Logger &log, std::list<std::pair<std::string, std::string>> devicesConfigs);
void poolingDevice(Logger &log, std::string srcAddr, std::string devAddr);
void terminalRemoteTask(Logger &log);
void findServerIp(char *ip_address);

#endif