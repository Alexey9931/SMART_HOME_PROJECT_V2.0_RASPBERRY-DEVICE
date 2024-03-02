#include "device_lib.hpp"

uint8_t Device::writeCmd(uint16_t regAddr, void *value, uint16_t valueSize) {
    ModbusPack requestReplyIterat;
    std::string devAddr;
    std::string srcIpAddr;
    devAddr.append((char*)deviceIpAddr, sizeof(deviceIpAddr));
    srcIpAddr.append((char*)sourceIpAddr, sizeof(sourceIpAddr));

    _writeReg(deviceAddrFromIP(devAddr), deviceAddrFromIP(srcIpAddr), regAddr, value, valueSize, requestReplyIterat);
    
    if (send(socket_fd, requestReplyIterat.txBuf, sizeof(ModbusHeader) + sizeof(ModbusTail) 
        + requestReplyIterat.txDataSize, 0) < 0) {
        std::cout << "Sent error!" << std::endl;
        return 1;
    }

    numTxPack++;

    int len = read(socket_fd, requestReplyIterat.rxBuf, sizeof(ModbusHeader) + sizeof(ModbusTail) + 1);
    if (len < 0) {
        std::cout << "Receive error!" << std::endl;
        return 1;
    }

    if (requestReplyIterat.translateRxPack(requestReplyIterat.rxBuf) != 0) {
        return 1; 
    }

    if ((requestReplyIterat.rxDataSize != 1) || (requestReplyIterat.rxPackFrame.data[0] != WRITE_CMD)) {
        return 1; 
    }

    numRxPack++;

    return 0;
}

uint8_t Device::_writeReg(uint32_t dstAddr, uint32_t srcAddr, uint16_t regAddr, void *value, uint16_t valueSize, ModbusPack& iterationPack) {
    memcpy(iterationPack.txPackFrame.data, &regAddr, sizeof(regAddr));
    memcpy(iterationPack.txPackFrame.data + sizeof(regAddr), &valueSize, sizeof(valueSize));
    memcpy(iterationPack.txPackFrame.data + sizeof(regAddr) + sizeof(valueSize), value, valueSize);

    iterationPack.txDataSize = valueSize + sizeof(regAddr) + sizeof(valueSize);

    iterationPack.txPackFrame.headerFrame.cmd = WRITE_CMD;
    iterationPack.txPackFrame.headerFrame.recvAddr = dstAddr;
    iterationPack.txPackFrame.headerFrame.senderAddr = srcAddr;
    iterationPack.txPackFrame.headerFrame.length = sizeof(ModbusHeader) - sizeof(iterationPack.txPackFrame.headerFrame.header) + 
        sizeof(iterationPack.txPackFrame.tailFrame.crc) + iterationPack.txDataSize;

    iterationPack.makeTxPack();

    return 0;
}

uint8_t Device::readCmd(uint16_t regAddr, uint16_t valueSize, uint8_t* value) {
    ModbusPack requestReplyIterat;
    std::string devAddr;
    std::string srcIpAddr;
    devAddr.append((char*)deviceIpAddr, sizeof(deviceIpAddr));
    srcIpAddr.append((char*)sourceIpAddr, sizeof(sourceIpAddr));

    _readReg(deviceAddrFromIP(devAddr), deviceAddrFromIP(srcIpAddr), regAddr, valueSize, requestReplyIterat);
    
    if (send(socket_fd, requestReplyIterat.txBuf, sizeof(ModbusHeader) + sizeof(ModbusTail) 
        + requestReplyIterat.txDataSize, 0) < 0) {
        std::cout << "Sent error!" << std::endl;
        return 1;
    }

    numTxPack++;

    int len = read(socket_fd, requestReplyIterat.rxBuf, sizeof(ModbusHeader) + sizeof(ModbusTail) + valueSize);
    if (len < 0) {
        std::cout << "Receive error!" << std::endl;
        return 1;
    }

    if (requestReplyIterat.translateRxPack(requestReplyIterat.rxBuf) != 0) {
        return 1; 
    }

    if (requestReplyIterat.rxDataSize != valueSize) {
        return 1; 
    }

    numRxPack++;

    memcpy(value, requestReplyIterat.rxPackFrame.data, valueSize);

    return 0;
}

