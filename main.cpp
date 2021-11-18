/*

    SMARTMAP v1 beta

                        */

#include "smartmap.hpp"

#include "vendor/ImGuiColorTextEdit/TextEditor.h"


#define DEBUG

ArtnetDevice artnet;

int WIDTH = 600, HEIGHT = 300;

int MAT_X = 5; 
int MAT_Y = 2;

auto* window = new GL::Window(false,WIDTH,HEIGHT,1920-WIDTH);

auto*  gui = new GUI{window->window}; 

auto* shader = new ShaderProgram({"assets/shader/smartmap.vert", "assets/shader/smartmap.frag"});
auto* blur_x = new ShaderProgram({"assets/shader/blur_x.comp"});
auto* blur_y = new ShaderProgram({"assets/shader/blur_x.comp"});
 
Quad draw2D;
void Draw2D(const Texture& tex) {

    glBindTexture(GL_TEXTURE_2D, tex.id);
    shader->use();
    draw2D.draw();

}

double lastTime = glfwGetTime();
bool checkFPS() { if (glfwGetTime() - lastTime <= 1./280. ) { return false; } else { lastTime = glfwGetTime(); return true; } }

int main() {

    	TextEditor editor;
std::string str("test");
			editor.SetText(str);
            static const char* fileToEdit = "C:/Users/ebkc/Documents/testmake/web/index.html";
	{
		std::ifstream t(fileToEdit);
		if (t.good())
		{
			std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
			editor.SetText(str);
		}
	}
    Atlas atlas("assets/media/");
    atlas.link(shader);
    
    // int FW = 1920*MAT_X, FH = 1200*MAT_Y; // GOOD 
    int FW = WIDTH*MAT_X, FH = HEIGHT*MAT_Y; // TEST ONLY !!!REMOVE FOR RELEASE

    Texture passBuf(FW,FH, GL_RGBA8);

    Texture outBuf(FW,FH, GL_RGBA8); 
    FrameBuffer outFB(outBuf);
    Texture outBlur(FW*.5,FH*.5, GL_RGBA8); 

    FrameBuffer winFB(0);

    VBO quad;
    quad.addQuad(1); // UID #1 in shader (feedback)
    // quad.addQuad(2); // UID #2 in shader (fixture)

    VBO quad2;
    quad2.addQuad(2); // UID #2 in shader (fixture)

    auto mat = matrice(MAT_X,MAT_Y);
    UBO matriceUBO(&mat[0], mat.size()*sizeof(RectF), "MatriceUBO"); 
    matriceUBO.link(shader);
    matriceUBO.send();

    Fixture fixtures[25];
    int address = 1;

    for (auto f:fixtures) address += f.DMXpatch(1,address, {
    
        "Color.Red",
        "Color.Green",
        "Color.Blue",
        "Position.Horizontal",
        "Position.Vertical",
        "Gobo.ID",
        "Gobo.Fx1",
        "Gobo.Fx2",
        "Gobo.Fx3",
        "Strobe",
        "Feedback",
        "Orientation",
        "Focus.Horizontal",
        "Focus.Vertical"
        
    });

    UBO fixtureUBO(&Attribute::UBO[0], 24*64, "FixtureUBO"); 
    fixtureUBO.link(shader);
    fixtureUBO.send();

    // auto count = gui->add(new GUI::Counter("count"), shader);
    // auto blur = gui->add(new GUI::SliderF("blurv",   1, .0,  0,  1));
    // blur->links.insert(blur_x);
    // blur->links.insert(blur_y);

    glEnable(GL_CLIP_DISTANCE0);
    glEnable(GL_CLIP_DISTANCE1);
    glEnable(GL_CLIP_DISTANCE2);
    glEnable(GL_CLIP_DISTANCE3);

    shader->sendUniform("MatriceUBOSize", MAT_X*MAT_Y);

    while(true) window->render([&]() {

        artnet.run();

        // if (!checkFPS()) return;

        // CLUSTER RENDER LOOP

        outFB.clear(); // thus bind

        fixtureUBO.send();

        passBuf.bind();
        shader->use();

        glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_SRC_COLOR);
        quad.draw(MAT_X*MAT_Y); // quantity is instances count in shader 
        glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
        quad2.draw(MAT_X*MAT_Y); // quantity is instances count in shader 

        passBuf.copy(outBuf);

        glBindImageTexture(0, outBlur, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);
        glBindImageTexture(1, outBuf, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA8);
        blur_x->use(FW*.5/16,FH*.5/16);
        blur_y->use(FW*.5/16,FH*.5/16);
        glMemoryBarrier( GL_ALL_BARRIER_BITS ); 

        winFB.clear(); // thus bind

        // ID 0
        Draw2D(outBlur);

        // // END OF LOOP
    
        gui->newframe();  
        ImGui::Begin("Uniforms");
        ImGui::Text("%.1f FPS", ImGui::GetIO().Framerate);

        for (auto a:fixtures[0].attributes->childrens) a->gui();

        ImGui::End();

        		auto cpos = editor.GetCursorPosition();
		ImGui::Begin("Text Editor Demo", nullptr, ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_MenuBar);
		ImGui::SetWindowSize(ImVec2(800, 600), ImGuiCond_FirstUseEver);
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Save"))
				{
					auto textToSave = editor.GetText();
					/// save text....
				}
				// if (ImGui::MenuItem("Quit", "Alt-F4"))
				// 	break;
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Edit"))
			{
				bool ro = editor.IsReadOnly();
				if (ImGui::MenuItem("Read-only mode", nullptr, &ro))
					editor.SetReadOnly(ro);
				ImGui::Separator();

				if (ImGui::MenuItem("Undo", "ALT-Backspace", nullptr, !ro && editor.CanUndo()))
					editor.Undo();
				if (ImGui::MenuItem("Redo", "Ctrl-Y", nullptr, !ro && editor.CanRedo()))
					editor.Redo();

				ImGui::Separator();

				if (ImGui::MenuItem("Copy", "Ctrl-C", nullptr, editor.HasSelection()))
					editor.Copy();
				if (ImGui::MenuItem("Cut", "Ctrl-X", nullptr, !ro && editor.HasSelection()))
					editor.Cut();
				if (ImGui::MenuItem("Delete", "Del", nullptr, !ro && editor.HasSelection()))
					editor.Delete();
				if (ImGui::MenuItem("Paste", "Ctrl-V", nullptr, !ro && ImGui::GetClipboardText() != nullptr))
					editor.Paste();

				ImGui::Separator();

				if (ImGui::MenuItem("Select all", nullptr, nullptr))
					editor.SetSelection(TextEditor::Coordinates(), TextEditor::Coordinates(editor.GetTotalLines(), 0));

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("View"))
			{
				if (ImGui::MenuItem("Dark palette"))
					editor.SetPalette(TextEditor::GetDarkPalette());
				if (ImGui::MenuItem("Light palette"))
					editor.SetPalette(TextEditor::GetLightPalette());
				if (ImGui::MenuItem("Retro blue palette"))
					editor.SetPalette(TextEditor::GetRetroBluePalette());
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		ImGui::Text("%6d/%-6d %6d lines  | %s | %s | %s | %s", cpos.mLine + 1, cpos.mColumn + 1, editor.GetTotalLines(),
			editor.IsOverwrite() ? "Ovr" : "Ins",
			editor.CanUndo() ? "*" : " ",
			editor.GetLanguageDefinition().mName.c_str(), fileToEdit);

		editor.Render("TextEditor");
        ImGui::End();


        gui->render();

 
    });

} 

