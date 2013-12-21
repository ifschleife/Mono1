//
//  PathPoint.cpp
//  MonoOSX
//
//  Created by Niklas Damberg on 21/12/13.
//
//

#include "PathPoint.h"
#include "PathFactory.h"
#include "IPath.h"
#include "Texture.h"
#include "SysOpenGL.h"

using namespace game;


PathPoint::PathPoint()
    : mElapsedTime(0.00001)
{
    std::vector<math::Vector2f> points;
    points.push_back(math::Vector2f(0, 0));
    points.push_back(math::Vector2f(0, 100));
    points.push_back(math::Vector2f(50, 100));
    points.push_back(math::Vector2f(250, 200));
    points.push_back(math::Vector2f(30, 30));
    points.push_back(math::Vector2f(0, 0));
    
    mPath = mono::CreatePath(points);
}

void PathPoint::Draw(mono::IRenderer& renderer) const
{
    mono::Texture::Clear();
    
    const float vertex[] = { 0.0f, 0.0f };
    
    glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
    glPointSize(10.0f);
    
    glEnableClientState(GL_VERTEX_ARRAY);
    
    glVertexPointer(2, GL_FLOAT, 0, vertex);
    glDrawArrays(GL_POINTS, 0, 1);
    
    glDisableClientState(GL_VERTEX_ARRAY);
}

void PathPoint::Update(unsigned int delta)
{
    mElapsedTime += delta;
    
    // 5000 == 5 seconds
    const float time = mElapsedTime / 5000;
    const float position = mPath->Length() * time;
    
    mPosition = mPath->GetPositionByLength(position);

    // Restart the path
    if(time >= 1.0f)
        mElapsedTime = 0;
}
