
#include "buffer.hpp"
#include "instance.hpp"
// #include "engine.hpp"


Instance Buffer::operator[](std::string name) { return Instance{this}[name]; }
Instance Buffer::operator[](int id) { return Instance{this}[id]; }

Buffer::Buffer(std::string name) : Struct(name) {

    data.reserve(MAX_SIZE);

    memset(data.data(),0,data.size());

}

Buffer::Buffer(const Buffer& other) : Struct( other ) , data( other.data ) { }

void Buffer::upload() { }

void  Buffer::pre_change() {

    if (!data.size()) return;

    bkp = copy();

    PLOGV << "bkp " << name() ;

}

void  Buffer::post_change(std::vector<Member*> addeds) {

    for (auto added : addeds) { // only to set default I guess

        each([&](Instance& inst) {

            auto *m = inst.def();

            int offset = inst.offset;

            if (m == added) {

                if (m->isRef()) m = m->members[0];

                for (auto m_ : m->members) {

                    if (m_->default_val_ptr) {

                            // if (m_->type() == typeid(glm::vec2)) {

                            //     PLOGD  << "NID TOU SAITE : " << m_->name() << " @ " << offset << " - val VEEEC2: " << (*(glm::vec2*) m_->default_val_ptr).x ;
                            // }

                            // if (m_->type() == typeid(float)) {

                            //     PLOGD  << "NID TOU SAITE : " << m_->name() << " @ " << offset << " - val : " << *(float*) m_->default_val_ptr ;

                            // }

                        memcpy(&data[offset], m_->default_val_ptr, m_->size());

                    }

                    offset += m_->footprint_all();

                }

            }

        });

    }

    if (!bkp) return;

    PLOGV << "remap " << bkp->name();

    remap(*bkp);

    bkp->hard_delete();

    delete bkp;

    bkp = nullptr;

    upload();

}

Buffer* Buffer::copy() { return new Buffer(*this); }

void Buffer::update() {

    Struct::update();

    if (data.size() > MAX_SIZE) { PLOGE << "data.size() > MAX_SIZE"; }

    data.resize(footprint_all());

    memset( data.data(), 0, data.size() );

    each([&](Instance& inst) {

        for (auto &x : inst.def()->instances) {

            if (x.get()->buff == inst.buff && x.get()->stl == inst.stl) {
                // int y = inst.eq_id;
                x.get()->offset = inst.offset;

            }

        }

    });

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

            auto thiseq = this_member->eq(i);

            int this_offset_ = this_offset + thiseq;

            for (auto this_member_ : this_member->members) {

                if (

                    !strcmp(src_member_->name().c_str(), this_member_->name().c_str())

                    //&& src_member_->type() == this_member_->type() // dont think thats relevent, fuxks

                ) { found = this_member_; break; }

                else this_offset_ += this_member_->footprint_all();

            }

            if (!found) { src_offset_ += src_member_->footprint(); PLOGE << "couldnt find " << src_member_->name() << " in " << src_buffer.name(); continue; }

            remap(src_buffer, src_member_, found, src_offset_, this_offset_);

            if (found->isData()) {

                // PLOGV  << src_member->name() << "::" << src_member_->name() << "@" << src_offset_ << " -> "  << " " << this_member->name() << "::" << found->name()  << "@" <<  this_offset_<< " - " << src_member_->size() << " : " << (unsigned int)*(char*)&src_buffer.data[src_offset_] << " -> " << *(float*)&data[this_offset_];

                memcpy(&data[this_offset_], &src_buffer.data[src_offset_],found->size());

            }

            src_offset_ += src_member_->footprint_all();

        }

    }

}
