/*

    SMARTMAP (beta)

                        */

#include "engine.hpp"

#include "smartmap.hpp"
#include "ndi.hpp"
#include "widgets/artnet_widget.hpp"
#include "widgets/file.hpp"
#include "widgets/stack.hpp"
#include "widgets/fps.hpp"

#include "shader_builder.hpp"
#include "model.hpp"

// shader sources
// clocks
// fix feedback blending

int main() { 

    Log log;
    
    auto &engine = Engine::getInstance();
    engine.init();

    SmartMap::Base sm;

    auto *x = engine.dynamic_ubo.buffer.add("infos", {"int","int","int","int"},4);
    engine.specs = x->create();x->create();x->create();x->create();

    ShaderBuilder sb;

    
    
    // sm.config.import("config.json");

    // std::cout <<  "\nSHADERS HEADER:" << std::endl;
    // for (auto ubo:UBO::pool) { 

    //     std::string str = "layout (binding = ";
    //     str += std::to_string(ubo->binding);
    //     str += ", std140) uniform ";
    //     str += ubo->name;
    //     str += " { ";
        
    //     int i = 0; for (auto obj:ubo->buffer.objects) { 

    //         str += obj.name;
    //         str += " ";
    //         for(int i = 0; i < obj.name.length(); i++) str += std::tolower(obj.name[i]); 
    //         str += "[";
    //         str += std::to_string(obj.reserved);
    //         str += "]; ";

    //      }

    //     str += " };";
    //     std::cout <<  str << std::endl;

    // }





    struct TreeWidget : GUI::Window {

    TreeWidget() : GUI::Window("Tree")  {  }

    void draw() override { drawNode(&Engine::getInstance().tree); }

    void drawNode(Node* node) { 

            ImGui::SetNextItemOpen(true);

            if (ImGui::TreeNode(node->name.c_str())) {

                if (ImGui::BeginDragDropSource()) {

                    auto ptr = (uint64_t)node;
                    ImGui::SetDragDropPayload("_TREENODE", &ptr, sizeof(uint64_t));
                    ImGui::Text("This is a drag and drop source");
                    ImGui::EndDragDropSource();
                    
                }

                if (ImGui::BeginDragDropTarget()) {

                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_TREENODE")) {
                        
                        Node* snode = (Node*)(*(uint64_t*)payload->Data);

                        snode->parent(node);

                        PLOGD << snode->name << " > " << node->name;
       
                    }

                     ImGui::EndDragDropTarget();
                    
                }
                

                for (auto child : node->childrens) drawNode(child);
                ImGui::TreePop();

                }
    

                
                
            }
    }treezidget;

    FileWidget fw;

    FPSWidget fpsw;

    Engine::Run();
 
}


