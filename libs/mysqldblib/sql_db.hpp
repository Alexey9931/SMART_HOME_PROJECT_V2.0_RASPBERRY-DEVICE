#ifndef _SQL_DB_HPP
#define _SQL_DB_HPP

#include <mysql.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "log.hpp"
#include "device_lib.hpp"

#define GASBOIL_CONTR_TABLE_NAME    "GasBoilerController"
#define WEATH_STAT_TABLE_NAME       "WeatherStation"
#define CONROL_PANEL_TABLE_NAME     "ControlPanel"

typedef struct controlPanelDB {
    float temperature;
    float humidity;
    float pressure;
}__attribute__((packed)) controlPanelDB;

typedef struct gasBoilControlDB {
    uint8_t status;
    float   setpointTemp;
    float   currentTemp;
    float   tempRange;
    uint8_t setpointSource;
}__attribute__((packed)) gasBoilControlDB;

typedef struct weatherStationDB {
    float           temperature;
    float           humidity;
    float           windSpeed;
    windDirection   windDirect;
    float           rainFall;
}__attribute__((packed)) weatherStationDB;

class SQLDataBase : public Logger {
    public:
        MYSQL *conn;
        SQLDataBase(char *serverName, char *user,
                    char *password, char *dataBaseName);
        ~SQLDataBase();
        void createTable(char *tableName);
        void sendData(char *tableName, void *data);
};

#endif