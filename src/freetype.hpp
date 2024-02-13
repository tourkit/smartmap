#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H

struct Freetype { 

    FT_Library library;
    FT_Face face;
    FT_GlyphSlot slot;

    GLuint width;
    GLuint height;
    
    void* buffer;

    Freetype(const char* chr, int size = 200) {
        
        FT_Init_FreeType(&library);
        
        FT_New_Face(library, "assets/Anonymous.ttf", 0, &face); 

        FT_Set_Pixel_Sizes(face, 0, size);
        
        slot = face->glyph;
        FT_Load_Char(face, *chr, FT_LOAD_RENDER);

        buffer = slot->bitmap.buffer;
        width = slot->bitmap.width;
        height = slot->bitmap.rows;

    }

    ~Freetype() {
        
        FT_Done_Face(face);
        FT_Done_FreeType(library);

    }

};