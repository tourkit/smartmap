/*

    SMARTMAP (beta)

                        */


#include "smartmap.hpp"

#include "debug.hpp"
Debug debug;

int main() {  

    sm.createFixtures(1);
    sm.artnet->universes[0].callback = [&]() { sm.artnet->universes[0].floatify(&sm.fixtureUBO->data, std::vector<uint8_t>{2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2}); sm.fixtureUBO->update(); };

    while(true) sm.window->render([&]() {

        debug.run();

        sm.artnet->run();



        sm.outFB->clear(); // thus bind

        sm.passBuf->bind();
        sm.shader->use();

        glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_SRC_COLOR);
        sm.quadA->draw(sm.MATS); // quantity is instances count in shader 
        glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
        sm.quadB->draw(sm.MATS); // quantity is instances count in shader 

        // sm.passBuf->copy(*sm.outBuf);

        // glBindImageTexture(0, *sm.outBlur, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);
        // glBindImageTexture(1, *sm.outBuf, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA8);
        // sm.blur_x->use(sm.FW*.5/16,sm.FH*.5/16);
        // sm.blur_y->use(sm.FW*.5/16,sm.FH*.5/16);
        // glMemoryBarrier( GL_ALL_BARRIER_BITS ); 

        sm.winFB->clear(); 
        
        sm.outBuf->bind();
        sm.shader->use();
        sm.quadA->draw();

        sm.gui->draw2();  

    }); 

 
}