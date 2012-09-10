//
//  TraceCamera.h
//  Mono1
//
//  Created by Niblit on 2012-05-11.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#pragma once

#include "ICamera.h"
#include "MonoFwd.h"
#include "Quad.h"
#include "CameraController.h"

namespace mono
{
    class TraceCamera : public mono::ICamera
    {
    public:
        
        TraceCamera(int width, int height);
        
        virtual void Update(unsigned int delta);
        
        virtual void Follow(const mono::IEntityPtr entity);
        virtual void Unfollow();
        
        virtual const Math::Quad& GetViewport() const;
        virtual /* */ Math::Quad& GetViewport() /* */;
        
        virtual void SetTargetViewport(const Math::Quad& target);
        virtual void SetPosition(const Math::Vector2f& position);
                        
    private:
        
        CameraController mController;
        
        IEntityPtr mEntity;
        Math::Quad mViewport;
        Math::Quad mTargetViewport;
    };
}
