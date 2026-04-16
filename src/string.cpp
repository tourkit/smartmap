#include "string.hpp"

bool String::set(const char* value) {
    
    auto len = strlen(value);

    if (len > length)
        resize(len);
    else
        length = len;
        
    memmove(&data[offset], value, len);

    return 1;
}

char* String::mut() { return &data[offset]; }

auto String::view() const { return std::string_view(data.data() + offset, length); }

String String::string() { return String(*this); }

void String::resize(size_t length) {
    
    int diff =  length-this->length;

    auto old_size = data.size();

    if (length && old_size+diff>data.capacity()) 
        data.reserve(std::max(old_size,length)*2);

    data.resize(old_size+diff);

    auto end = offset+this->length;

    memmove(&data[end]+diff, &data[end], old_size-end);

    this->length = length;

    for(auto e : pool)
        if (e->offset>offset)   
            e->offset += diff;

}