#ifndef SMARTMAP_GUI_RENDERER_HPP
#define SMARTMAP_GUI_RENDERER_HPP
#pragma once

#include "ecs.hpp"
#include <iostream>

struct  GUIRenderer {

     flecs::filter<> components = ecs.filter_builder<>().term(flecs::ChildOf, flecs::Wildcard).oper(flecs::Not).build();
   
    std::vector<std::pair<std::string, std::string>> filter_strings;
    	
    TEST_GUI() {
        // build_filter();
    }

    void build_filter() {

        auto b = ecs.filter_builder<>();

        // for (auto s : filter_strings) {
            b.term(flecs::ChildOf, flecs::Wildcard).oper(flecs::Not);
        // }   
        
        components = b.build();

    }


    void draw() override { 

        ImGui::ShowDemoWindow();

        ImGui::Begin("Tree");


        components.each([](flecs::entity e) {

            bool has_color = false;

            if (e.has<flecs::Component>()) {
                // const auto* color = e.get<Object, Color>();
                ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255,100,100,255));
                // ImGui::PushStyleColor(ImGuiCol_TextSelectedBg, IM_COL32(255,100,100,255));
                has_color = true;
            }

            if (e.has<Object, Color>()) {
                // const auto* color = e.get<Object, Color>();
                // ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(color->value[0],color->value[1],color->value[2],255));
                // has_color = true;
            }

            ImGui::Text(e.str().c_str());

            if (has_color) ImGui::PopStyleColor();
    });

        ImGui::End();

     }

} ;


};


#endif