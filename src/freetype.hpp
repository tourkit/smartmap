#pragma once

#include <cstdint>
#include "log.hpp"

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H  
#include FT_OUTLINE_H
#include FT_STROKER_H

#ifdef _MSC_VER
#define MIN __min
#define MAX __max
#else
#define MIN std::min
#define MAX std::max
#endif

struct Span
{
  Span() { }
  Span(int _x, int _y, int _width, int _coverage)
  : x(_x), y(_y), width(_width), coverage(_coverage) { }

  int x, y, width, coverage;
};
typedef std::vector<Span> Spans;

static void RasterCallback(const int y,
               const int count,
               const FT_Span * const spans,
               void * const user) 
{
  Spans *sptr = (Spans *)user;
  for (int i = 0; i < count; ++i) 
    sptr->push_back(Span(spans[i].x, y, spans[i].len, spans[i].coverage));
}


static void RenderSpans(FT_Library &library,
            FT_Outline * const outline,
            Spans *spans) 
{
  FT_Raster_Params params;
  memset(&params, 0, sizeof(params));
  params.flags = FT_RASTER_FLAG_AA | FT_RASTER_FLAG_DIRECT;
  params.gray_spans = RasterCallback;
  params.user = spans;

  FT_Outline_Render(library, outline, &params);
}

union Pixel32
{
  Pixel32()
  : integer(0) { }
  Pixel32(uint8_t bi, uint8_t gi, uint8_t ri, uint8_t ai = 255)
  {
    b = bi;
    g = gi;
    r = ri;
    a = ai;
  }

  uint32_t integer;

  struct
  {
#ifdef BIG_ENDIAN
    uint8_t a, r, g, b;
#else // BIG_ENDIAN
    uint8_t b, g, r, a;
#endif // BIG_ENDIAN
  };
};



struct Vec2
{
  Vec2() { }
  Vec2(float a, float b)
  : x(a), y(b) { }

  float x, y;
};


struct Rect
{
  Rect() { }
  Rect(float left, float top, float right, float bottom)
  : xmin(left), xmax(right), ymin(top), ymax(bottom) { }

  void Include(const Vec2 &r)
  {
    xmin = MIN(xmin, r.x);
    ymin = MIN(ymin, r.y);
    xmax = MAX(xmax, r.x);
    ymax = MAX(ymax, r.y);
  }

  float Width() const { return xmax - xmin + 1; }
  float Height() const { return ymax - ymin + 1; }

  float xmin, xmax, ymin, ymax;
};




struct Freetype { 

    FT_Library library;
    FT_Face face;

    uint32_t width;
    uint32_t height;

    float outlineWidth = 20;

    Pixel32 outlineCol,fontCol;

    
    void* buffer;

