#ifndef _SQL_DB_HPP
#define _SQL_DB_HPP

#include <mysql.h>
#include <curl/curl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "log.hpp"
#include "device_lib.hpp"

#define CONROL_PANEL_TABLE_NAME     "ControlPanel"
#define GASBOIL_CONTR_TABLE_NAME    "GasBoilerController"
#define WEATH_STAT_TABLE_NAME       "WeatherStation"

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

template <typename T>
class SQLTable : public Logger {
    public:
        SQLTable(char *tableName, MYSQL *conn, CURL* curl)
            : Logger("smhomekernel", "/usr/local/sm_home/smhomekernel.log", "a"),
            tableName{tableName}, conn{conn}, curl{curl} { }
        T tableStruct;
        char *tableName;
        int rawid = 0;
        void createLocalTable();
        void sendDataToLocalTable(void *data);
        void translateDataToRemoteTable();
    private:
        MYSQL *conn;
        CURL *curl;
};

template <typename T>
void SQLTable<T>::createLocalTable(){
    char query[512] = {};

    sprintf(query, "DROP TABLE IF EXISTS %s", tableName);
    if (mysql_query(conn, query)) {
        Logger::systemlog(LOG_ERR, "%s", mysql_error(conn));
        mysql_close(conn);
        return;
    }
    memset(query, NULL, sizeof(query));

    if (std::is_same<T, controlPanelDB>::value){
        strcat(query,
            "CREATE TABLE `ControlPanel` ("
            "`id` INT PRIMARY KEY AUTO_INCREMENT,"
            "`Temperature` FLOAT DEFAULT NULL,"
            "`Humidity` FLOAT DEFAULT NULL,"
            "`Pressure` FLOAT DEFAULT NULL,"
            "`Time stamp` timestamp NOT NULL DEFAULT current_timestamp() ON UPDATE current_timestamp()"
            ") ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci");
    } else if (std::is_same<T, gasBoilControlDB>::value) {
        strcat(query,
            "CREATE TABLE `GasBoilerController` ("
            "`id` INT PRIMARY KEY AUTO_INCREMENT,"
            "`Status` INT DEFAULT NULL,"
            "`Set temperature` FLOAT DEFAULT NULL,"
            "`Current temperature` FLOAT DEFAULT NULL,"
            "`Temperature range` FLOAT DEFAULT NULL,"
            "`Setpoint source` INT DEFAULT NULL,"
            "`Time stamp` timestamp NOT NULL DEFAULT current_timestamp() ON UPDATE current_timestamp()"
            ") ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci");
    } else if (std::is_same<T, weatherStationDB>::value) {
        strcat(query,
            "CREATE TABLE `WeatherStation` ("
            "`id` INT PRIMARY KEY AUTO_INCREMENT,"
            "`Temperature` FLOAT DEFAULT NULL,"
            "`Humidity` FLOAT DEFAULT NULL,"
            "`Wind speed` FLOAT DEFAULT NULL,"
            "`Wind direction` INT NOT NULL,"
            "`Rainfall` FLOAT DEFAULT NULL,"
            "`Time stamp` timestamp NOT NULL DEFAULT current_timestamp() ON UPDATE current_timestamp()"
            ") ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci");
    } else {
        return;
    }

    if (mysql_query(conn, query)) {
        Logger::systemlog(LOG_ERR, "%s", mysql_error(conn));
        mysql_close(conn);
        return;
    }
    memset(query, NULL, sizeof(query));
    Logger::systemlog(LOG_INFO, "Table \"%s\" has been created!", tableName);
}

template <>
void SQLTable<controlPanelDB>::sendDataToLocalTable(void *data) {
    char query[256] = {0};

    memcpy(&tableStruct, data, sizeof(tableStruct));
    sprintf(query,
        "INSERT INTO `%s` (`Temperature`, `Humidity`, `Pressure`) VALUES(%.2f,%.2f,%.2f)",
        tableName, tableStruct.temperature, tableStruct.humidity, tableStruct.pressure);

    if (mysql_query(conn, query)) {
        Logger::systemlog(LOG_ERR, "%s", mysql_error(conn));
        mysql_close(conn);
        return;
    }
    memset(query, NULL, sizeof(query));
}

template <>
void SQLTable<gasBoilControlDB>::sendDataToLocalTable(void *data) {
    char query[256] = {0};

    memcpy(&tableStruct, data, sizeof(tableStruct));
    sprintf(query,
        "INSERT INTO `%s` (`Status`, `Set temperature`, `Current temperature`, `Temperature range`, `Setpoint source`) VALUES(%d,%.2f,%.2f,%.2f,%d)",
        tableName, tableStruct.status, tableStruct.setpointTemp, tableStruct.currentTemp, tableStruct.tempRange, tableStruct.setpointSource);

    if (mysql_query(conn, query)) {
        Logger::systemlog(LOG_ERR, "%s", mysql_error(conn));
        mysql_close(conn);
        return;
    }
    memset(query, NULL, sizeof(query));
}

