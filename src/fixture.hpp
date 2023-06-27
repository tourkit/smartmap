#ifndef FIXTURE_H
#define FIXTURE_H
	
#include "device/artnet.hpp"
#include "attribute.hpp"

struct Fixture {

  Attribute* attributes;

  Attribute* attr(const char* t);

  int DMXpatch(int universe, int address, std::vector<std::string> dmx_chart);

};
#endif