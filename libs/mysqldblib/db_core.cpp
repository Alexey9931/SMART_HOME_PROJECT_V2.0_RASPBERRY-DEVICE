#include "sql_db.hpp"

SQLDataBase::SQLDataBase(char *serverName,
                         char *user,
                         char *password,
                         char *dataBaseName) :
    Logger("smhomekernel", "/usr/local/sm_home/smhomekernel.log", "a") {
    
    char query[64] = {};

    conn = mysql_init(NULL);
    if (conn == NULL) {
        Logger::systemlog(LOG_ERR, "%s", mysql_error(conn));
        mysql_close(conn);
        return;
    }

    if (!mysql_real_connect(conn, serverName, user, password, NULL, 0, NULL, 0))
    {
        Logger::systemlog(LOG_ERR, "%s", mysql_error(conn));
        mysql_close(conn);
        return;
    }

    sprintf(query, "USE %s", dataBaseName);
    mysql_query(conn, query);
    memset(query, NULL, sizeof(query));

    sprintf(query, "DROP DATABASE IF EXISTS %s", dataBaseName);
    if (mysql_query(conn, query))
    {
        Logger::systemlog(LOG_ERR, "%s", mysql_error(conn));
        mysql_close(conn);
        return;
    }
    memset(query, NULL, sizeof(query));

    sprintf(query, "CREATE DATABASE %s", dataBaseName);
    if (mysql_query(conn, query))
    {
        Logger::systemlog(LOG_ERR, "%s", mysql_error(conn));
        mysql_close(conn);
        return;
    }
    memset(query, NULL, sizeof(query));

    sprintf(query, "USE %s", dataBaseName);
    mysql_query(conn, query);
    memset(query, NULL, sizeof(query));

    Logger::systemlog(LOG_INFO, "DataBase \"%s\" has been succesfully initialized!", dataBaseName);

    curl = curl_easy_init();
    
    if (!curl) {
        Logger::systemlog(LOG_ERR, "Failed to initialize curl session");
        return;
    }

    curl_easy_setopt(curl, CURLOPT_POST, 1L);

    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/x-www-form-urlencoded");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    Logger::systemlog(LOG_INFO, "Curl session has been succesfully initialized!");

    truncateRemoteTables();
}

SQLDataBase::~SQLDataBase() {
    mysql_close(conn);
    curl_easy_cleanup(curl);
}