template <>
void SQLTable<weatherStationDB>::sendDataToLocalTable(void *data) {
    char query[256] = {0};

    memcpy(&tableStruct, data, sizeof(tableStruct));
    sprintf(query,
        "INSERT INTO `%s` (`Temperature`, `Humidity`, `Wind speed`, `Wind direction`, `Rainfall`) VALUES(%.2f,%.2f,%.2f,%d,%.2f)",
        tableName, tableStruct.temperature, tableStruct.humidity, tableStruct.windSpeed, tableStruct.windDirect, tableStruct.rainFall);

    if (mysql_query(conn, query)) {
        Logger::systemlog(LOG_ERR, "%s", mysql_error(conn));
        mysql_close(conn);
        return;
    }
    memset(query, NULL, sizeof(query));
}

template <>
void SQLTable<controlPanelDB>::translateDataToRemoteTable() {
    char postData[256] = {0};
    char query[64] = {0};

    sprintf(query, "SELECT * FROM `%s`", tableName);
    if (mysql_query(conn, query)) {
        Logger::systemlog(LOG_ERR, "%s", mysql_error(conn));
        return;
    }

    MYSQL_RES *result = mysql_store_result(conn);
    if (result == NULL)
    {
        return;
    }

    int numFields = mysql_num_fields(result);
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result)))
    {
        if (atoi((char*)row[0]) > rawid)
        {
            curl_easy_setopt(curl, CURLOPT_URL, CONTROL_PANEL_POST_URL);
            sprintf(postData,
                "api_key=%s&Temperature=%s&Humidity=%s&Pressure=%s&TimeStamp=%s",
                API_KEY, row[1], row[2], row[3], row[4]);

            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postData);

            CURLcode res = curl_easy_perform(curl);

            if(res != CURLE_OK) {
                Logger::systemlog(LOG_ERR, "CURL POST request failed:%s", curl_easy_strerror(res));
                return;
            }
            memset(postData, NULL, sizeof(postData));
            rawid++;
        }
    }
}

template <>
void SQLTable<gasBoilControlDB>::translateDataToRemoteTable() {
    char postData[256] = {0};
    char query[64] = {0};

    sprintf(query, "SELECT * FROM `%s`", tableName);
    if (mysql_query(conn, query)) {
        Logger::systemlog(LOG_ERR, "%s", mysql_error(conn));
        return;
    }

    MYSQL_RES *result = mysql_store_result(conn);
    if (result == NULL)
    {
        return;
    }

    int numFields = mysql_num_fields(result);
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result)))
    {
        if (atoi((char*)row[0]) > rawid)
        {
            curl_easy_setopt(curl, CURLOPT_URL, GAS_BOIL_CONTR_POST_URL);
            sprintf(postData,
                "api_key=%s&Status=%s&SetTemperature=%s&CurrentTemperature=%s&TemperatureRange=%s&SetpointSource=%s&TimeStamp=%s",
                API_KEY, row[1], row[2], row[3], row[4], row[5], row[6]);

            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postData);

            CURLcode res = curl_easy_perform(curl);

            if(res != CURLE_OK) {
                Logger::systemlog(LOG_ERR, "CURL POST request failed:%s", curl_easy_strerror(res));
                return;
            }
            memset(postData, NULL, sizeof(postData));
            rawid++;
        }
    }
}

template <>
void SQLTable<weatherStationDB>::translateDataToRemoteTable() {
    char postData[512] = {0};
    char query[64] = {0};

    sprintf(query, "SELECT * FROM `%s`", tableName);
    if (mysql_query(conn, query)) {
        Logger::systemlog(LOG_ERR, "%s", mysql_error(conn));
        return;
    }

    MYSQL_RES *result = mysql_store_result(conn);
    if (result == NULL)
    {
        return;
    }

    int numFields = mysql_num_fields(result);
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result)))
    {
        if (atoi((char*)row[0]) > rawid)
        {
            curl_easy_setopt(curl, CURLOPT_URL, WEATHERSTATION_POST_URL);
            sprintf(postData,
                "api_key=%s&Temperature=%s&Humidity=%s&WindSpeed=%s&WindDirection=%s&RainFall=%s&TimeStamp=%s",
                API_KEY, row[1], row[2], row[3], row[4], row[5], row[6]);

            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postData);

            CURLcode res = curl_easy_perform(curl);

            if(res != CURLE_OK) {
                Logger::systemlog(LOG_ERR, "CURL POST request failed:%s", curl_easy_strerror(res));
                return;
            }
            memset(postData, NULL, sizeof(postData));
            rawid++;
        }
    }
}

class SQLDataBase : public Logger {
    public:
        SQLTable<controlPanelDB> *controlPanelTable;
        SQLTable<gasBoilControlDB> *gasBoilControlTable;
        SQLTable<weatherStationDB> *weatherStationTable;
        SQLDataBase(char *serverName, char *user,
                    char *password, char *dataBaseName);
        ~SQLDataBase();
        void truncateRemoteTables();
    private:
        MYSQL *conn;
        CURL *curl;
};

#endif