#include "instance.hpp"
#include "struct.hpp"
#include "buffer.hpp"


Remap::Remap(Instance* src , Instance* dst, int quantity) : src(src) ,dst(dst) , quantity(quantity) { }

void Remap::update() {

    int size = src->def()->size();

    if (dst->def()->size()<size) size = dst->def()->size();

    memcpy(dst->data(), src->data(), quantity * size);

}


// void DMXRemap::update() {

//   auto data = (uint8_t*)src;

//     for (int offset = 0; offset < quantity; offset++) {

//         auto size = def()->size();

//         auto pos = (offset*size);
//         pos /=sizeof(float);

//         for (int i = 0; i < attributes.size(); i++) {

//             float target = 0;

//             auto c = attributes[i].combining;

//             if (c==1) target      = data[0]/255.0;
//             else if (c==2) target = ((data[0] << 8) | data[1])/65535.0f;
//             else if (c==3) target = ((data[0] << 16) | (data[1] << 8) | data[2])/16777215.0f;
//             else if (c==4) target = ((data[0] << 24) | (data[1] << 16) | (data[2] << 8) | data[3])/4294967295.0f;

//             // range remap
//             if (attributes[i].active && c > 0) *((float*)this->data()+i+pos) = (target * (attributes[i].max - attributes[i].min)) + attributes[i].min;

//             data += c;

//         };

//     }

// }


bool Instance::exist(){

    if (offset == -1) return false;

    return true;

}

Instance::Instance(Buffer* buff, uint32_t offset, std::vector<Member*> stl) : buff(buff), offset(offset), stl(stl) { }

Instance Instance::operator[](std::string name) {

    auto offset = this->offset;

    Member* found = nullptr;

    for (auto &m : def()->members) {

        if (!(strcmp(m->name().c_str(),name.c_str()))) {

            found = m;

            break;

        }

        offset += m->footprint_all();

    }

    if (!found) PLOGE << "\"" << name << "\" does not exist in " << def()->name();
    auto stl_ = stl;
    stl_.push_back(found);
    return Instance{buff,offset,stl_};

}
Instance Instance::operator[](int id) {

    auto offset = this->offset;
    auto member = this->def();

    if (id >= def()->members.size()) {PLOGE << id << "exceed"; exit(0);}

    for (int i = 0 ; i < id; i ++ ){

        auto &m = def()->members[i];

        offset += m->footprint_all();

    }

    member = member->members[id];

    auto stl_ = stl;
    stl_.push_back(member);

    return Instance{buff,offset,stl_};

}

Instance Instance::eq(int id) {

    if (!stl.size() || id >= def()->quantity()) {PLOGE<<"Not that many quantity exist";return *this;}

    uint32_t new_offset = offset + def()->footprint() * (id-eq_id);

    eq_id = id;

    return Instance{buff, new_offset ,stl};

}

void Instance::setDefault(Member* toset, int offset) {

    if (!toset) toset = def();

    for (auto x : toset->members) {

        setDefault(x, offset);

        offset += x->footprint_all();

    }

        // PLOGD << "------------>set " << toset->name();
    if (toset->default_val_ptr) {
        // PLOGD << "------------>set " << toset->name() << " to default " << *(float*)toset->default_val_ptr << " @ " << offset;


        memcpy(data()+offset, toset->default_val_ptr, toset->size());

    }

}

Instance Instance::push(void* ptr, size_t size) {

    PLOGV << "new " << def()->name() << " in " << buff->name() ;

    def()->quantity(def()->quantity()+1);

    auto inst = eq(def()->quantity()-1);

    if (!ptr) { inst.setDefault(); }

    else {

        if (!size) size = def()->size();

        inst.set(ptr,size);

    }

    return inst;

}


std::string Instance::stl_name() {

    std::string str;

    for (auto x : stl)  str += x->name() + "::";

    if (str.length()) str.resize(str.length()-2);

    else str = buff->name();

    return str;

}
