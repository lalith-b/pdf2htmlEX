/*
 * Preprocessor.cc
 *
 * Check used codes for each font
 *
 * by WangLu
 * 2012.09.07
 */

#include <cstring>
#include <iostream>
#include <algorithm>

#include <GfxState.h>
#include <GfxFont.h>

#include "Preprocessor.h"
#include "util.h"

namespace pdf2htmlEX {

using std::cerr;
using std::endl;
using std::flush;
using std::max;

Preprocessor::Preprocessor(const Param * param)
    : OutputDev()
    , param(param)
    , max_width(0)
    , max_height(0)
    , cur_font_id(0)
    , cur_code_map(nullptr)
{ }

Preprocessor::~Preprocessor(void)
{
    for(auto iter = code_maps.begin(); iter != code_maps.end(); ++iter)
        delete [] iter->second;
}

void Preprocessor::process(PDFDoc * doc)
{
    for(int i = param->first_page; i <= param->last_page ; ++i) 
    {
        doc->displayPage(this, i, DEFAULT_DPI, DEFAULT_DPI,
                0, true, false, false,
                nullptr, nullptr, nullptr, nullptr);

        cerr << "." << flush;
    }
    cerr << endl;
}

void Preprocessor::drawChar(GfxState *state, double x, double y,
      double dx, double dy,
      double originX, double originY,
      CharCode code, int nBytes, Unicode *u, int uLen)
{
    GfxFont * font = state->getFont();
    if(!font) return;

    long long fn_id = hash_ref(font->getID());

    if(fn_id != cur_font_id)
    {
        cur_font_id = fn_id;
        auto p = code_maps.insert(std::make_pair(cur_font_id, (char*)nullptr));
        if(p.second)
        {
            // this is a new font
            int len = font->isCIDFont() ? 0x10000 : 0x100;
            p.first->second = new char [len];
            memset(p.first->second, 0, len * sizeof(char));
        }

        cur_code_map = p.first->second;
    }

    cur_code_map[code] = 1;
}

void Preprocessor::startPage(int pageNum, GfxState *state)
{
    max_width = max<double>(max_width, state->getPageWidth());
    max_height = max<double>(max_height, state->getPageHeight());
}

const char * Preprocessor::get_code_map (long long font_id) const
{
    auto iter = code_maps.find(font_id);
    return (iter == code_maps.end()) ? nullptr : (iter->second);
}

} // namespace pdf2htmlEX
