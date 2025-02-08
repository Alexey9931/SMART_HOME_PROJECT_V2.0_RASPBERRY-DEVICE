#ifndef _SQL_DB_HPP
#define _SQL_DB_HPP

#include <mysql.h>
#include <stdio.h>
#include <stdlib.h>
#include "log.hpp"

class SQLDataBase : public Logger {
    public:
        SQLDataBase(char *serverName, char *user,
                    char *password, char *dataBaseName);
        ~SQLDataBase();
        MYSQL *conn;
};

#endif