//
//  PhysicsEntityBase.h
//  Mono1
//
//  Created by Niblit on 2013-03-31.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#pragma once

#include "IEntity.h"
#include "Vector2f.h"
#include "CMObject.h"

namespace mono
{
    class PhysicsEntityBase : public IPhysicsEntity
    {
    public:
        
        virtual const math::Vector2f& Position() const;
        virtual math::Quad BoundingBox() const;
        virtual cm::Object& GetPhysics();
        virtual bool RemoveMe() const;
        
    protected:
        
        PhysicsEntityBase();
        
        virtual void doDraw(IRenderer& renderer) const;
        virtual void doUpdate(unsigned int delta);
        
        virtual void Draw(IRenderer& renderer) const = 0;
        virtual void Update(unsigned int delta) = 0;
        
        math::Vector2f mPosition;
        float mRotation;
        float mScale;
        
        cm::Object mPhysicsObject;
    };
}

