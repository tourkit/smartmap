#pragma once

template <typename T>
static void ADD_UNIQUE( std::vector<T>& list, T n) {

    for (auto x : list) if (x == n) return;

    list.push_back(n);

}


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
