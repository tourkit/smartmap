#pragma once

#include <string>
#include <vector>
#include <functional>
#include <cstddef>
#include <map>
#include <memory>
#include <unordered_set>
#include <set>

#include "member.hpp"
#include "data.hpp"
#include "log.hpp"



    struct Struct : Member {

        Struct(std::string name = "", uint32_t quantity = 1) : Member(name) { quantity_v = quantity; if (!name.length()) quantity_v = 0; }

        static inline std::set<Struct*> owned;

        static Struct& create(std::string name, uint32_t quantity = 1) { return **owned.insert(new Struct(name, quantity)).first; }

        static Struct* exist(std::string name) { for (auto &s : owned) if (s->name() == name) return s; return nullptr; }

        static Struct& id(std::string name) {

            auto x = exist(name);

            if (x) return *x;

            return create(name);

        }

        static void clear() { for ( auto s : owned ) delete s;  }

        static bool destroy(std::string name) {

            Struct* found = nullptr;

            for (auto &s : owned) if (s->name() == name) { found = s; break; }

            if (found) { owned.erase(found); delete found; return true; }

            return false;

        }

        Struct& add(Struct& s) { Member::add(&s); return *this; }

        Struct& add(const char* name) {

            for (auto s : owned) if (!strcmp(name,s->name().c_str())) { add(*s); return *this; }

            PLOGW << " noadd" << name; return *this;

        }

        Struct& remove(Struct& s) { Member::remove(&s); return *this; }

        template <typename T>
        Struct& add(std::string name = "") { Member::add(new Data<T>(name)); return *this; }

        Struct& add(Member* m) { Member::add(m); return *this; }

        Struct& range(float from, float to, float def) {

            auto a = members.back();
            if (typeid(*a) == typeid(Data<float>)) {
                ((Data<float>*)members.back())->range_from = from;
                ((Data<float>*)members.back())->range_to = to;
                // PLOGD << " ----- is : float";
            }

            if (typeid(*a) == typeid(Data<uint32_t>)) {
                ((Data<uint32_t>*)members.back())->range_from = (uint32_t) from;
                ((Data<uint32_t>*)members.back())->range_to = (uint32_t) to;
                // PLOGD << " ----- is : uint32_t";
            }

            return *this;

        }
        std::type_index type() override { if (typed()) { return members[0]->type(); } return typeid(Struct); }


        Member* copy(Member* x = nullptr) override {

            if (!x) x = new Struct(name_v);

            return Member::copy(x);

        }


    };
