//
//  Shuttle.h
//  Mono1
//
//  Created by Niblit on 2012-08-25.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#pragma once

#include "MonoPtrFwd.h"
#include "PhysicsEntityBase.h"
#include "ShuttleController.h"
#include "CMIBody.h"

namespace game
{
    class IWeaponSystem;
    enum class WeaponType;

    class Shuttle : public mono::PhysicsEntityBase, public mono::ICollisionHandler
    {
    public:
        
        Shuttle(float x, float y, mono::EventHandler& eventHandler);

    private:

        friend class ShuttleController;

        void SelectWeapon(WeaponType weapon);
        
        void ApplyRotationForce(float force);
        void ApplyThrustForce(float force);
        void ApplyImpulse(const math::Vector2f& force);

        void Fire();
        void StopFire();

        void StartThrusting();
        void StopThrusting();
        
        virtual void Draw(mono::IRenderer& renderer) const;
        virtual void Update(unsigned int delta);
        
        virtual void OnCollideWith(const mono::IBodyPtr& body);
        virtual void OnPostStep();

        std::unique_ptr<IWeaponSystem> mWeapon;

        mono::ISpritePtr mSprite;
        //mono::Sprite mWeaponSprite;
        ShuttleController mController;
        mono::EventHandler& mEventHandler;

        bool m_fire;
    };
}

