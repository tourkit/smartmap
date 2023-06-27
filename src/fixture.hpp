#ifndef FIXTURE_H
#define FIXTURE_H
	
#include "device/artnet.hpp"
#include "attribute.hpp"

struct Fixture {

  Attribute* attributes = new Attribute("Attributes", {

    new Attribute("Color", {

      new Attribute("Red", 1),
      new Attribute("Green", 1),
      new Attribute("Blue", 1)
      
    }),

    new Attribute("Dimmer", 0),

    new Attribute("Position", {

      new Attribute("Horizontal", 0, -1, 1),
      new Attribute("Vertical", 0, -1, 1)
      
    }),

    new Attribute("Focus", {

      new Attribute("Horizontal", 1),
      new Attribute("Vertical", 1)
      // new Attribute(0,"Focus")
      
    }),

    new Attribute("Gobo", {

      new Attribute("ID",0),
      new Attribute("Fx1",0),
      new Attribute("Fx2",0),
      new Attribute("Fx3",0)
      
    }),


    new Attribute("Strobe",0),
    new Attribute("Feedback",1),
    new Attribute("Orientation",0 ,-1, 1),

    
    new Attribute("Ratio", 600./300.)
    // new Attribute(0,"Control")

  });

  Attribute* attr(const char* t) { return attributes->child(t); }

  int DMXpatch(int universe, int address, std::vector<std::string> dmx_chart) {

    --address;
    // DMXUniverse& uni = Artnet::data[--universe];

    int offset = address;

    // for ( int i = 0; i < dmx_chart.size(); ++i)  {
  
    //   char* finestr = strrchr(&dmx_chart[i][0], ' ');

    //   if (finestr && !strcmp(finestr," --FINE")) {
      
    //     dmx_chart[i].resize(dmx_chart[i].length()-strlen(finestr)); // suffisant ?

    //     uni.links.push_back(new DMXAttributeFine(address));

    //     address+=2;

    //   }else{

    //     uni.links.push_back(new DMXAttribute(address));
       
    //     address+=1;
      
    //   } 
        
    //   uni.links.back()->linkTo(attributes->child(&dmx_chart[i][0]));

    // }


    return address-offset;

  }

};
#endif