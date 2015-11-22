//
//  Shuttle.cpp
//  Mono1
//
//  Created by Niblit on 2012-08-25.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#include "Shuttle.h"
#include "CMIBody.h"
#include "CMIShape.h"
#include "CMFactory.h"

#include "EventHandler.h"
#include "IRenderer.h"
#include "IWeaponSystem.h"
#include "WeaponFactory.h"

#include "MathFunctions.h"
#include "SysTime.h"
#include <cmath>

using namespace game;

namespace constants
{    
    enum
    {
        IDLE = 0,
        THRUSTING
    };
}


Shuttle::Shuttle(float x, float y, mono::EventHandler& eventHandler)
    : mSprite("shuttle.sprite"),
      mController(this, eventHandler),
      mEventHandler(eventHandler),
      m_fire(false),
      m_lastFireTimestamp(0)
{
    mPosition = math::Vector2f(x, y);
    mScale = math::Vector2f(20.0f, 20.0f);
    
    mPhysicsObject.body = cm::Factory::CreateBody(10.0f, 1.0f);
    mPhysicsObject.body->SetPosition(mPosition);
    mPhysicsObject.body->SetCollisionHandler(this);

    cm::IShapePtr shape = cm::Factory::CreateShape(mPhysicsObject.body, mScale.x, mScale.y);
    shape->SetElasticity(0.1f);
    
    mPhysicsObject.body->SetMoment(shape->GetInertiaValue());
    mPhysicsObject.shapes.push_back(shape);    
    
    mSprite.SetAnimation(constants::IDLE);

    // Make sure we have a weapon
    SelectWeapon(WeaponType::STANDARD);
    //SelectWeapon(WeaponType::ROCKET);
}

Shuttle::~Shuttle()
{ }

void Shuttle::Draw(mono::IRenderer& renderer) const
{
    renderer.DrawSprite(mSprite);
}

void Shuttle::Update(unsigned int delta)
{
    mSprite.doUpdate(delta);

    if(m_fire)
    {
        const float rpsHz = 1.0 / mWeapon->RoundsPerSecond();
        const unsigned int weaponDelta = rpsHz * 1000;

        const unsigned int now = Time::GetMilliseconds();
        const unsigned int delta = now - m_lastFireTimestamp;

        if(delta > weaponDelta)
        {
            mWeapon->Fire(mPosition, mRotation);
            m_lastFireTimestamp = now;
        }
    }
}

void Shuttle::OnCollideWith(cm::IBodyPtr body)
{
    //body->IsStatic();
    //mPhysicsObject.body->GetMoment();
}

void Shuttle::OnPostStep()
{ }

void Shuttle::SelectWeapon(WeaponType weapon)
{
    mWeapon = Factory::CreateWeapon(weapon, mEventHandler);

    // Setup the weapon's sprite
    //mWeaponSprite.
}

void Shuttle::ApplyRotationForce(float force)
{
    const math::Vector2f forceVector(force, 0.0);

    // First apply the rotational force at an offset of 20 in y axis, then negate the vector
    // and apply it to zero to counter the movement when we only want rotation.
    mPhysicsObject.body->ApplyForce(forceVector, math::Vector2f(0, 20));
    mPhysicsObject.body->ApplyForce(forceVector * -1, math::zeroVec);
}

void Shuttle::ApplyThrustForce(float force)
{
    const float rotation = Rotation();
    const math::Vector2f unit(-std::sin(math::ToRadians(rotation)),
                               std::cos(math::ToRadians(rotation)));

    mPhysicsObject.body->ApplyForce(unit * force, math::zeroVec);
}

void Shuttle::ApplyImpulse(const math::Vector2f& force)
{
    mPhysicsObject.body->ApplyImpulse(force, math::zeroVec);
}

void Shuttle::Fire()
{
    m_fire = true;
}

void Shuttle::StopFire()
{
    m_fire = false;
}

void Shuttle::StartThrusting()
{
    mSprite.SetAnimation(constants::THRUSTING);
}

void Shuttle::StopThrusting()
{
    mSprite.SetAnimation(constants::IDLE);
}



