#pragma once

#include "gui.hpp"
#include "component.hpp"

struct Artnet;

struct ArtnetWindow : GUI::Window {

  Artnet *artnet;

  int  cell_min = 0, cell_max = 255, cells_count = 48;

   int uni_current = 3;

   std::vector<int> universes;

  ~ArtnetWindow();

  StringsBuffer options;

  ArtnetWindow(Artnet * artnet);

  void updateList(int uni);

  void draw() override;

};
