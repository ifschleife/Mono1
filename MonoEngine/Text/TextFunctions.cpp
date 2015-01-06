//
//  TextFunctions.cpp
//  Mono1
//
//  Created by Niblit on 2012-07-02.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#include "TextFunctions.h"
#include "SysFile.h"
#include "SysTypes.h"
#include "ITexture.h"
#include "TextureFactory.h"
#include "Vector2f.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

#include <map>


namespace
{
    struct CharData
    {
        float xadvance;
        float xoffset;
        float yoffset;
        float width;
        float height;
        
        float texCoordX0;
        float texCoordY0;
        float texCoordX1;
        float texCoordY1;
    };

    static mono::ITexturePtr fontTexture;
    static std::map<char, CharData> charMap;
}

void mono::LoadFont(const std::string& font, float size, float scale)
{
    File::FilePtr fontfile = File::OpenBinaryFile(font);
    
    std::vector<byte> fontbuffer;
    File::FileRead(fontfile, fontbuffer);
    
    constexpr int width = 512;
    constexpr int height = 512;
    constexpr int base = 32;
    constexpr int chars = 224;

    byte bitmap[width * height];
    stbtt_bakedchar chardata[chars];
    
    // The bitmap generated by this function has origo in the upper left corner,
    // thats why a bit down the y texture coordinate is flipped.
    stbtt_BakeFontBitmap(fontbuffer.data(), 0, size, bitmap, width, height, base, chars, chardata);

    // Same as GL_ALPHA!
    constexpr unsigned int ALPHA = 0x1906;
    fontTexture = mono::CreateTexture(bitmap, width, height, ALPHA, ALPHA);
    

    const float texCoordXMulti = 1.0f / width;
    const float texCoordYMulti = 1.0f / height;
    
    for(int index = 0; index < chars; ++index)
    {
        const stbtt_bakedchar& bakedchar = chardata[index];
        
        CharData data;
        data.width = (bakedchar.x1 - bakedchar.x0) * scale;
        data.height = (bakedchar.y1 - bakedchar.y0) * scale;
        data.xadvance = bakedchar.xadvance * scale;
        data.xoffset = bakedchar.xoff * scale;
        data.yoffset = (bakedchar.yoff * scale) + data.height;
        data.texCoordX0 = bakedchar.x0 * texCoordXMulti;
        data.texCoordY0 = bakedchar.y1 * texCoordYMulti;
        data.texCoordX1 = bakedchar.x1 * texCoordXMulti;
        data.texCoordY1 = bakedchar.y0 * texCoordYMulti;
        
        // For the correct char we need to add base to the index.
        const char thechar = static_cast<char>(index + base);
        charMap[thechar] = data;
    }
}

void mono::UseFont()
{
    if(!fontTexture)
        return;

    fontTexture->Use();
}

void mono::UnloadFont()
{
    fontTexture = nullptr;
    charMap.clear();
}

mono::TextDefinition mono::GenerateVertexDataFromString(const std::string& text, const math::Vector2f& pos, bool center)
{
    mono::TextDefinition textDef;
    textDef.chars = static_cast<unsigned int>(text.length());
    textDef.color = mono::Color(1.0f, 1.0f, 1.0f, 1.0f);
    textDef.vertices.reserve(textDef.chars * 12);
    textDef.texcoords.reserve(textDef.chars * 12);
    
    float xposition = pos.mX;
    float yposition = pos.mY;
    
    if(center)
        xposition -= MeasureString(text).mX / 2.0f;

    for(const char& currentChar : text)
    {
        // Look up char in map.
        const auto foundChar = charMap.find(currentChar);
        if(foundChar == charMap.end())
            continue;
        
        const CharData& data = foundChar->second;
        
        const float x0 = xposition + data.xoffset;
        const float y0 = yposition - data.yoffset;
        const float x1 = x0 + data.width;
        const float y1 = y0 + data.height;

        // First triangle
        textDef.vertices.emplace_back(x0);
        textDef.vertices.emplace_back(y0);
        textDef.texcoords.emplace_back(data.texCoordX0);
        textDef.texcoords.emplace_back(data.texCoordY0);
        
        textDef.vertices.emplace_back(x1);
        textDef.vertices.emplace_back(y1);
        textDef.texcoords.emplace_back(data.texCoordX1);
        textDef.texcoords.emplace_back(data.texCoordY1);
        
        textDef.vertices.emplace_back(x0);
        textDef.vertices.emplace_back(y1);
        textDef.texcoords.emplace_back(data.texCoordX0);
        textDef.texcoords.emplace_back(data.texCoordY1);
        
        // Second triangle
        textDef.vertices.emplace_back(x0);
        textDef.vertices.emplace_back(y0);
        textDef.texcoords.emplace_back(data.texCoordX0);
        textDef.texcoords.emplace_back(data.texCoordY0);
        
        textDef.vertices.emplace_back(x1);
        textDef.vertices.emplace_back(y0);
        textDef.texcoords.emplace_back(data.texCoordX1);
        textDef.texcoords.emplace_back(data.texCoordY0);
        
        textDef.vertices.emplace_back(x1);
        textDef.vertices.emplace_back(y1);
        textDef.texcoords.emplace_back(data.texCoordX1);
        textDef.texcoords.emplace_back(data.texCoordY1);
        
        // Add x size of char so that the next char is placed a little bit to the right.
        xposition += data.xadvance;
    }
    
    return textDef;
}

math::Vector2f mono::MeasureString(const std::string& text)
{
    math::Vector2f size;
    
    for(const char& currentChar : text)
    {
        // Look up char in map.
        const auto foundChar = charMap.find(currentChar);
        if(foundChar == charMap.end())
            continue;
        
        const CharData& data = foundChar->second;
        size.mX += data.xadvance;
        size.mY = std::max(data.height, size.mY);
    }
    
    return size;
}

