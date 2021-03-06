// © Copyright 2010 - 2017 BlackTopp Studios Inc.
/* This file is part of The Mezzanine Engine.

The Mezzanine Engine is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

The Mezzanine Engine is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with The Mezzanine Engine. If not, see <http://www.gnu.org/licenses/>.
*/
/* The original authors have included a copy of the license specified above in the
'Docs' folder. See 'gpl.txt'
*/
/* We welcome the use of the Mezzanine engine to anyone, including companies who wish to
Build professional software and charge for their product.

However there are some practical restrictions, so if your project involves
any of the following you should contact us and we will try to work something
out:
- DRM or Copy Protection of any kind(except Copyrights)
- Software Patents You Do Not Wish to Freely License
- Any Kind of Linking to Non-GPL licensed Works
- Are Currently In Violation of Another Copyright Holder's GPL License
- If You want to change our code and not add a few hundred MB of stuff to
your distribution

These and other limitations could cause serious legal problems if you ignore
them, so it is best to simply contact us or the Free Software Foundation, if
you have any questions.

Joseph Toppi - toppij@gmail.com
John Blackwood - makoenergy02@gmail.com
*/
#ifndef _rigidproxy_h
#define _rigidproxy_h

#include "collidableproxy.h"
#include "transform.h"

class btRigidBody;

namespace Mezzanine
{
    class Entity;
    class CollisionShape;

    namespace Physics
    {
        class RigidProxy;
        class FixedConstraint;

        // Sticky Stuff
        typedef FixedConstraint StickyConstraint;

        /// @brief Simple struct for holding information on how sticky constraints should be constructed.
        struct MEZZ_LIB StickyConstraintConstructionInfo
        {
            Transform TransA;
            Transform TransB;
            RigidProxy* ProxA;
            RigidProxy* ProxB;

            StickyConstraintConstructionInfo()
            {
                this->ProxA = NULL;
                this->ProxB = NULL;
            }
            StickyConstraintConstructionInfo(const StickyConstraintConstructionInfo& Other)
            {
                this->TransA = Other.TransA;
                this->TransB = Other.TransB;
                this->ProxA = Other.ProxA;
                this->ProxB = Other.ProxB;
            }
        };//StickyConstraintConstructionInfo

        /// @brief This is a basic class for storing the data related to the sticky behavior available to rigid bodies.
        struct MEZZ_LIB StickyData
        {
            StickyData() : MaxNumContacts(0) {};
            std::vector<StickyConstraint*> StickyConstraints;
            std::vector<StickyConstraintConstructionInfo> CreationQueue;
            Whole MaxNumContacts;
        };//StickyData

        ///////////////////////////////////////////////////////////////////////////////
        /// @brief This is a proxy from which rigid body proxys are handled.
        /// @details This class is used to hold and configure the physics information for a rigid body.
        /// This class holds physics information from the physics sub-library and serves as a means to interact with it.
        /// Direct interaction with the internal physics proxy is discouraged.
        ///////////////////////////////////////
        class MEZZ_LIB RigidProxy : public CollidableProxy
        {
        protected:
            /// @internal
            /// @brief RigidBody proxy used by the internal physics.
            btRigidBody* PhysicsRigidBody;
            /*/// @internal
            /// @brief
            /// @internal
            /// @brief Data related to sticky behavior, if any is enabled.
            StickyData* StickyContacts;*/

            /// @internal
            /// @brief Used to create the physics representation of the rigid body.
            /// @param Mass The mass of the rigid body to be created.
            virtual void CreateRigidObject(const Real Mass);
        public:
            /// @brief Class Constructor.
            /// @param ID The unique ID assigned to this proxy.
            /// @param Mass The mass of the rigid body.
            /// @param Creator A pointer to the manager that created this proxy.
            RigidProxy(const UInt32 ID, const Real Mass, PhysicsManager* Creator);
            /// @brief Class Constructor.
            /// @param ID The unique ID assigned to this proxy.
            /// @param Mass The mass of the rigid body.
            /// @param Shape A pointer to the collision shape that will be applied to this proxy.
            /// @param Creator A pointer to the manager that created this proxy.
            RigidProxy(const UInt32 ID, const Real Mass, CollisionShape* Shape, PhysicsManager* Creator);
            /// @brief XML constructor.
            /// @param SelfRoot An XML::Node containing the data to populate the new instance with.
            /// @param Creator A pointer to the manager that created this proxy.
            RigidProxy(const XML::Node& SelfRoot, PhysicsManager* Creator);
            /// @brief Class Destructor
            virtual ~RigidProxy();

