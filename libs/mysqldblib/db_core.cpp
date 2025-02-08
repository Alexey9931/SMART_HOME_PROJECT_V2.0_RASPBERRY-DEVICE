#include "sql_db.hpp"

SQLDataBase::SQLDataBase(char *serverName,
                         char *user,
                         char *password,
                         char *dataBaseName) :
    Logger("smhomekernel", "/usr/local/sm_home/smhomekernel.log", "a") {
    
    conn = mysql_init(NULL);
    if (conn == NULL) {
        Logger::systemlog(LOG_ERR, "%s", mysql_error(conn));
        mysql_close(conn);
        return;
    }

    if (!mysql_real_connect(conn, serverName, user, password, dataBaseName, 0, NULL, 0))
    {
        Logger::systemlog(LOG_ERR, "%s", mysql_error(conn));
        mysql_close(conn);
        return;
    }

    // if (mysql_query(con, "CREATE DATABASE testdb"))
    // {
    //     Logger::systemlog(LOG_ERR, "%s", mysql_error(con));
    //     mysql_close(conn);
    //     return;
    // }
}

SQLDataBase::~SQLDataBase() {
    mysql_close(conn);
}

// int main() {

//     char *server = "<your mysql server name>";
//     char *user = "<your mysql username>";
//     char *password = "<your mysql password>"; 
//     char *database = "<your mysql database name>";
//     MYSQL *conn = mysql_init(NULL);
//     if (!mysql_real_connect(conn, server, user, password, database, 0, NULL, 0))
//     {
//         fprintf(stderr, "%s\n", mysql_error(conn));
//         return(1);
//     }

//     /* send SQL query */
//     if (mysql_query(conn, "show tables"))
//     {
//         fprintf(stderr, "%s\n", mysql_error(conn));
//         return(1);
//     }

//     MYSQL_RES *res = mysql_use_result(conn);

//     printf("Tables in mysql database:\n");
//     MYSQL_ROW row;
//     while ((row = mysql_fetch_row(res)) != NULL){
//         printf("%s \n", row[0]);
//     }

//     mysql_free_result(res);
//     mysql_close(conn);
//     return 0;
// }