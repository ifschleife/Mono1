//
//  SysOpenGL.h
//  Mono1
//
//  Created by Niblit on 2012-04-06.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#pragma once

#ifdef __IPHONEOS__
#include "SDL_opengles.h"
#else
#include "SDL_opengl.h"
#endif

namespace OGL
{
    struct OGLPushPopMatrix
    {
        OGLPushPopMatrix();
        ~OGLPushPopMatrix();
    };
    
    void glOrtho(float left, float right, float bottom, float top, float zNear, float zFar);
}
