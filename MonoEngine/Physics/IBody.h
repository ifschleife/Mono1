
#pragma once

#include "CMFwd.h"
#include "Math/MathFwd.h"

namespace mono
{
    class ICollisionHandler
    {
    public:

        virtual ~ICollisionHandler()
        { }
        
        virtual void OnCollideWith(const mono::IBodyPtr& body, unsigned int categories) = 0;
    };

    class IBody
    {
    public:

        virtual ~IBody()
        { }
        
        //! Checks if this is a static body or not
        //! @return bool
        virtual bool IsStatic() const = 0;
        
        //! Sets the bodys mass
        //! @param mass The new mass
        virtual void SetMass(float mass) = 0;
        
        //! Get this bodys mass
        //! @return float The mass
        virtual float GetMass() const = 0;
        
        //! Sets the angle in radians
        //! @param angle New angle
        virtual void SetAngle(float angle) = 0;
        
        //! Gets the angle in radians
        //! @return float Angle
        virtual float GetAngle() const = 0;
        
        //! Sets the moment(inertia)
        //! @param moment Moment
        virtual void SetMoment(float moment) = 0;
        
        //! Gets the moment(inertia) of the body
        //! @return float moment
        virtual float GetMoment() const = 0;
        
        //! Set the position of the body
        //! @param position A new position
        virtual void SetPosition(const math::Vector& position) = 0;
        
        //! Gets the bodys position
        //! @return math::Vector The position
        virtual math::Vector GetPosition() const = 0;
        
        //! Apply a force to the body
        //! @param force The force
        //! @param offset From where to apply it
        virtual void ApplyForce(const math::Vector& force, const math::Vector& offset) = 0;

        //! Apply a local force to the body
        //! @param force The force
        //! @param offset From where to apply it
        virtual void ApplyLocalForce(const math::Vector& force, const math::Vector& offset) = 0;
        
        //! Apply an impulse to the body
        //! @param impulse The impulse
        //! @param offset From where to apply it
        virtual void ApplyImpulse(const math::Vector& impulse, const math::Vector& offset) = 0;

        //! Apply a local impulse to the body
        //! @param impulse The impulse
        //! @param offset From where to apply it
        virtual void ApplyLocalImpulse(const math::Vector& impulse, const math::Vector& offset) = 0;

        //! Sets a velocity to the body
        //! @param velocity A velocity vector
        virtual void SetVelocity(const math::Vector& velocity) = 0;

        //! Resets all the forces that are working on the body
        virtual void ResetForces() = 0;
        
        //! Sets a collision handler to the body
        //! @param handler A pointer to a collision handler
        virtual void SetCollisionHandler(mono::ICollisionHandler* handler) = 0;
        
        //! Called by the framework when a collision occures
        //! @param body The other body of the collision
        virtual void OnCollideWith(const IBodyPtr& body, unsigned int categories) = 0;
        
        //! Gets the underlying cpBody object, dont use this one
        //! @return cpBody* A pointer to a cpBody struct
        virtual cpBody* Handle() = 0;
    };
}
