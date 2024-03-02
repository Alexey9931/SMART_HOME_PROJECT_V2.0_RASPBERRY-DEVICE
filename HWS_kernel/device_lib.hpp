#ifndef _DEVICELIB_HPP
#define _DEVICELIB_HPP

#include <iostream>
#include "modbus.hpp"
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>

typedef struct deviceRegisterSpace {
    uint8_t     deviceName[32];
    uint8_t     ipAddr[4];
    uint8_t     ipGate[4];
    uint8_t     ipMask[4];
    uint8_t     macAddr[6];
    uint32_t    localPort;
    int         numRxPack;
    int         numTxPack;
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
        uint8_t _writeReg(uint32_t dstAddr, uint32_t srcAddr, uint16_t regAddr, void *value, uint16_t valueSize, ModbusPack& iterationPack);
        uint8_t readCmd(uint16_t regAddr, uint16_t valueSize, uint8_t* value);
        uint8_t _readReg(uint32_t dstAddr, uint32_t srcAddr, uint16_t regAddr, uint16_t valueSize, ModbusPack& iterationPack);
        uint8_t configCmd(uint16_t regAddr, void *value, uint16_t valueSize);
        uint8_t _config(uint32_t dstAddr, uint32_t srcAddr, uint16_t regAddr, void *value, uint16_t valueSize, ModbusPack& iterationPack);
        uint8_t resetCmd(void);
        uint8_t _reset(uint32_t dstAddr, uint32_t srcAddr, ModbusPack& iterationPack);
        uint8_t typeCmd(void);
        uint8_t _type(uint32_t dstAddr, uint32_t srcAddr, ModbusPack& iterationPack);
        uint8_t deviceAddrFromIP(std::string devAddr);
};

#endif