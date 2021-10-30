#ifndef FIXTURE_H
#define FIXTURE_H
	
#include "device/artnet.hpp"
#include "attribute.hpp"

struct Fixture {

  Attribute* attributes = new Attribute({

    new Attribute(0,"Dimmer"),

    new Attribute({

      new Attribute(1,"Red"),
      new Attribute(1,"Green"),
      new Attribute(1,"Blue")
      
      },"Color"
    
    ),

    new Attribute({

      new Attribute(0,"Horizontal"),
      new Attribute(0,"Vertical")
      
      },"Position"
    
    ),

    new Attribute({

      new Attribute(1,"Horizontal"),
      new Attribute(1,"Vertical"),
      new Attribute(1,"Focus")
      
      },"Focus"
    
    ),

    new Attribute({

      new Attribute(0,"Gobo"),
      new Attribute(0,"Fx1"),
      new Attribute(0,"Fx2"),
      new Attribute(0,"Fx3")
      
      },"Gobo"
    
    ),

    new Attribute({

      new Attribute(0,"Strobe"),
      new Attribute(1,"Feedback"),
      new Attribute(1,"Orientation")
      
      },"Beam"
    
    ),

    new Attribute(0,"Control")

  });

  Attribute* attr(const char* t) { return attributes->child(t); }

  void DMXpatch(int universe, int address, std::vector<std::string> dmx_chart) {

    --address;
    DMXUniverse& uni = ArtnetDevice::data[--universe];

    for ( int i = 0; i < dmx_chart.size(); ++i)  {
  
      char* finestr = strrchr(&dmx_chart[i][0], ' ');

      if (finestr && !strcmp(finestr," --FINE")) {
      
        dmx_chart[i].resize(dmx_chart[i].length()-strlen(finestr)); // suffisant ?

        uni.links.push_back(new DMXAttributeFine(address));

        address+=2;

      }else{

        uni.links.push_back(new DMXAttribute(address));
       
        address+=1;
      
      } 
        
      uni.links.back()->linkTo(attributes->child(&dmx_chart[i][0]));

    }

  }

};
#endif