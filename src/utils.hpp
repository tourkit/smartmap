#pragma once

#include <vector>
#include "log.hpp"

template <typename T>
static bool ADD_UNIQUE( std::vector<T>& list, T n) {

    for (auto x : list) if (x == n) return false;

    list.push_back(n);

    return true;

}

template <typename T>
struct Pool {

    std::vector<T*> list;

    Pool(int size = 0) { list.reserve(size); }

    void add(T* x) { ADD_UNIQUE<T*>(list, x); if (list.size()>20) PLOGW << "viens on en parle"; }

    void remove(T* x) { for (int i = 0; i < list.size(); ) if (list[i] == x) list.erase(list.begin() + i); else i++;} // nbot sure


};

static std::vector<std::string> split(std::string s, std::string delimiter = "::") {

    std::vector<std::string> out;

    size_t pos = 0;
    std::string token;
    while ((pos = s.find(delimiter)) != std::string::npos) {
        token = s.substr(0, pos);
        out.push_back(token);
        s.erase(0, pos + delimiter.length());
    }
    out.push_back(s);

    return out;

}
