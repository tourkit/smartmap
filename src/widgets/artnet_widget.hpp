#pragma once

#include "gui.hpp"
#include "component.hpp"

struct Artnet;

struct ArtnetWidget : GUI::Window {

  Artnet *artnet;

  int  cell_min = 0, cell_max = 255, cells_count = 48;

   int uni_current = 3;

   std::vector<int> universes;

  ~ArtnetWidget();

  StringsBuffer options;

  ArtnetWidget(Artnet * artnet);

  void updateList(int uni);

  void draw() override;

};


  struct ArtnetWidget2 : GUI::Window {

      Artnet *artnet;

      ArtnetWidget2(Artnet *artnet) : GUI::Window("Artnet"), artnet(artnet) { 

          // engine.list.push_back(new Stack::Action([](){}));
      
      }

      void draw() override {}

  };