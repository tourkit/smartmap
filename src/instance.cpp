#include "instance.hpp"
#include "struct.hpp"
#include "buffer.hpp"

bool Instance::exist(){

    if (offset == -1) return false;

    return true;

}

Instance::Instance(Buffer* buff, uint32_t offset, Member* member) : buff(buff), offset(offset), member(member) { if (!this->member) this->member = buff; }
Instance Instance::operator[](std::string name) {

    auto offset = this->offset;

    Member* found = nullptr;

    for (auto &m : member->members) {

        if (!(strcmp(m->name().c_str(),name.c_str()))) {

            found = m;

            break;

        }

        offset += m->footprint_all();

    }

    if (!found) PLOGE << "\"" << name << "\" does not exist in " << member->name();

    return Instance{buff,offset,found};

}
Instance Instance::operator[](int id) {

    auto offset = this->offset;
    auto member = this->member;

    if (id >= member->members.size()) {PLOGE << id << "exceed"; exit(0);}

    for (int i = 0 ; i < id; i ++ ){

        auto &m = member->members[i];

        offset += m->footprint_all();

    }

    member = member->members[id];

    return Instance{buff,offset,member};

}

Instance Instance::eq(int id) {

    if (!member || id >= member->quantity()) {PLOGE<<"Not that many quantity exist";return *this;}

    uint32_t new_offset = offset + member->footprint() * (id-eq_id);

    eq_id = id;

    return Instance{buff, new_offset ,member};

}

void Instance::setDefault(Member* toset, int offset) {

    if (!toset) toset = member;

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

void Instance::remap(void* src,  std::vector<Attribute> attributes, int quantity) {

  auto data = (uint8_t*)src;

    for (int offset = 0; offset < quantity; offset++) {

        auto size = member->size();

        auto pos = (offset*size);
        pos /=sizeof(float);

        for (int i = 0; i < attributes.size(); i++) {

            float target = 0;

            auto c = attributes[i].combining;

            if (c==1) target      = data[0]/255.0;
            else if (c==2) target = ((data[0] << 8) | data[1])/65535.0f;
            else if (c==3) target = ((data[0] << 16) | (data[1] << 8) | data[2])/16777215.0f;
            else if (c==4) target = ((data[0] << 24) | (data[1] << 16) | (data[2] << 8) | data[3])/4294967295.0f;

            // range remap
            if (attributes[i].active && c > 0) *((float*)this->data()+i+pos) = (target * (attributes[i].max - attributes[i].min)) + attributes[i].min;

            data += c;

        };

    }



}

void Instance::remap(Instance* src) {

    std::vector<std::pair<Member*,int>> list;
    src->member->each( [&](Member* m, int offset) { list.push_back({m,offset}); } );

    int count = 0;
    member->each( [&](Member* m, int offset) {

        if (!(count<list.size())) return;

        if (m->type() == list[count].first->type()) {

            memcpy(data()+offset, src->data()+list[count].second, m->size());

        }

        count++;

    });



}

Instance Instance::push(void* ptr, size_t size) {

    PLOGV << "new " << member->name() << " in " << buff->name() ;

    member->quantity(member->quantity()+1);

    auto inst = eq(member->quantity()-1);

    if (!ptr) { inst.setDefault(); }

    else {

        if (!size) size = member->size();

        inst.set(ptr,size);

    }

    return inst;

}
