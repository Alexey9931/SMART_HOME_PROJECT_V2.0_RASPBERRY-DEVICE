#ifndef _DEVICELIB_HPP
#define _DEVICELIB_HPP

#include <iostream>
#include "modbus.hpp"
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <cstdint>

//Errors
#define NO_ERROR    0
#define TX_ERROR    1
#define RX_ERROR    2
#define PACK_ERROR  3

typedef struct deviceSystemTime {
    uint8_t     seconds;
    uint8_t     minutes;
    uint8_t     hour;
    uint8_t     dayOfWeak;
    uint8_t     dayOfMonth;
    uint8_t     month;
    uint8_t     year;
}__attribute__((packed)) devSysTime;

typedef struct deviceRegisterSpace {
    uint8_t     deviceName[32];
    uint8_t     ipAddr1[4];
    uint8_t     ipAddr2[4];
    uint8_t     ipGate[4];
    uint8_t     ipMask[4];
    uint8_t     macAddr1[6];
    uint8_t     macAddr2[6];
    uint32_t    localPort;
    int         numRxPack;
    int         numTxPack;
    devSysTime  sysTime;
    float       temperature;
    float       humidity;
    float       pressure;
}__attribute__((packed)) devRegs;

class Device {
    public:
        bool isInit = false;
        int numRxPack = 0;
        int numTxPack = 0;
        uint8_t deviceName[32];
        uint8_t deviceIpAddr[15];
        uint8_t sourceIpAddr[15];
        int socket_fd = 0;

        devRegs devRegsSpace;

        uint8_t writeCmd(uint16_t regAddr, void *value, uint16_t valueSize);
        void _writeReg(uint32_t dstAddr, uint32_t srcAddr, uint16_t regAddr, void *value, uint16_t valueSize, ModbusPack& iterationPack);
        uint8_t readCmd(uint16_t regAddr, uint16_t valueSize, uint8_t* value);
        void _readReg(uint32_t dstAddr, uint32_t srcAddr, uint16_t regAddr, uint16_t valueSize, ModbusPack& iterationPack);
        uint8_t configCmd(uint16_t regAddr, void *value, uint16_t valueSize);
        void _config(uint32_t dstAddr, uint32_t srcAddr, uint16_t regAddr, void *value, uint16_t valueSize, ModbusPack& iterationPack);
        uint8_t resetCmd(void);
        void _reset(uint32_t dstAddr, uint32_t srcAddr, ModbusPack& iterationPack);
        uint8_t typeCmd(void);
        void _type(uint32_t dstAddr, uint32_t srcAddr, ModbusPack& iterationPack);
        uint8_t deviceAddrFromIP(std::string devAddr);
};

#endif