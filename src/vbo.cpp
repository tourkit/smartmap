#include "vbo.hpp"  

#include "engine.hpp"  
#include "model.hpp"  


VBO::VBO() {  

    static bool init = false;

    if (!init) { 
        
        vertices = Engine::getInstance().dynamic_ubo.buffer.addObj(new Struct("Vertex", {"Position","UV","ID",}));;

        indices = Engine::getInstance().dynamic_ubo.buffer.addObj(new Struct("Index",{"Vertex", "Vertex", "Vertex"}));

        init = true;
    
    }

    create();
}

void VBO::destroy() {

    if (vbo<0) return;

    glDisableVertexAttribArray(0); 
    glDisableVertexAttribArray(1); 
    glDisableVertexAttribArray(2); 
    glDisableVertexAttribArray(3); 

    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ibo);
    glDeleteVertexArrays(1, &vao);

}

void VBO::create() {

    destroy();

    glGenBuffers(1, &vbo); glGenBuffers(1, &ibo); glGenVertexArrays(1, &vao);

}

VBO::~VBO()  { destroy(); }

void VBO::upload() {

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER,  vertices->size(), vertices->data(), GL_STATIC_DRAW );

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices->size(), indices->data(), GL_STATIC_DRAW );

    // make this parametric from Object vertices definition
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_TRUE, vertices->s->size, (GLvoid *) 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_TRUE, vertices->s->size, (GLvoid *) (2*sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_TRUE, vertices->s->size, (GLvoid *) (4*sizeof(float)));
    glEnableVertexAttribArray(3);

}

void VBO::add(Model *model,int id) {    

    models.push_back({model,id});

    // updatye buffer : by adding converted Model to Buffer::Object(Indices) && Buffer::Object(Indices)


    // Buffer::Object(Indices).add()

    upload();

}


void VBO::draw(int count) {

    glBindVertexArray(vao); 

    glDrawElementsInstanced(GL_TRIANGLES, indices->size(), GL_UNSIGNED_INT, 0, count);

}
	
