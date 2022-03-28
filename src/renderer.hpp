#ifndef RENDERER_H
#define RENDERER_H
	
#include <vector>
#include "gui/TreeviewWindow.h"

struct Renderer : Node { 
	
	static inline std::vector<Renderer*> pool; 
	
	Renderer(const char* label = "Renderer") : Node(label) { pool.push_back(this); }  
	
};

struct RenderersNode : GroupNode {

	RenderersNode() : GroupNode("Renderers") {};

	void drawTree() override { for (auto* child : Renderer::pool) child->draw(); }

};

#endif