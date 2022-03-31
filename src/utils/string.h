#ifndef STRING_H
#define STRING_H

#pragma once
#include <string>
#include <vector>
#include <regex>

//include for regex_search
// using namespace std;

struct String {

    String() {}
    
    static std::vector<std::string> find(std::string cstr, const char * regex) {

        std::vector<std::string> results;

        std::regex re(regex);
        std::smatch m;

        while (std::regex_search(cstr, m, re)) {
            results.push_back(m.str());
            cstr = m.suffix().str().c_str();
        }

        return results;
    }

    static std::vector<std::string> split(const std::string& s, char delim) {
        std::vector<std::string> elems;
        std::stringstream ss(s);
        std::string item;
        while (std::getline(ss, item, delim)) {
            elems.push_back(item);
        }
        return elems;
    }

    // ~string();


};

#endif