
#pragma once

#include "TextDefinition.h"
#include "Math/MathFwd.h"
#include "Rendering/RenderPtrFwd.h"

namespace mono
{    
    //! Loads a font
    //! @param font_id The id of the font that you specify
    //! @param font Font file to use
    //! @param size Size of the font on the generated bitmap
    //! @param scale Scale of font when drawing with opengl (small font sizes can produce bad quality)
    void LoadFont(int font_id, const char* font, float size, float scale);
    void LoadFontRaw(int font_id, const unsigned char* data_bytes, int data_size, float size, float scale);

    //! Get the loaded font texture, might be nullptr if no texture is loaded.
    //! @param font_id The id of the font
    ITexturePtr GetFontTexture(int font_id);

    //! Clear the loaded fonts
    void UnloadFonts();

    //! Generates a text definition
    //! @param font_id The id of the font
    //! @param text The text...
    //! @param pos The position...
    //! @param center If the text should be horizontally centered to the position
    //! @return TextDefinition
    TextDefinition GenerateVertexDataFromString(int font_id, const char* text, const math::Vector& pos, bool center);
    
    //! Measures a string
    //! @param font_id The id of the font
    //! @param text The text to measure
    //! @return Size of the string
    math::Vector MeasureString(int font_id, const char* text);
}