uint8_t Device::_readReg(uint32_t dstAddr, uint32_t srcAddr, uint16_t regAddr, uint16_t valueSize, ModbusPack& iterationPack) {
    memcpy(iterationPack.txPackFrame.data, &regAddr, sizeof(regAddr));
    memcpy(iterationPack.txPackFrame.data + sizeof(regAddr), &valueSize, sizeof(valueSize));
    
    iterationPack.txDataSize = sizeof(valueSize) + sizeof(regAddr);

    iterationPack.txPackFrame.headerFrame.cmd = READ_CMD;
    iterationPack.txPackFrame.headerFrame.recvAddr = dstAddr;
    iterationPack.txPackFrame.headerFrame.senderAddr = srcAddr;
    iterationPack.txPackFrame.headerFrame.length = sizeof(ModbusHeader) - sizeof(iterationPack.txPackFrame.headerFrame.header) + 
        sizeof(iterationPack.txPackFrame.tailFrame.crc) + iterationPack.txDataSize;

    iterationPack.makeTxPack();

    return 0;
}

uint8_t Device::configCmd(uint16_t regAddr, void *value, uint16_t valueSize) {
    ModbusPack requestReplyIterat;
    std::string devAddr;
    std::string srcIpAddr;
    devAddr.append((char*)deviceIpAddr, sizeof(deviceIpAddr));
    srcIpAddr.append((char*)sourceIpAddr, sizeof(sourceIpAddr));

    _config(deviceAddrFromIP(devAddr), deviceAddrFromIP(srcIpAddr), regAddr, value, valueSize, requestReplyIterat);
    
    if (send(socket_fd, requestReplyIterat.txBuf, sizeof(ModbusHeader) + sizeof(ModbusTail) 
        + requestReplyIterat.txDataSize, 0) < 0) {
        std::cout << "Sent error!" << std::endl;
        return 1;
    }

    numTxPack++;

    int len = read(socket_fd, requestReplyIterat.rxBuf, sizeof(ModbusHeader) + sizeof(ModbusTail) + 1);
    if (len < 0) {
        std::cout << "Receive error!" << std::endl;
        return 1;
    }

    if (requestReplyIterat.translateRxPack(requestReplyIterat.rxBuf) != 0) {
        return 1; 
    }

    if ((requestReplyIterat.rxDataSize != 1) || (requestReplyIterat.rxPackFrame.data[0] != CONFIG_CMD)) {
        return 1; 
    }

    numRxPack++;

    return 0;
}

uint8_t Device::_config(uint32_t dstAddr, uint32_t srcAddr, uint16_t regAddr, void *value, uint16_t valueSize, ModbusPack& iterationPack) {
    memcpy(iterationPack.txPackFrame.data, &regAddr, sizeof(regAddr));
    memcpy(iterationPack.txPackFrame.data + sizeof(regAddr), &valueSize, sizeof(valueSize));
    memcpy(iterationPack.txPackFrame.data + sizeof(regAddr) + sizeof(valueSize), value, valueSize);

    iterationPack.txDataSize = valueSize + sizeof(regAddr) + sizeof(valueSize);

    iterationPack.txPackFrame.headerFrame.cmd = CONFIG_CMD;
    iterationPack.txPackFrame.headerFrame.recvAddr = dstAddr;
    iterationPack.txPackFrame.headerFrame.senderAddr = srcAddr;
    iterationPack.txPackFrame.headerFrame.length = sizeof(ModbusHeader) - sizeof(iterationPack.txPackFrame.headerFrame.header) + 
        sizeof(iterationPack.txPackFrame.tailFrame.crc) + iterationPack.txDataSize;

    iterationPack.makeTxPack();

    return 0;
}

