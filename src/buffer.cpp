
#include "buffer.hpp"
#include "instance.hpp"
// #include "engine.hpp"


Instance Buffer::operator[](std::string name) { return Instance{this}[name]; }
Instance Buffer::operator[](int id) { return Instance{this}[id]; }

Buffer::~Buffer() {

    buffers.erase(this);

}

Buffer::Buffer(std::string name) : Struct(name) {

    structs.erase(this);
    buffers.insert(this);

    data.reserve(MAX_SIZE);

    memset(data.data(),0,data.size());

}

Buffer::Buffer(const Buffer& other) : Struct( other ) , data( other.data ) { }

void Buffer::upload() { }

void  Buffer::pre_change() { // return; // aka padbakeuuup

    if (!data.size()) return;

    bkp = copy();

    PLOGV << "bkp " << name() ;

}

Instance Buffer::add(Member* m) { 
    
    Struct::add(m); 
    
    return Instance(this).find(m); 
    
}

void  Buffer::post_change(std::vector<NewMember> addeds) { //return; // aka padbakeuuup

    update(); 

    for (auto added : addeds) {

        Instance(this).each([&](Instance& inst) {

            bool found = false;
            if (inst.def() == added.m) 
                found = true;   
            else  
                if (inst.def()->ref()) 
                    for (auto x : inst.stl) {
                        for (auto y : addeds) 
                            if ( x == y.m ) { 
                                found = true; 
                                break; 
                            }
                        
                        if (found) break;
                    }    

            if (found) 
                inst.setDefault(); 

        });

    }
    
    if (!bkp) return;

    PLOGV << "remap " << bkp->name();

    remap(*bkp);

    bkp->deleteData();

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

    // each([](Member* m){ 

    //     for (auto inst : m->instances) {}

    //     return true;
        
    // });

    Instance(this).each([&](Instance& inst) {

        for (auto &x : inst.def()->instances) {

            if (x.get()->buff == inst.buff && x.get()->stl == inst.stl) {

                 x.get()->offset = inst.stl.size()?inst.offset:inst.def()->footprint(); // dirty shity hack rigght here - or is it ? // also might be missing eq support

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

        for (auto src_member_ : src_member->ref()?src_member->ref()->members:src_member->members) {

            Member* found = nullptr;

            auto thiseq = this_member->eq(i);

            int this_offset_ = this_offset + thiseq;

            for (auto this_member_ : this_member->ref()?this_member->ref()->members:this_member->members) {

                if (

                    !strcmp(src_member_->name().c_str(), this_member_->name().c_str())

                    //&& src_member_->type() == this_member_->type() // dont think thats relevent, fuxks

                ) { found = this_member_; break; }

                else this_offset_ += this_member_->footprint_all();

            }


            if (!found ) {

                src_offset_ += src_member_->footprint();

// #ifdef ROCH
               if (![src_member_](){ // COULD BE DEBUG MODE ONLY

                    for (auto x : removing) if (!strcmp(x->name().c_str(),src_member_->name().c_str())) return true;

                    return false;

                }())

                    PLOGE << "couldnt find " << src_member_->name() << " in " << src_buffer.name();
// #endif
                continue;
            }

            remap(src_buffer, src_member_, found, src_offset_, this_offset_);

            if (found->isData()) {

                // PLOGV  << src_member->name() << "::" << src_member_->name() << "@" << src_offset_ << " -> "  << " " << this_member->name() << "::" << found->name()  << "@" <<  this_offset_<< " - " << src_member_->size() << " : " << (unsigned int)*(char*)&src_buffer.data[src_offset_] << " -> " << *(float*)&data[this_offset_];

                memcpy(&data[this_offset_], &src_buffer.data[src_offset_],found->size());

            }

            src_offset_ += src_member_->footprint_all();

        }

    }

}
