#include "test.hpp"
#include "log.hpp"
#include "node.hpp"
#include "directory.hpp"
#include <cstring>
#include <typeinfo>
#include <functional>
#include <vector>

#include "engine.hpp"
#include "buffer.hpp"



struct Cmp {

    const char* name;

    static Cmp& getInstance() { static Cmp instance;  return instance; }

private:

    Cmp(std::string name = ""){ PLOGD << name<<"!"; }

    Cmp(const Cmp&) = delete;
    Cmp& operator=(const Cmp&) = delete;

    ~Cmp(){}
};

struct Comp {
    Comp(std::string name = "") {  }
    operator int() { return 1; }

    // int operator<<() { return 1; }
};


Test::Test(){

    Cmp::getInstance();

    // Comp("wow");



    auto &ubos = *Engine::getInstance().tree.add(new Node("UBOS"));
    auto &buffer = *(Buffer*)ubos.add(new Buffer("dynamic_ubo"));

    buffer.add(new Buffer::Object("TOUT",{"float"},1));



 
}