

#include "src/window.hpp"
#include "src/gui.hpp"


auto* window = new GL::Window(false,800,400,1120); 

auto*  gui = new GUI{window->window}; 


struct Attribute {

    const char * name;

    float val;

    void gui() { ImGui::SliderFloat(name, &val, 0, 1); }

};

struct Effector {

    const char * name;
    
    const char* snippet;

    std::set<Attribute> attr;

};


struct TreeNode {

    const char * name;
    
    std::set<TreeNode> tree;

  };

//   struct GLObject : public TreeNode {

//     // Mesh mesh;
//     // Shader shader;
    
//     std::set<Attribute> attr;

//     Attribute& mesh_src = (attr.insert(Attribute()).first);
//     std::set<Effector> effectors;

//   };

 TreeNode* current = nullptr;
 
//   void RenderTree(TreeNode* n) {

//       ImGui::SetNextItemOpen(true, ImGuiCond_Once);

//       bool tn = ImGui::TreeNode(n->name);

//       if (ImGui::IsItemClicked()) current = n;

//       if (tn) {

//           for (TreeNode* child : n->tree) RenderTree(child);
            
//           ImGui::TreePop();
          
//       }

//   }


int main() {


    TextEditor editor("C:/Users/ebkc/Documents/testmake/assets/shader/smartmap.frag");
    TextEditor editor2("C:/Users/ebkc/Documents/testmake/assets/shader/smartmap.vert");

    auto* aaa = new TreeNode{"aaa"};
    current = aaa;
    // current->tree.push_back(new TreeNode{"bbb"});
    // current->tree.push_back(new TreeNode{"ccc"});
    // current->tree[0]->tree.push_back(new TreeNode{"ddd"});
 
    while(true) window->render([&]() {

        gui->newframe();  

        ImGui::Begin("Uniforms");
        ImGui::Text("%.1f FPS", ImGui::GetIO().Framerate);

        ImGui::End();

        editor.render();

        // RenderTree(aaa);

        std::string t_name = current->name;
        t_name += "###attributes";

        ImGui::Begin(t_name.c_str());
        

        // if (current) for (auto& a : current->attr) a.gui();

        ImGui::End();

        gui->render();

 
    });

} 

