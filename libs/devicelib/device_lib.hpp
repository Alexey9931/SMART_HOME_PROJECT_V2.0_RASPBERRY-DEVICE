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

//Device's register space
#define CONTROL_PANEL_REGS_SIZE             122
#define GAS_BOILER_CONTROLLER_REGS_SIZE     119
#define WEATH_STATION_REGS_SIZE             127

//Device's names
#define CONTROL_PANEL_NAME          "Control Panel"
#define GAS_BOILER_CONTROLLER_NAME  "Gas Boiler Controller"
#define WEATHER_STATION_NAME        "Weather Station"

typedef struct deviceSystemTime {
    uint8_t     seconds;
    uint8_t     minutes;
    uint8_t     hour;
    uint8_t     dayOfWeak;
    uint8_t     dayOfMonth;
    uint8_t     month;
    uint8_t     year;
}__attribute__((packed)) devSysTime;

typedef struct controlPanelRomRegSpace {
    uint8_t reserv[8];
}__attribute__((packed)) controlPanelRomRegs;

typedef struct controlPanelRamRegSpace {
    float           temperature;
    float           humidity;
    float           pressure;
}__attribute__((packed)) controlPanelRamRegs;

typedef struct gasBoilerControllerRomRegSpace {
    float           tempSetpoint;
    float           tempRange;
}__attribute__((packed)) gasBoilerRomRegs;

typedef struct gasBoilerControllerRamRegSpace {
    float           temperature;
    float           humidity;
    uint8_t         releStatus;
}__attribute__((packed)) gasBoilerRamRegs;

// Структура байта-описвания регистра WindDirection
typedef struct windDirectionStruct {
	unsigned int north:1;
	unsigned int northeast:1;
	unsigned int east:1;
	unsigned int southeast:1;
	unsigned int south:1;
	unsigned int southwest:1;
	unsigned int west:1;
	unsigned int northwest:1;
}__attribute__((packed)) windDirection;

typedef struct weathStatRomRegSpace {
    uint8_t reserv[8];
}__attribute__((packed)) weathStatRomRegs;

typedef struct weathStatRamRegSpace {
    float           temperature;
    float           humidity;
    float           rainFall;
    float           windSpeed;
    windDirection	windDirect;
}__attribute__((packed)) weathStatRamRegs;

// Общие регистры для всех модулей, которые хранятся в ПЗУ
typedef struct commonRomRegsStruct {
    uint8_t     deviceName[32];
    uint8_t     ipAddr1[4];
    uint8_t     ipAddr2[4];
    uint8_t     ipGate[4];
    uint8_t     ipMask[4];
    uint8_t     macAddr1[6];
    uint8_t     macAddr2[6];
    uint32_t    localPort[2];
    float		temp_correction;
	float		hum_correction;
}__attribute__((packed)) commonRomRegs;

// Общие регистры для всех модулей, которые хранятся в ОЗУ
typedef struct commonRamRegsStruct {
    int         numRxPack;
    int         numTxPack;
    int         workHours;
    devSysTime  startTime;
    devSysTime  sysTime;
}__attribute__((packed)) commonRamRegs;

// Регистры конкретного у-ва, которые хранятся в ПЗУ
typedef union deviceRomRegsStruct {
    controlPanelRomRegs    contPanelRomRegSpace;
    gasBoilerRomRegs       gasBoilerContRomRegSpace;
    weathStatRomRegs       weathStatRomRegSpace;
} devRomRegs;

// Регистры конкретного у-ва, которые хранятся в ОЗУ
typedef union deviceRamRegsStruct {
    controlPanelRamRegs    contPanelRamRegSpace;
    gasBoilerRamRegs       gasBoilerContRamRegSpace;
    weathStatRamRegs       weathStatRamRegSpace;
} devRamRegs;

// Вся карта регистров устройства
typedef struct deviceRegisterSpace {
    commonRomRegs   commonRomRegsSpace;
    devRomRegs      deviceRomRegsSpace;
    commonRamRegs   commonRamRegsSpace;
    devRamRegs      deviceRamRegsSpace;
} __attribute__((packed)) devRegsSpace;

class Device {
    public:
        bool isInit = false;
        int numRxPack = 0;
        int numTxPack = 0;
        uint8_t deviceName[32];
        uint8_t deviceIpAddr[15];
        uint8_t sourceIpAddr[15];
        int socket_fd = 0;
        
        bool isWriteLock = false;

        devRegsSpace deviceRegs;

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
        uint8_t initCmd(void);
        void _init(uint32_t dstAddr, uint32_t srcAddr, ModbusPack& iterationPack);
        uint8_t deviceAddrFromIP(std::string devAddr);
};

#endif