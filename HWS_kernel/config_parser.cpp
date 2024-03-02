#include "config_parser.hpp"

Parser::Parser(const std::string &path) {
    iniFile.open(path);
    if(iniFile.fail()) {
        isOpened = false;
        return;
    } else {
        isOpened = true;
    }
    std::list<std::string> tmpList;
    std::string tmpStr;
    for (getline(iniFile, tmpStr); !iniFile.eof(); getline(iniFile, tmpStr)) {
        tmpList.push_back(tmpStr);
    }
    for (const auto &el : tmpList) {
        if(el == "" || el.at(0) == '#') continue;
        if (el.at(0) == '[' && el.back() == ']') {
            ConfigEntry n;
            n.first = el;
            parsed_configs.push_back(n);
        } else if (parsed_configs.size() > 0) {
            std::pair<std::string, std::string> t_pair;
            size_t pos = el.find('=');
            if (pos == std::string::npos || pos == el.length() - 1)
                continue;
            else {
                t_pair.first = el.substr(0, pos);
                t_pair.second = el.substr(pos + 1);
            }
            parsed_configs.back().second.push_back(t_pair);
        }
    }
}

bool Parser::getIsOpened() {
    return isOpened;
}

Parser::~Parser() {
    if (iniFile.is_open()) {
        iniFile.close();
    }
}