uint8_t Device::resetCmd(void) {
    ModbusPack requestReplyIterat;
    std::string devAddr;
    std::string srcIpAddr;
    devAddr.append((char*)deviceIpAddr, sizeof(deviceIpAddr));
    srcIpAddr.append((char*)sourceIpAddr, sizeof(sourceIpAddr));

    _reset(deviceAddrFromIP(devAddr), deviceAddrFromIP(srcIpAddr), requestReplyIterat);
    
    if (send(socket_fd, requestReplyIterat.txBuf, sizeof(ModbusHeader) + sizeof(ModbusTail) 
        + requestReplyIterat.txDataSize, 0) < 0) {
        std::cout << "Sent error!" << std::endl;
        return 1;
    }

    numTxPack++;

    int len = read(socket_fd, requestReplyIterat.rxBuf, sizeof(ModbusHeader) + sizeof(ModbusTail) + 1);
    if (len < 0) {
        std::cout << "Receive error!" << std::endl;
        return 1;
    }

    if (requestReplyIterat.translateRxPack(requestReplyIterat.rxBuf) != 0) {
        return 1; 
    }

    if ((requestReplyIterat.rxDataSize != 1) || (requestReplyIterat.rxPackFrame.data[0] != RESET_CMD)) {
        return 1; 
    }

    numRxPack++;

    return 0;
}

uint8_t Device::_reset(uint32_t dstAddr, uint32_t srcAddr, ModbusPack& iterationPack) {
    memset(iterationPack.txPackFrame.data, RESET_CMD, 1);
    iterationPack.txDataSize = 1;

    iterationPack.txPackFrame.headerFrame.cmd = RESET_CMD;
    iterationPack.txPackFrame.headerFrame.recvAddr = dstAddr;
    iterationPack.txPackFrame.headerFrame.senderAddr = srcAddr;
    iterationPack.txPackFrame.headerFrame.length = sizeof(ModbusHeader) - sizeof(iterationPack.txPackFrame.headerFrame.header) + 
        sizeof(iterationPack.txPackFrame.tailFrame.crc) + iterationPack.txDataSize;

    iterationPack.makeTxPack();

    return 0;
}

uint8_t Device::typeCmd(void) {
    ModbusPack requestReplyIterat;
    std::string devAddr;
    std::string srcIpAddr;
    devAddr.append((char*)deviceIpAddr, sizeof(deviceIpAddr));
    srcIpAddr.append((char*)sourceIpAddr, sizeof(sourceIpAddr));

    _type(deviceAddrFromIP(devAddr), deviceAddrFromIP(srcIpAddr), requestReplyIterat);
    
    if (send(socket_fd, requestReplyIterat.txBuf, sizeof(ModbusHeader) + sizeof(ModbusTail) 
        + requestReplyIterat.txDataSize, 0) < 0) {
        std::cout << "Sent error!" << std::endl;
        return 1;
    }
    numTxPack++;

    int len = read(socket_fd, requestReplyIterat.rxBuf, sizeof(ModbusHeader) + sizeof(ModbusTail) + sizeof(devRegsSpace));
    if (len < 0) {
        std::cout << "Receive error!" << std::endl;
        return 1;
    }

    if (requestReplyIterat.translateRxPack(requestReplyIterat.rxBuf) != 0) {
        return 1; 
    }
    numRxPack++;
    
    memcpy(&devRegsSpace, requestReplyIterat.rxPackFrame.data, requestReplyIterat.rxDataSize);
    //deviceName.assign((char*)requestReplyIterat.rxPackFrame.data, requestReplyIterat.rxDataSize);

    return 0;
}

uint8_t Device::_type(uint32_t dstAddr, uint32_t srcAddr, ModbusPack& iterationPack) {
    memset(iterationPack.txPackFrame.data, TYPE_CMD, 1);
    iterationPack.txDataSize = 1;

    iterationPack.txPackFrame.headerFrame.cmd = TYPE_CMD;
    iterationPack.txPackFrame.headerFrame.recvAddr = dstAddr;
    iterationPack.txPackFrame.headerFrame.senderAddr = srcAddr;
    iterationPack.txPackFrame.headerFrame.length = sizeof(ModbusHeader) - sizeof(iterationPack.txPackFrame.headerFrame.header) + 
        sizeof(iterationPack.txPackFrame.tailFrame.crc) + iterationPack.txDataSize;

    iterationPack.makeTxPack();

    return 0;
}

uint8_t Device::deviceAddrFromIP(std::string devAddr) {
    uint8_t i = 0;
    std::string devIP[4];
    std::string delimeter = ".";

    while(devAddr.find(delimeter) != std::string::npos) {
        devIP[i] = devAddr.substr(0, devAddr.find(delimeter));
        devAddr.erase(0, devAddr.find(delimeter) + delimeter.length());
        i++;
    }
    devIP[i++] = devAddr;

    return atoi(devIP[3].c_str());
}