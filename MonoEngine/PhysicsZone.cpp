

#include "PhysicsZone.h"
#include "IRenderer.h"
#include "IUpdatable.h"
#include "Physics/CMSpace.h"
#include "Physics/CMObject.h"
#include "Physics/CMIShape.h"
#include "IPhysicsEntity.h"

#include "Utils.h"
#include <cstdio>

using namespace mono;

struct PhysicsZone::PhysicsImpl : IUpdatable
{
    PhysicsImpl(const math::Vector& gravity, float damping)
        : mSpace(gravity, damping)
    { }
    void doUpdate(unsigned int delta)
    {
        const float floatDelta = delta;
        mSpace.Tick(floatDelta / 1000.0f);
    }
    mono::Space mSpace;
};

PhysicsZone::PhysicsZone(const math::Vector& gravity, float damping)
    : mPhysics(new PhysicsImpl(gravity, damping))
{
    // Add space physics as an updatable
    AddUpdatable(mPhysics);
}

void PhysicsZone::ForEachBody(const mono::BodyFunc& func)
{
    mPhysics->mSpace.ForEachBody(func);
}

void PhysicsZone::AddPhysicsEntity(const mono::IPhysicsEntityPtr& entity, int layer)
{
    AddDrawable(entity, layer);
    AddUpdatable(entity);
    
    const mono::Object& object = entity->GetPhysics();
    mPhysics->mSpace.AddBody(object.body);
    
    for(auto& shape : object.shapes)
        mPhysics->mSpace.AddShape(shape);
    
    mPhysicsEntities.push_back(entity);
}

void PhysicsZone::RemovePhysicsEntity(const mono::IPhysicsEntityPtr& entity)
{
    RemoveDrawable(entity);
    RemoveUpdatable(entity);

    const bool result = FindAndRemove(mPhysicsEntities, entity);
    if(result)
    {
        mono::Object& object = entity->GetPhysics();
        mPhysics->mSpace.RemoveBody(object.body);
        
        for(auto& shape : object.shapes)
            mPhysics->mSpace.RemoveShape(shape);
    }
    else
    {
        std::printf("PhysicsZone - Unable to remove physics entity\n");
        //throw std::runtime_error("PhysicsZone - Unable to remove physics entity");
    }
}

IPhysicsEntityPtr PhysicsZone::FindPhysicsEntityFromBody(const mono::IBodyPtr& body) const
{
    for(auto& entity : mPhysicsEntities)
    {
        if(entity->GetPhysics().body == body)
            return entity;
    }

    return nullptr;
}

IPhysicsEntityPtr PhysicsZone::FindPhysicsEntityFromId(uint id) const
{
    for(auto& entity : mPhysicsEntities)
    {
        if(entity->Id() == id)
            return entity;
    }

    return nullptr;
}



