//
//  Texture.cpp
//  Monolith1
//
//  Created by Niblit on 2011-12-10.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "Texture.h"
#include "SysOpenGL.h"
#include <cstdio>

using namespace mono;

namespace
{
    unsigned int NextPowerOfTwo(unsigned int x)
    {
        unsigned int val = 1;
        while(val < x)
            val *= 2;

        return val;
    }
}

Texture::Texture(IImagePtr image)
    : mTextureId(-1),
      mWidth(image->Width()),
      mHeight(image->Height())
{
    glGenTextures(1, &mTextureId);
    glBindTexture(GL_TEXTURE_2D, mTextureId);
    
    const int internalFormat = image->ColorComponents();
    const unsigned int width = NextPowerOfTwo(image->Width());
    const unsigned int height = NextPowerOfTwo(image->Height());
    const unsigned int sourceFormat = image->TargetFormat();
    const byte* data = image->Data();
    
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, (int)width, (int)height, 0, sourceFormat, GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    const GLenum error = glGetError();
    if(error != GL_NO_ERROR)
        std::printf("Open GL error in Texture. Error no: %X\n", error);
}

Texture::~Texture()
{
    glDeleteTextures(1, &mTextureId);
}

void Texture::Use() const
{
    glBindTexture(GL_TEXTURE_2D, mTextureId);
}

unsigned int Texture::Id() const
{
    return mTextureId;
}

unsigned int Texture::Width() const
{
    return mWidth;
}

unsigned int Texture::Height() const
{
    return mHeight;
}
