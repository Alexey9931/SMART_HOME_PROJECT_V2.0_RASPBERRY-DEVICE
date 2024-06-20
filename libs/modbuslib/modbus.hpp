#ifndef _MODBUS_HPP
#define _MODBUS_HPP

#include <iostream>
#include <string.h>
#include <cstdint>

#define BUF_LEN     1024
#define PACK_HEADER 0x55
#define PACK_TAIL   0xAAAA

#define READ_CMD    0x01
#define WRITE_CMD   0x02
#define CONFIG_CMD  0x03
#define RESET_CMD   0x04
#define TYPE_CMD    0x05
#define INIT_CMD    0x06

struct ModbusHeader {
    uint8_t header;
    uint16_t recvAddr;
    uint16_t senderAddr;
    uint16_t length;
    uint8_t cmd;
} __attribute__((packed));

struct ModbusTail {
    uint32_t crc;
    uint16_t end;
} __attribute__((packed));

struct ModbusFrame {
    ModbusHeader headerFrame;
    uint8_t data[512];
    ModbusTail tailFrame;
} __attribute__((packed));

class ModbusPack {
    public:
        ModbusFrame rxPackFrame;
        ModbusFrame txPackFrame;
        uint8_t rxBuf[1024];
        uint8_t txBuf[1024];
        uint32_t rxDataSize;
        uint32_t txDataSize;
        ModbusPack();
        uint8_t translateRxPack(uint8_t *rxBuf);
        void makeTxPack();
    private:
        uint_least32_t crc32Table[256];
        uint_least32_t countCrc(unsigned char *buf, size_t len);
        void fillCrc32Table(void);
};

#endif