            ///////////////////////////////////////////////////////////////////////////////
            // Utility

            /// @copydoc EntityProxy::GetComponentType() const
            virtual Mezzanine::ComponentType GetComponentType() const override;

            /// @copydoc EntityProxy::Activate()
            virtual void Activate() override;
            /// @copydoc EntityProxy::Deactivate()
            virtual void Deactivate() override;

            ///////////////////////////////////////////////////////////////////////////////
            // Collision Settings

            /// @copydoc CollidableProxy::SetCollisionShape(CollisionShape*)
            virtual void SetCollisionShape(CollisionShape* Shape) override;

            ///////////////////////////////////////////////////////////////////////////////
            // Movement Factors

            /// @brief Restricts movement on the axis or axes of your choice.
            /// @details This function can lock or limit any and all axes you define.
            /// 0.0 means no linear movement on that axis.  1.0 means normal movement.
            /// @param Factor Vector3 containing the Factors for the 3 linear axes.
            virtual void SetLinearMovementFactor(const Vector3& Factor);
            /// @brief Gets the current linear factors being applied to this actor.
            /// @return Returns a Vector3 representing the factors on the 3 linear axes.
            virtual Vector3 GetLinearMovementFactor() const;
            /// @brief Restricts movement on the axis or axes of your choice.
            /// @details This function can lock or limit any and all axes you define.
            /// 0.0 means no angular movement on that axis.  1.0 means normal movement.
            /// @param Factor Vector3 containing the Factors for the 3 angular axes.
            virtual void SetAngularMovementFactor(const Vector3& Factor);
            /// @brief Gets the current angular factors being applied to this actor.
            /// @return Returns a Vector3 representing the factors on the 3 angular axes.
            virtual Vector3 GetAngularMovementFactor() const;

            ///////////////////////////////////////////////////////////////////////////////
            // Rigid Physics Properties

            /// @brief Change the mass of the proxy.
            /// @param Mass The amount of mass this should have.
            virtual void SetMass(const Real Mass);
            /// @brief Get the total Mass of the proxy
            /// @return A Real with the Mass of the proxy
            virtual Real GetMass() const;

            /// @brief Sets the Damping for this proxy.
            /// @details Both of Linear Damping and Angular Damping default to zero.  This is useful if you wish to simulate
            /// something like air resistance.  Values can range from 0.0 to 1.0.
            /// @param LinDamping Real representing the amount of Linear Damping(Movement) to be applied.
            /// @param AngDamping Real representing the amount of Angular Damping(Rotation) to be applied.
            virtual void SetDamping(const Real LinDamping, const Real AngDamping);
            /// @brief Get the linear damping
            /// @return A Real that has the Linear damping.
            virtual Real GetLinearDamping() const;
            /// @brief Get the Angular damping
            /// @return A Real that has the Angular damping.
            virtual Real GetAngularDamping() const;

            /// @brief Sets the Linear Velocity of this proxy.
            /// @param LinVel Vector3 representing the Linear Velocity to be set.
            virtual void SetLinearVelocity(const Vector3& LinVel);
            /// @brief Gets the Linear Velocity of this proxy.
            /// @return Returns the currently set Linear Velocity of this proxy.
            virtual Vector3 GetLinearVelocity() const;
            /// @brief Sets the Angular Velocity of this proxy.
            /// @param AngVel Vector3 representing the Angular Velocity to be set.
            virtual void SetAngularVelocity(const Vector3& AngVel);
            /// @brief Gets the Angular Velocity of this proxy.
            /// @return Returns the currently set Angular Velocity of this proxy.
            virtual Vector3 GetAngularVelocity() const;

            /// @brief Sets the gravity for only this proxy.
            /// @details This value will override the world gravity.
            /// @param Gravity Vector3 representing the direction and strength of gravity to be applied.
            virtual void SetGravity(const Vector3& Gravity);
            /// @brief Gets the gravity being applied to this proxy.
            /// @details This is the gravity applied to this proxy, which may or may not be the same as the world gravity.
            /// @return Returns a Vector3 representing the gravity currently being applied to this proxy.
            virtual Vector3 GetGravity() const;
            /// @brief Sets whether or not this proxy will ignore the world gravity when it's added to the world.
            /// @details Normally when a proxy is added to the world, the gravity set on that world is applied to the proxy.
            /// This will cause the world to which it is being added to not apply it's gravity to it.  This is ideal for objects
            /// that are having their gravity manipulated through gravity fields or something more manual.
            /// @param Ignore Whether or not to have the physics world skip applying it's gravity to this proxy when it's added to the world.
            virtual void SetIgnoreWorldGravity(const Boole Ignore);
            /// @brief Gets whether or not this proxy will ignore the world gravity when it's added to the world.
            /// @return Returns true if this proxy is not having the world gravity applied to it when it's added to the world, false otherwise.
            virtual Boole GetIgnoreWorldGravity() const;