void SQLDataBase::createLocalTable(char *tableName)
{
    char query[512] = {};

    sprintf(query, "DROP TABLE IF EXISTS %s", tableName);
    if (mysql_query(conn, query)) {
        Logger::systemlog(LOG_ERR, "%s", mysql_error(conn));
        mysql_close(conn);
        return;
    }
    memset(query, NULL, sizeof(query));

    if (strstr(tableName, CONROL_PANEL_TABLE_NAME) != NULL) {
        strcat(query,
            "CREATE TABLE `ControlPanel` ("
            "`id` INT PRIMARY KEY AUTO_INCREMENT,"
            "`Temperature` FLOAT DEFAULT NULL,"
            "`Humidity` FLOAT DEFAULT NULL,"
            "`Pressure` FLOAT DEFAULT NULL,"
            "`Time stamp` timestamp NOT NULL DEFAULT current_timestamp() ON UPDATE current_timestamp()"
            ") ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci");
    } else if ((strstr(tableName, GASBOIL_CONTR_TABLE_NAME) != NULL)) {
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
    } else if ((strstr(tableName, WEATH_STAT_TABLE_NAME) != NULL)) {
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
}

void SQLDataBase::sendDataToLocalTable(char *tableName, void *data) {
    char query[256] = {0};

    if (strstr(tableName, CONROL_PANEL_TABLE_NAME) != NULL) {
        controlPanelDB dbData;
        memcpy(&dbData, data, sizeof(dbData));
        sprintf(query,
            "INSERT INTO `%s` (`Temperature`, `Humidity`, `Pressure`) VALUES(%.2f,%.2f,%.2f)",
            CONROL_PANEL_TABLE_NAME, dbData.temperature, dbData.humidity, dbData.pressure);
    } else if ((strstr(tableName, GASBOIL_CONTR_TABLE_NAME) != NULL)) {
        gasBoilControlDB dbData;
        memcpy(&dbData, data, sizeof(dbData));
        sprintf(query,
            "INSERT INTO `%s` (`Status`, `Set temperature`, `Current temperature`, `Temperature range`, `Setpoint source`) VALUES(%d,%.2f,%.2f,%.2f,%d)",
            GASBOIL_CONTR_TABLE_NAME, dbData.status, dbData.setpointTemp, dbData.currentTemp, dbData.tempRange, dbData.setpointSource);
    } else if ((strstr(tableName, WEATH_STAT_TABLE_NAME) != NULL)) {
        weatherStationDB dbData;
        memcpy(&dbData, data, sizeof(dbData));
        sprintf(query,
            "INSERT INTO `%s` (`Temperature`, `Humidity`, `Wind speed`, `Wind direction`, `Rainfall`) VALUES(%.2f,%.2f,%.2f,%d,%.2f)",
            WEATH_STAT_TABLE_NAME, dbData.temperature, dbData.humidity, dbData.windSpeed, dbData.windDirect, dbData.rainFall);
    } else {
        return;
    }

    if (mysql_query(conn, query)) {
        Logger::systemlog(LOG_ERR, "%s", mysql_error(conn));
        mysql_close(conn);
        return;
    }
    memset(query, NULL, sizeof(query));
}

void SQLDataBase::sendDataToRemoteTable(char *tableName) {
    static int prevRowID = 0;
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
        if (atoi((char*)row[0]) > prevRowID)
        {
            if (strstr(tableName, CONROL_PANEL_TABLE_NAME) != NULL) {
                curl_easy_setopt(curl, CURLOPT_URL, CONTROL_PANEL_POST_URL);
                sprintf(postData,
                    "api_key=%s&Temperature=%.2f&Humidity=%.2f&Pressure=%.2f&TimeStamp=%s",
                    API_KEY, atof((char*)row[1]), atof((char*)row[2]), atof((char*)row[3]), row[4]);
            } else if ((strstr(tableName, GASBOIL_CONTR_TABLE_NAME) != NULL)) {
                curl_easy_setopt(curl, CURLOPT_URL, GAS_BOIL_CONTR_POST_URL);
                sprintf(postData,
                    "api_key=%s&Status=%d&SetTemperature=%.2f&CurrentTemperature=%.2f&TemperatureRange=%.2f&SetpointSource=%d&TimeStamp=%s",
                    API_KEY, atoi((char*)row[1]), atof((char*)row[2]), atof((char*)row[3]), atof((char*)row[4]), atoi((char*)row[5]), row[6]);
            } else if ((strstr(tableName, WEATH_STAT_TABLE_NAME) != NULL)) {
                curl_easy_setopt(curl, CURLOPT_URL, WEATHERSTATION_POST_URL);
                sprintf(postData,
                    "api_key=%s&Temperature=%d&Humidity=%.2f&WindSpeed=%.2f&WindDirection=%d&RainFall=%.2f&TimeStamp=%s",
                    API_KEY, atof((char*)row[1]), atof((char*)row[2]), atof((char*)row[3]), atoi((char*)row[4]), atof((char*)row[5]), row[6]);
            } else {
                return;
            }

            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postData);

            CURLcode res = curl_easy_perform(curl);

            if(res != CURLE_OK) {
                Logger::systemlog(LOG_ERR, "CURL POST request failed:%s", curl_easy_strerror(res));
                return;
            }
            memset(postData, NULL, sizeof(postData));
            prevRowID++;
        }
    }
}

void SQLDataBase::truncateRemoteTables() {
    char postData[32] = {0};

    sprintf(postData, "api_key=%s", API_KEY);

    curl_easy_setopt(curl, CURLOPT_URL, TRUNCATETABLES_POST_URL);

    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postData);

    CURLcode res = curl_easy_perform(curl);

    if(res != CURLE_OK) {
        Logger::systemlog(LOG_ERR, "CURL POST request failed:%s", curl_easy_strerror(res));
    }
}
