#include "sqlDataBaseTask.hpp"
#include "sql_db.hpp"

SqlDataBaseTask::SqlDataBaseTask(std::mutex *sharedMemoryMut, SharedMemory *sharedMemory) :
            Logger("smhomekernel", "/usr/local/sm_home/smhomekernel.log", "a"),
            sharedMemoryMut{sharedMemoryMut}, sharedMemory{sharedMemory} {}

SqlDataBaseTask::~SqlDataBaseTask() {
    task.join();
}

void SqlDataBaseTask::runTask() {
    task = std::thread([&]() {
        sqlDataBaseTask();
    });
}

void SqlDataBaseTask::sqlDataBaseTask() {
    SQLDataBase MySQLDb(MYSQL_SERVER_NAME, MYSQL_USER, MYSQL_PASSWD, MYSQL_DB_NAME);
    MySQLDb.controlPanelTable->createLocalTable();
    MySQLDb.gasBoilControlTable->createLocalTable();
    MySQLDb.weatherStationTable->createLocalTable();
    MySQLDb.controlPanelTable->deleteDataFromLocalTableForTimePeriod(7);
    MySQLDb.gasBoilControlTable->deleteDataFromLocalTableForTimePeriod(7);
    MySQLDb.weatherStationTable->deleteDataFromLocalTableForTimePeriod(7);
    MySQLDb.controlPanelTable->translateDataToRemoteTable();
    MySQLDb.gasBoilControlTable->translateDataToRemoteTable();
    MySQLDb.weatherStationTable->translateDataToRemoteTable();

    while (1) {
        if (MySQLDb.deferedTruncateRemoteTables) {
            MySQLDb.truncateRemoteTables();
        }
        for (uint8_t id = 0; id < (sizeof(sharedMemory->shMemoryStruct.device)/sizeof(sharedMemory->shMemoryStruct.device[0])); id++)
        {
            sharedMemoryMut->lock();
            if ((sharedMemory->shMemoryStruct.device[id].isInit) &&
                (sharedMemory->shMemoryStruct.device[id].deviceRegs.commonRomRegsSpace.deviceName[0] != NULL))
            {
                if (strstr((const char*)sharedMemory->shMemoryStruct.device[id].deviceName, CONTROL_PANEL_NAME) != NULL) {
                    controlPanelDB dbData = {0};
                    dbData.humidity = sharedMemory->shMemoryStruct.device[id].deviceRegs.deviceRamRegsSpace.contPanelRamRegSpace.humidity;
                    dbData.pressure = sharedMemory->shMemoryStruct.device[id].deviceRegs.deviceRamRegsSpace.contPanelRamRegSpace.pressure;
                    dbData.temperature = sharedMemory->shMemoryStruct.device[id].deviceRegs.deviceRamRegsSpace.contPanelRamRegSpace.temperature;
                    sharedMemoryMut->unlock();
                    MySQLDb.controlPanelTable->sendDataToLocalTable(&dbData);
                    MySQLDb.controlPanelTable->translateDataToRemoteTable();
                } else if (strstr((const char*)sharedMemory->shMemoryStruct.device[id].deviceName, GAS_BOILER_CONTROLLER_NAME)!= NULL) {
                    gasBoilControlDB dbData = {0};
                    dbData.currentTemp = sharedMemory->shMemoryStruct.device[id].deviceRegs.deviceRamRegsSpace.gasBoilerContRamRegSpace.currentTemperature;
                    dbData.setpointSource = sharedMemory->shMemoryStruct.device[id].deviceRegs.deviceRomRegsSpace.gasBoilerContRomRegSpace.tempSource;
                    dbData.setpointTemp = sharedMemory->shMemoryStruct.device[id].deviceRegs.deviceRomRegsSpace.gasBoilerContRomRegSpace.tempSetpoint;
                    dbData.status = sharedMemory->shMemoryStruct.device[id].deviceRegs.deviceRamRegsSpace.gasBoilerContRamRegSpace.releStatus;
                    dbData.tempRange = sharedMemory->shMemoryStruct.device[id].deviceRegs.deviceRomRegsSpace.gasBoilerContRomRegSpace.tempRange;
                    sharedMemoryMut->unlock();
                    MySQLDb.gasBoilControlTable->sendDataToLocalTable(&dbData);
                    MySQLDb.gasBoilControlTable->translateDataToRemoteTable();
                } else if (strstr((const char*)sharedMemory->shMemoryStruct.device[id].deviceName, WEATHER_STATION_NAME)!= NULL) {
                    weatherStationDB dbData = {0};
                    dbData.humidity = sharedMemory->shMemoryStruct.device[id].deviceRegs.deviceRamRegsSpace.weathStatRamRegSpace.humidity;
                    dbData.rainFall = sharedMemory->shMemoryStruct.device[id].deviceRegs.deviceRamRegsSpace.weathStatRamRegSpace.rainFall;
                    dbData.temperature = sharedMemory->shMemoryStruct.device[id].deviceRegs.deviceRamRegsSpace.weathStatRamRegSpace.temperature;
                    dbData.windDirect = sharedMemory->shMemoryStruct.device[id].deviceRegs.deviceRamRegsSpace.weathStatRamRegSpace.windDirect;
                    dbData.windSpeed = sharedMemory->shMemoryStruct.device[id].deviceRegs.deviceRamRegsSpace.weathStatRamRegSpace.windSpeed;
                    sharedMemoryMut->unlock();
                    MySQLDb.weatherStationTable->sendDataToLocalTable(&dbData);
                    MySQLDb.weatherStationTable->translateDataToRemoteTable();
                } else {
                    sharedMemoryMut->unlock();
                }
            } else {
                sharedMemoryMut->unlock();
            }
        }
        MySQLDb.controlPanelTable->deleteDataFromLocalTableForTimePeriod(7);
        MySQLDb.gasBoilControlTable->deleteDataFromLocalTableForTimePeriod(7);
        MySQLDb.weatherStationTable->deleteDataFromLocalTableForTimePeriod(7);
        MySQLDb.deleteDataFromRemoteTablesForTimePeriod(7);
        sleep(300);
    }
}