            /// @brief Push/Apply force to an proxy.
            /// @param Force The amount and direction of the force in a Vector3
            virtual void ApplyForce(const Vector3& Force);
            /// @brief Get the total Force currently being applied to this proxy.
            /// @return Returns a Vector3 representing the amount of linear force being applied on each axis.
            virtual Vector3 GetAppliedForce() const;
            /// @brief Spin/Apply torque to an proxy.
            /// @param Torque The amount and direction of the torque in a Vector3
            virtual void ApplyTorque(const Vector3& Torque);
            /// @brief Get the total Torque currently being applied to this proxy.
            /// @return Returns a Vector3 representing the amount of angular force being applied on each axis.
            virtual Vector3 GetAppliedTorque() const;

            ///////////////////////////////////////////////////////////////////////////////
            // Sticky Data

            /*/// @brief Sets the basic parameters for enabling sticky behavior with this proxy.
            /// @param MaxNumContacts The maximum number of proxy this proxy can stick to or have stuck to it.
            virtual void SetStickyData(const Whole& MaxNumContacts);
            /// @brief Removes all the constraints currently active on this proxy
            virtual void ClearStickyContacts();
            /// @brief Gets the struct storing the data related to sticky behavior.
            /// @return Returns a pointer to the struct storing the sticky data for this proxy.
            virtual StickyData* GetStickyData() const;// */

            ///////////////////////////////////////////////////////////////////////////////
            // Transform Synchronization

            /*/// @brief Adds a TransformableObject that will force it's transform to sync with this RigidProxy.
            /// @param ToBeAdded A pointer to the Entity being added.
            virtual void AddSyncObject(TransformableObject* ToBeAdded);
            /// @brief Gets a TransformableObject being sync'd to this RigidProxy by it's index.
            /// @param Index The index of the sync object to retrieve.
            /// @return Returns a pointer to the TransformableObject at the specified Index.
            virtual TransformableObject* GetSyncObject(const UInt32 Index) const;
            /// @brief Gets the number of WorldProxies being sync'd to this RigidProxy.
            /// @return Returns a UInt32 representing the number of WorldProxies being sync'd with this RigidProxy.
            virtual UInt32 GetNumSyncObjects() const;
            /// @brief Removes a proxy being sync'd, so it will no longer match it's transform with this RigidProxy.
            /// @param ToBeRemoved A pointer to the TransformableObject to be removed.
            virtual void RemoveSyncObject(TransformableObject* ToBeRemoved);
            /// @brief Removes all WorldProxies being sync'd to this RigidProxy.
            virtual void RemoveAllSyncObjects();//*/

            ///////////////////////////////////////////////////////////////////////////////
            // Serialization

            /// @copydoc EntityProxy::ProtoSerializeProperties(XML::Node&) const
            virtual void ProtoSerializeProperties(XML::Node& SelfRoot) const override;
            /// @copydoc EntityProxy::ProtoDeSerializeProperties(const XML::Node&)
            virtual void ProtoDeSerializeProperties(const XML::Node& SelfRoot) override;

            /// @copydoc EntityProxy::GetDerivedSerializableName() const
            virtual String GetDerivedSerializableName() const override;
            /// @copydoc EntityProxy::GetSerializableName()
            static String GetSerializableName();

            ///////////////////////////////////////////////////////////////////////////////
            // Internal Methods

            /// @copydoc EntityProxy::_Bind(Entity*)
            virtual void _Bind(Entity* NewParent);

            /// @internal
            /// @brief Accessor for the internal rigid body physics proxy.
            /// @return Returns a pointer to the internal proxy this proxy is based on.
            virtual btRigidBody* _GetPhysicsObject() const;
            /// @copydoc CollidableProxy::_GetBasePhysicsObject()
            virtual btCollisionObject* _GetBasePhysicsObject() const override;
        };//RigidProxy
    }//Physics
}//Mezzanine

#endif
