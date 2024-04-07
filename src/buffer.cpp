
#include "buffer.hpp"
#include "instance.hpp"
// #include "engine.hpp"


Instance Buffer::operator[](std::string name) { return Instance{this,0,this}[name]; }
Instance Buffer::operator[](int id) { return Instance{this,0,this}[id]; }


Buffer::Buffer(std::string name) : Struct(name) {

    data.reserve(MAX_SIZE);

    memset(data.data(),0,data.size());

}

void upload() { }

Member* Buffer::copy(Member* x) {

    if (!x) {x = new Buffer();}

    Struct::copy(x);

    ((Buffer*)x)->data = data;

    return x;

}

void Buffer::update() {

    Struct::update();

    if (data.size() > MAX_SIZE) { PLOGW << "data.size() > MAX_SIZE"; }

    data.resize(footprint_all());

}

void Buffer::printData(int max) {

    if (!max) max = data.size();

    std::string str;

    for (auto i = 0 ; i < max; i++) str += std::to_string(data[i]) + " ";

    PLOGD << str;

}

void Buffer::remap(Buffer& src_buffer, Member* src_member, Member* this_member , int src_offset, int this_offset) {

    if (!src_member) src_member = &src_buffer;

    if (!this_member) this_member = this;

    for (int i = 0 ; i < ( src_member->quantity() < this_member->quantity() ? src_member->quantity() :  this_member->quantity() ); i ++) {

        int src_offset_ = src_offset + src_member->eq(i);

        for (auto src_member_ : src_member->members) {

            Member* found = nullptr;

            int this_offset_ = this_offset + this_member->eq(i);

            for (auto this_member_ : this_member->members) {

                if (

                    !strcmp(src_member_->name().c_str(), this_member_->name().c_str())

                    && src_member_->type() == this_member_->type()

                ) { found = this_member_; break; }

                else this_offset_ += this_member_->size();

            }

            if (!found) { PLOGV << "couldnt find " << src_member_->name(); continue; }

            remap(src_buffer, src_member_, found, src_offset_, this_offset+this_offset_);

            if (found->typed()) {

                PLOGV  << src_offset_ << " -> "  << this_offset_ << " " << src_member->name() << "::" << src_member_->name() << "(" << src_member_->size() << ")";

                memcpy(&data[src_offset_], &src_buffer.data[this_offset_],found->size());

            }

            src_offset_ += src_member_->footprint();

        }

    }

}
