#ifndef _LOG_HPP
#define _LOG_HPP

#include <iostream>
#include <fstream>
#include <mutex>
#include <ctime>

#include <stdarg.h>
#include <string.h>
#include <syslog.h>

using namespace std;

class Logger {
    public:
        Logger() {
            openlog("Undefined", LOG_PID, LOG_USER);
        }
        Logger(const char *name) {
            openlog(name, LOG_PID, LOG_USER);
        }
        Logger(const char *name, const char* fname, const char* mode) {
            openlog(name, LOG_PID, LOG_USER);
            logfile = fopen(fname, mode);
            if(logfile == NULL) {
                syslog(LOG_ERR, "Unable to open log file.");
            } else {
                isLogFile = true;
            }
        }
        Logger(string ident, int option, int facility) {
            openlog(ident.c_str(), option, facility);
        }
        void systemlog(const int loglevel, const char *format, ...) {
            va_list plist;
            va_start(plist, format);
            vsyslog(loglevel, format, plist);
            va_end(plist);
            if (isLogFile) {
                time_t ttime = time(NULL);
                string cppFormat(format, strlen(format));
                cppFormat = string(ctime(&ttime), strlen(ctime(&ttime)) - 1) + ": " + cppFormat + "\n";
                va_start(plist, format);
                mut.lock();
                vfprintf(logfile, cppFormat.c_str(), plist);
                mut.unlock();
                va_end(plist);
                fflush(logfile);
            }
        }
        ~Logger() {
            closelog();
            if (isLogFile)
                fclose(logfile);
        }
    private:
        bool isLogFile = false;
        FILE *logfile;
        mutex mut;
};

#endif
