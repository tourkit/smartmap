#include "dmx.hpp"
#include "struct.hpp"
#include "log.hpp"
#include "struct.hpp"
#include "artnet.hpp"

DMX::DMX(int id) : id(id) { memset(&data[0],0,512); PLOGD << "universe " << id;  }

void DMX::update() {

    for (auto r : remaps) r.update();

    for (auto cb:callbacks) cb(this);

}

DMX::Remap::Remap(void *src, void *dst, Member *s, uint16_t q) {

    this->src = (char*)src;
    this->dest = (char*)dst;
    this->s = s;

    quantity = s->quantity();

    extract(s);


}

uint8_t DMX::Remap::get8(uint8_t* data) { return GMAui2f[*data]; }
