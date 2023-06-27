#include "fixture.hpp"  
#include "attribute.hpp"  
#include "smartmap.hpp"  

Attribute* Fixture::attr(const char* t) { 
    
    attributes = new Attribute("Attributes", {

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
    
    return attributes->child(t); }

int Fixture::DMXpatch(int universe, int address, std::vector<std::string> dmx_chart)  {

    --address;
    DMXUniverse* uni = sm.artnet.data[--universe];

    int offset = address;

    for ( int i = 0; i < dmx_chart.size(); ++i)  {
  
      char* finestr = strrchr(&dmx_chart[i][0], ' ');

      if (finestr && !strcmp(finestr," --FINE")) {
      
        dmx_chart[i].resize(dmx_chart[i].length()-strlen(finestr)); // suffisant ?

        uni->links.push_back(new DMXAttributeFine(address));

        address+=2;

      }else{

        uni->links.push_back(new DMXAttribute(address));
       
        address+=1;
      
      } 
        
      uni->links.back()->linkTo(attributes->child(&dmx_chart[i][0]));

    }


    return address-offset;

  }

