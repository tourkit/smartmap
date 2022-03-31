#ifndef RENDERER_H
#define RENDERER_H
	
#include <vector>
#include "gui/TreeviewWindow.h"


struct Renderer : Node<Renderer> { 

	Renderer(const char * label = "Renderer" ) : Node<Renderer>(label) {};
	
};

struct Repository : Node<Repository> { 

	Repository(const char * label = "Repository" ) : Node<Repository>(label) {};
	
};

struct DrawCall : Node<DrawCall> { 

	DrawCall(const char * label = "DrawCall" ) : Node<DrawCall>(label) {};
	
};


#endif