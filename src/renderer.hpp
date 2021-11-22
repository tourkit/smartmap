#ifndef RENDERER_H
#define RENDERER_H
	
#include <vector>

struct Drawcall {

	static inline std::vector<Drawcall*> callstack;

};

// struct Attribute {



// };

struct Renderer {

	Renderer();

	~Renderer();

	std::vector<Renderer*> tree;

	void execute() { 

		for (auto* r:tree) r->execute();

		callback();

	}

	virtual void callback() = 0;
		
};

struct Object : public Renderer {

	Object() {  }

	~Object() {  }

	// Mesh* mesh;

	// ShaderSnippet* shader;

	// BlendingMode blending;

	void execute() {



	}

};

#endif