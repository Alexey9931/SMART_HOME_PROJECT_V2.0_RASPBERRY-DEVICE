#include "modbus.hpp"

ModbusPack::ModbusPack() {
    fillCrc32Table();
    txPackFrame.headerFrame.header = PACK_HEADER;
    txPackFrame.tailFrame.end = PACK_TAIL;
}

uint8_t ModbusPack::translateRxPack(uint8_t *rxBuf) {
    memcpy(&rxPackFrame.headerFrame, rxBuf, sizeof(ModbusHeader));
    if (rxPackFrame.headerFrame.header != PACK_HEADER) {
        return 1;       
    }

    memcpy(rxPackFrame.data, rxBuf + sizeof(ModbusHeader), rxPackFrame.headerFrame.length - sizeof(rxPackFrame.tailFrame.crc)
    - sizeof(rxPackFrame.headerFrame) + sizeof(rxPackFrame.headerFrame.header));

    memcpy(&rxPackFrame.tailFrame, rxBuf + sizeof(ModbusHeader) + rxPackFrame.headerFrame.length - sizeof(rxPackFrame.tailFrame.crc)
    - sizeof(rxPackFrame.headerFrame) + sizeof(rxPackFrame.headerFrame.header), sizeof(rxPackFrame.tailFrame));
    if (rxPackFrame.tailFrame.end != PACK_TAIL) {
        return 1;
    }

    if (rxPackFrame.tailFrame.crc != countCrc((uint8_t *)&rxPackFrame.headerFrame.recvAddr, rxPackFrame.headerFrame.length - 
    sizeof(rxPackFrame.tailFrame.crc))) {
        return 1;    
    }

    rxDataSize = rxPackFrame.headerFrame.length - sizeof(rxPackFrame.tailFrame.crc)
    - sizeof(rxPackFrame.headerFrame) + sizeof(rxPackFrame.headerFrame.header);

    return 0;
}

void ModbusPack::makeTxPack() {
    memcpy(txBuf, &txPackFrame.headerFrame, sizeof(ModbusHeader));
    memcpy(txBuf + sizeof(ModbusHeader), txPackFrame.data, txDataSize);

    txPackFrame.tailFrame.crc = countCrc(txBuf + sizeof(txPackFrame.headerFrame.header), txPackFrame.headerFrame.length - 
        sizeof(txPackFrame.tailFrame.crc));

    memcpy(txBuf + sizeof(ModbusHeader) + txDataSize, &txPackFrame.tailFrame, sizeof(txPackFrame.tailFrame));
}

uint_least32_t ModbusPack::countCrc(unsigned char *buf, size_t len) {
    uint_least32_t crc;

    crc = 0xFFFFFFFFUL;
    while (len--) {
        crc = crc32Table[(crc ^ *buf++) & 0xFF] ^ (crc >> 8);
    }

    return crc ^ 0xFFFFFFFFUL;
}

void ModbusPack::fillCrc32Table(void) {
    uint_least32_t crc;
    int i, j;

    for (i = 0; i < 256; i++) {
        crc = i;
        for (j = 0; j < 8; j++) {
            crc = crc & 1 ? (crc >> 1) ^ 0xEDB88320UL : crc >> 1;
        }
        crc32Table[i] = crc;
    }
}


