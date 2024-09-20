
#include <string>
#include "gui.hpp"


struct Member;

static int hovered_offset = -1;
static int hovered_size = -1;
static bool is_hovered = false;

static int p_min_1 = 1;

namespace ImGui {

void CustomSliderScalarN();
void TextX(std::string label, int offset, int size, int depth, std::vector<float> range = {}) ;
bool DimWiget(uint32_t* x, uint32_t* y, std::string append = "") ;
void DefWidget(Member *member_, int offset = 0, int depth = 0);
bool RawWidget(void *data, size_t size);
bool SlidersWidget(Member* buff, Member* member, uint32_t offset, int& member_count);
bool IntButtons(int* p_data );

};