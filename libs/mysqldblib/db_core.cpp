#include "sql_db.hpp"
#include <type_traits>

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

    controlPanelTable = new SQLTable<controlPanelDB>{CONROL_PANEL_TABLE_NAME, conn, curl};
    gasBoilControlTable = new SQLTable<gasBoilControlDB>{GASBOIL_CONTR_TABLE_NAME, conn, curl};
    weatherStationTable = new SQLTable<weatherStationDB>{WEATH_STAT_TABLE_NAME, conn, curl};
}

SQLDataBase::~SQLDataBase() {
    mysql_close(conn);
    curl_easy_cleanup(curl);

    delete controlPanelTable;
    delete gasBoilControlTable;
    delete weatherStationTable;
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
