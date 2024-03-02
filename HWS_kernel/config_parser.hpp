#ifndef _CONFIG_PARSER_HPP
#define _CONFIG_PARSER_HPP
#include <iostream>
#include <fstream>
#include <list>

typedef std::pair<std::string, std::list<std::pair<std::string, std::string>>> ConfigEntry;

class Parser {
    public:
        Parser(const std::string &path);
        ~Parser();
        bool getIsOpened();
        std::list<ConfigEntry> parsed_configs;
    private:
        bool isOpened;
        std::fstream iniFile;
        std::string path;
};

#endif