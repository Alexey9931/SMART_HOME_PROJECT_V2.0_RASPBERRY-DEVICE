#ifndef _SQL_DB_HPP
#define _SQL_DB_HPP

#include <mysql.h>
#include <curl/curl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "log.hpp"
#include "device_lib.hpp"

#define GASBOIL_CONTR_TABLE_NAME    "GasBoilerController"
#define WEATH_STAT_TABLE_NAME       "WeatherStation"
#define CONROL_PANEL_TABLE_NAME     "ControlPanel"

#define CONTROL_PANEL_POST_URL      "http://alexgorlov99.ru/smarthomeproject2.0/post-controlpanel.php"
#define GAS_BOIL_CONTR_POST_URL     "http://alexgorlov99.ru/smarthomeproject2.0/post-gasboilercontroller.php"
#define WEATHERSTATION_POST_URL     "http://alexgorlov99.ru/smarthomeproject2.0/post-weatherstation.php"
#define TRUNCATETABLES_POST_URL     "http://alexgorlov99.ru/smarthomeproject2.0/truncate-tables.php"

#define API_KEY "tPmAT5Ab3j7F9&"

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
        SQLDataBase(char *serverName, char *user,
                    char *password, char *dataBaseName);
        ~SQLDataBase();
        void createLocalTable(char *tableName);
        void sendDataToLocalTable(char *tableName, void *data);
        void sendDataToRemoteTable(char *tableName);
        void truncateRemoteTables();
    private:
        MYSQL *conn;
        CURL* curl;
};

#endif