    Freetype(const char* chr, int size = 200) : outlineCol(255,0,255,255), fontCol(255,255, 0,255) {
        
        FT_Init_FreeType(&library);
        
        FT_New_Face(library, "assets/fonts/Anonymous.ttf", 0, &face); 
      FT_Set_Char_Size( face, 0, 100    ,
                            0, 100 );  
        // FT_Set_Pixel_Sizes(face, size, size);

    FT_GlyphSlot slot = face->glyph;
        FT_Load_Char(face, *chr, FT_LOAD_RENDER);

        buffer = slot->bitmap.buffer;
        width = slot->bitmap.width;
        height = slot->bitmap.rows;

//         FT_Set_Char_Size(face, size << 6, size << 6, 0, size);
//     FT_UInt gindex = FT_Get_Char_Index(face, *chr);
//     FT_Load_Glyph(face, gindex, FT_LOAD_NO_BITMAP);
    
//       // Need an outline for this to work.
//       if (face->glyph->format == FT_GLYPH_FORMAT_OUTLINE) {
//         Spans spans;
//         RenderSpans(library, &face->glyph->outline, &spans);

        
// // Next we need the spans for the outline.
// Spans outlineSpans;

// // Set up a stroker.
// FT_Stroker stroker;
// FT_Stroker_New(library, &stroker);
// FT_Stroker_Set(stroker,
//                 (int)(outlineWidth * 64),
//                 FT_STROKER_LINECAP_ROUND,
//                 FT_STROKER_LINEJOIN_ROUND,
//                 0);

// FT_Glyph glyph;
// FT_Get_Glyph(face->glyph, &glyph);
//   FT_Glyph_StrokeBorder(&glyph, stroker, 0, 1);
//   // Again, this needs to be an outline to work.
//   if (glyph->format == FT_GLYPH_FORMAT_OUTLINE)
//   {
//     // Render the outline spans to the span list
//     FT_Outline *o =
//       &reinterpret_cast<FT_OutlineGlyph>(glyph)->outline;
//     RenderSpans(library, o, &outlineSpans);
//   }

//   // Clean up afterwards.
//   FT_Stroker_Done(stroker);
//   FT_Done_Glyph(glyph);

//   // Now we need to put it all together.
//   if (!spans.empty())
//   {
//     // Figure out what the bounding rect is for both the span lists.
//     Rect rect(spans.front().x,
//               spans.front().y,
//               spans.front().x,
//               spans.front().y);
//     for (Spans::iterator s = spans.begin();
//           s != spans.end(); ++s)
//     {
//       rect.Include(Vec2(s->x, s->y));
//       rect.Include(Vec2(s->x + s->width - 1, s->y));
//     }
//     for (Spans::iterator s = outlineSpans.begin();
//           s != outlineSpans.end(); ++s)
//     {
//       rect.Include(Vec2(s->x, s->y));
//       rect.Include(Vec2(s->x + s->width - 1, s->y));
//     }

// #if 0
//     // This is unused in this test but you would need this to draw
//     // more than one glyph.
//     float bearingX = face->glyph->metrics.horiBearingX >> 6;
//     float bearingY = face->glyph->metrics.horiBearingY >> 6;
//     float advance = face->glyph->advance.x >> 6;
// #endif

//     // Get some metrics of our image.
//     int imgWidth = rect.Width(),
//         imgHeight = rect.Height(),
//         imgSize = imgWidth * imgHeight;

//     // Allocate data for our image and clear it out to transparent.
//     Pixel32 *pxl = new Pixel32[imgSize];
//     memset(pxl, 0, sizeof(Pixel32) * imgSize);

//     // Loop over the outline spans and just draw them into the
//     // image.
//     for (Spans::iterator s = outlineSpans.begin();
//           s != outlineSpans.end(); ++s)
//       for (int w = 0; w < s->width; ++w)
//         pxl[(int)((imgHeight - 1 - (s->y - rect.ymin)) * imgWidth
//                   + s->x - rect.xmin + w)] =
//           Pixel32(outlineCol.r, outlineCol.g, outlineCol.b,
//                   s->coverage);

//     // Then loop over the regular glyph spans and blend them into
//     // the image.
//     for (Spans::iterator s = spans.begin();
//           s != spans.end(); ++s)
//       for (int w = 0; w < s->width; ++w)
//       {
//         Pixel32 &dst =
//           pxl[(int)((imgHeight - 1 - (s->y - rect.ymin)) * imgWidth
//               + s->x - rect.xmin + w)];
//         Pixel32 src = Pixel32(fontCol.r, fontCol.g, fontCol.b,
//                               s->coverage);
//         dst.r = (int)(dst.r + ((src.r - dst.r) * src.a) / 255.0f);
//         dst.g = (int)(dst.g + ((src.g - dst.g) * src.a) / 255.0f);
//         dst.b = (int)(dst.b + ((src.b - dst.b) * src.a) / 255.0f);
//         dst.a = MIN(255, dst.a + src.a);
//       }

//         buffer = pxl;
//         width = imgWidth;
//         height = imgHeight;
//   }



//       }else {
//         PLOGE << "nonon";
//       }


    }

    ~Freetype() {
        
        FT_Done_Face(face);
        FT_Done_FreeType(library);

    }

};