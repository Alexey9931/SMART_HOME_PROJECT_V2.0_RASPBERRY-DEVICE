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

    Logger::systemlog(LOG_INFO, "DataBase \"%s\" has been succesfully inited!", dataBaseName);
}

SQLDataBase::~SQLDataBase() {
    mysql_close(conn);
}

void SQLDataBase::createTable(char *tableName)
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

void SQLDataBase::sendData(char *tableName, void *data)
{
    char query[128] = {};

    if (strstr(tableName, CONROL_PANEL_TABLE_NAME) != NULL) {
        controlPanelDB dbData;
        memcpy(&dbData, data, sizeof(dbData));
        sprintf(query,
            "INSERT INTO ControlPanel(Temperature, Humidity, Pressure) VALUES(%.2f,%.2f,%.2f)",
            dbData.temperature, dbData.humidity, dbData.pressure);
    } else if ((strstr(tableName, GASBOIL_CONTR_TABLE_NAME) != NULL)) {

    } else if ((strstr(tableName, WEATH_STAT_TABLE_NAME) != NULL)) {

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
