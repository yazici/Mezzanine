//c Copyright 2010 - 2012 BlackTopp Studios Inc.
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
#ifndef _physicsproxy_cpp
#define _physicsproxy_cpp

#include "physicsproxy.h"
#include "Physics/collisionshape.h"

#include <btBulletDynamicsCommon.h>

namespace Mezzanine
{
    namespace Physics
    {
        ///////////////////////////////////////////////////////////////////////////////
        // ScalingShape Methods

        ///////////////////////////////////////////////////////////////////////////////
        /// @class ScalingShape
        /// @brief This is a custom scaling shape that permits scaling specific to the object it is applied to.
        /// @details Scaling portion of a transform does not exist on RigidBodies in Bullet.  For an object to be
        /// scaled it has to be done on the collision shape.  However collision shapes can be shared and if they are
        /// it would scale all other objects it is shared with as well.  This shape is a simple scaling wrapper for
        /// collision shapes that can be created just for a single object that allows re-use and sharing of
        /// collision shapes when scaling them for different objects. @n @n
        /// Bullet does have another scaling shape built in for different types of shapes.  GImpact lacks one, but
        /// BVHTriangleMesh have an appropriate wrapper.  All the collision shapes under the Convex branch of the
        /// inheritance tree have another that only applies custom scaling uniformly on all axes.  This is
        /// unacceptable and one that allows independant scaling on each axis is needed.  That is where this class
        /// comes in.
        ///////////////////////////////////////
        ATTRIBUTE_ALIGNED16(class) MEZZ_LIB ScalingShape : public btConvexShape
        {
        protected:
            btConvexShape* ChildConvexShape;
            btVector3 ChildScaling;
        public:
            BT_DECLARE_ALIGNED_ALLOCATOR();

            /// @brief Class constructor.
            ScalingShape(btConvexShape* ChildShape, const btVector3& Scaling) :
                ChildConvexShape(ChildShape),
                ChildScaling(Scaling)
                { this->m_shapeType = UNIFORM_SCALING_SHAPE_PROXYTYPE; }
            /// @brief Class destructor.
            virtual ~ScalingShape()
                {  }

            ///////////////////////////////////////////////////////////////////////////////
            // Configuration Methods

            /// @brief Gets the child shape being scaled by this wrapper.
            virtual btConvexShape* GetChildShape() const
            {
                return this->ChildConvexShape;
            }
            /// @brief Gets the amount of scaling being applied to the child shape.
            virtual btVector3 GetChildScaling() const
            {
                return this->ChildScaling;
            }
            /// @brief Gets the serialization name of this shape.
            virtual const char*	getName()const
            {
                return "UniformScalingShape";
            }

            ///////////////////////////////////////////////////////////////////////////////
            // Internal Transform Methods

            /// @brief No Idea.
            virtual btVector3 localGetSupportingVertexWithoutMargin(const btVector3& vec)const
            {
                btVector3 tmpVertex;
                tmpVertex = this->ChildConvexShape->localGetSupportingVertexWithoutMargin(vec);
                return tmpVertex * this->ChildScaling;
            }
            /// @brief No Idea.
            virtual btVector3 localGetSupportingVertex(const btVector3& vec)const
            {
                btVector3 tmpVertex;
                btScalar ChildMargin = this->ChildConvexShape->getMargin();
                tmpVertex = this->ChildConvexShape->localGetSupportingVertexWithoutMargin(vec);
                return (tmpVertex * this->ChildScaling) + btVector3(ChildMargin,ChildMargin,ChildMargin);
            }
            /// @brief No Idea.
            virtual void batchedUnitVectorGetSupportingVertexWithoutMargin(const btVector3* vectors,btVector3* supportVerticesOut,int numVectors) const
            {
                this->ChildConvexShape->batchedUnitVectorGetSupportingVertexWithoutMargin(vectors,supportVerticesOut,numVectors);
                for( int i = 0 ; i < numVectors ; i++ )
                {
                    supportVerticesOut[i] = supportVerticesOut[i] * this->ChildScaling;
                }
            }
            /// @brief Calculates the local inertia for this shape and it's child shape.
            virtual void calculateLocalInertia(btScalar mass,btVector3& inertia) const
            {
                btVector3 tmpInertia;
                this->ChildConvexShape->calculateLocalInertia(mass,tmpInertia);
                inertia = tmpInertia * this->ChildScaling;
            }

            ///////////////////////////////////////////////////////////////////////////////
            // Inherited from btCollisionShape

            /// @brief Gets the AABB of this shape.
            virtual void getAabb(const btTransform& trans,btVector3& aabbMin,btVector3& aabbMax) const
            {
                this->getAabbSlow(trans,aabbMin,aabbMax);
            }
            /// @brief Gets the AABB of this shape.
            virtual void getAabbSlow(const btTransform& t,btVector3& aabbMin,btVector3& aabbMax) const
            {
                btVector3 _directions[] =
                {
                    btVector3( 1.,  0.,  0.),
                    btVector3( 0.,  1.,  0.),
                    btVector3( 0.,  0.,  1.),
                    btVector3( -1., 0.,  0.),
                    btVector3( 0., -1.,  0.),
                    btVector3( 0.,  0., -1.)
                };

                btVector3 _supporting[] =
                {
                    btVector3( 0., 0., 0.),
                    btVector3( 0., 0., 0.),
                    btVector3( 0., 0., 0.),
                    btVector3( 0., 0., 0.),
                    btVector3( 0., 0., 0.),
                    btVector3( 0., 0., 0.)
                };

                for( int i = 0 ; i < 6 ; i++ )
                {
                    _directions[i] = _directions[i] * t.getBasis();
                }

                this->batchedUnitVectorGetSupportingVertexWithoutMargin(_directions, _supporting, 6);

                btVector3 aabbMin1(0,0,0),aabbMax1(0,0,0);

                for ( int i = 0; i < 3; ++i )
                {
                    aabbMax1[i] = t(_supporting[i])[i];
                    aabbMin1[i] = t(_supporting[i + 3])[i];
                }
                btVector3 marginVec(this->getMargin(),this->getMargin(),this->getMargin());
                aabbMin = aabbMin1-marginVec;
                aabbMax = aabbMax1+marginVec;
            }

            /// @brief Sets the scaling to be applied to the sharable/global child collision shape.
            virtual void setLocalScaling(const btVector3& scaling)
                { this->ChildConvexShape->setLocalScaling(scaling); }
            /// @brief Gets the scaling being applied to the sharable/global child collision shape.
            virtual const btVector3& getLocalScaling() const
                { return this->ChildConvexShape->getLocalScaling(); }
            /// @brief Sets the collision margin of the sharable/global child collision shape.
            virtual void setMargin(btScalar margin)
                { this->ChildConvexShape->setMargin(margin); }
            /// @brief Gets the collision margin of the sharable/global child collision shape.
            virtual btScalar getMargin() const
                { return this->ChildConvexShape->getMargin(); }
            /// @brief Gets the number of directions available for the first parameter in "getPreferredPenetrationDirection".
            virtual int	getNumPreferredPenetrationDirections() const
                { return this->ChildConvexShape->getNumPreferredPenetrationDirections(); }
            /// @brief Gets the direction objects should follow for penetration recovery at the specified index.
            virtual void getPreferredPenetrationDirection(int index, btVector3& penetrationVector) const
                { this->ChildConvexShape->getPreferredPenetrationDirection(index,penetrationVector); }
        };//ScalingShape

        ///////////////////////////////////////////////////////////////////////////////
        // PhysicsProxy Methods

        PhysicsProxy::PhysicsProxy() :
            WorldObjectShape(NULL),
            ScalerShape(NULL),
            CollisionGroup(0),
            CollisionMask(0)
        {
        }

        PhysicsProxy::~PhysicsProxy()
        {
        }

        ///////////////////////////////////////////////////////////////////////////////
        // Utility

        ///////////////////////////////////////////////////////////////////////////////
        // Collision Settings

        void PhysicsProxy::SetCollisionGroupAndMask(const Whole& Group, const Whole& Mask)
        {
            this->CollisionGroup = Group;
            this->CollisionMask = Mask;
        }

        Whole PhysicsProxy::GetCollisionGroup() const
        {
            return this->CollisionGroup;
        }

        Whole PhysicsProxy::GetCollisionMask() const
        {
            return this->CollisionMask;
        }

        void PhysicsProxy::SetCollisionShape(CollisionShape* Shape)
        {
            /*if( this->WorldObjectShape != Shape )
            {
                if( this->ScalerShape != NULL ) {
                    delete this->ScalerShape;
                    this->ScalerShape = NULL;
                }

                switch( Shape->GetType() )
                {
                    // All the basic convex shapes
                    case CollisionShape::ST_Box:
                    case CollisionShape::ST_Capsule:
                    case CollisionShape::ST_Compound:
                    case CollisionShape::ST_Cone:
                    case CollisionShape::ST_ConvexHull:
                    case CollisionShape::ST_Cylinder:
                    case CollisionShape::ST_MultiSphere:
                    case CollisionShape::ST_Sphere:
                    {

                    }

                    case CollisionShape::ST_Compound:

                    case CollisionShape::ST_DynamicTriMesh:
                    case CollisionShape::ST_Heightfield:
                    case CollisionShape::ST_Plane:
                    case CollisionShape::ST_ActorSoft:
                    case CollisionShape::ST_StaticTriMesh:
                }
            }//*/


            this->WorldObjectShape = Shape;
            this->_GetBasePhysicsObject()->setCollisionShape( Shape->_GetInternalShape() );
        }

        CollisionShape* PhysicsProxy::GetCollisionShape() const
        {
            return this->WorldObjectShape;
        }

        void PhysicsProxy::SetCollisionResponse(bool Enable)
        {
            if( Enable == this->GetCollisionResponse() )
                return;

            btCollisionObject* Base = this->_GetBasePhysicsObject();
            if(Enable) {
                Base->setCollisionFlags( Base->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE );
            }else{
                Base->setCollisionFlags( Base->getCollisionFlags() & ~btCollisionObject::CF_NO_CONTACT_RESPONSE );
            }
        }

        bool PhysicsProxy::GetCollisionResponse() const
        {
            return !(this->_GetBasePhysicsObject()->getCollisionFlags() & btCollisionObject::CF_NO_CONTACT_RESPONSE);
        }

        ///////////////////////////////////////////////////////////////////////////////
        // Static or Kinematic Properties

        void PhysicsProxy::SetKinematic()
        {
            btCollisionObject* Base = this->_GetBasePhysicsObject();
            Base->setCollisionFlags( Base->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT );
        }

        void PhysicsProxy::SetStatic()
        {
            btCollisionObject* Base = this->_GetBasePhysicsObject();
            Base->setCollisionFlags( Base->getCollisionFlags() | btCollisionObject::CF_STATIC_OBJECT );
        }

        bool PhysicsProxy::IsKinematic() const
        {
            return this->_GetBasePhysicsObject()->isKinematicObject();
        }

        bool PhysicsProxy::IsStatic() const
        {
            return this->_GetBasePhysicsObject()->isStaticObject();
        }

        bool PhysicsProxy::IsStaticOrKinematic() const
        {
            return this->_GetBasePhysicsObject()->isStaticOrKinematicObject();
        }

        ///////////////////////////////////////////////////////////////////////////////
        // Physics Properties

        void PhysicsProxy::SetFriction(const Real& Friction)
        {
            this->_GetBasePhysicsObject()->setFriction(Friction);
        }

        Real PhysicsProxy::GetFriction() const
        {
            return this->_GetBasePhysicsObject()->getFriction();
        }

        void PhysicsProxy::SetRestitution(const Real& Restitution)
        {
            this->_GetBasePhysicsObject()->setRestitution(Restitution);
        }

        Real PhysicsProxy::GetRestitution() const
        {
            return this->_GetBasePhysicsObject()->getRestitution();
        }

        ///////////////////////////////////////////////////////////////////////////////
        // Activation State

        bool PhysicsProxy::IsActive() const
        {
            return ( ACTIVE_TAG == this->_GetBasePhysicsObject()->getActivationState() );
        }

        void PhysicsProxy::SetActivationState(const Physics::WorldObjectActivationState State, bool Force)
        {
            if(Force) this->_GetBasePhysicsObject()->forceActivationState(State);
            else this->_GetBasePhysicsObject()->setActivationState(State);
        }

        ///////////////////////////////////////////////////////////////////////////////
        // Transform Methods

        void PhysicsProxy::SetLocation(const Vector3& Loc)
        {
            this->SetLocation(Loc.X,Loc.Y,Loc.Z);
        }

        void PhysicsProxy::SetLocation(const Real X, const Real Y, const Real Z)
        {
            btVector3 NewLoc(X,Y,Z);
            this->_GetBasePhysicsObject()->getWorldTransform().setOrigin(NewLoc);
            this->_GetBasePhysicsObject()->getInterpolationWorldTransform().setOrigin(NewLoc);
        }

        Vector3 PhysicsProxy::GetLocation() const
        {
            return Vector3( this->_GetBasePhysicsObject()->getWorldTransform().getOrigin() );
        }

        void PhysicsProxy::SetOrientation(const Quaternion& Ori)
        {
            this->SetOrientation(Ori.X,Ori.Y,Ori.Z,Ori.W);
        }

        void PhysicsProxy::SetOrientation(const Real X, const Real Y, const Real Z, const Real W)
        {
            btQuaternion NewRot(X,Y,Z,W);
            this->_GetBasePhysicsObject()->getWorldTransform().setRotation(NewRot);
            this->_GetBasePhysicsObject()->getInterpolationWorldTransform().setRotation(NewRot);
        }

        Quaternion PhysicsProxy::GetOrientation() const
        {
            return Quaternion( this->_GetBasePhysicsObject()->getWorldTransform().getRotation() );
        }

        void PhysicsProxy::SetScale(const Vector3& Sc)
        {

        }

        void PhysicsProxy::SetScale(const Real X, const Real Y, const Real Z)
        {

        }

        Vector3 PhysicsProxy::GetScale() const
        {

        }

        void PhysicsProxy::Translate(const Vector3& Trans)
        {
            this->SetLocation( this->GetLocation() + Trans );
        }

        void PhysicsProxy::Translate(const Real X, const Real Y, const Real Z)
        {
            Vector3 Trans(X,Y,Z);
            this->SetLocation( this->GetLocation() + Trans );
        }

        void PhysicsProxy::Yaw(const Real Angle)
        {
            Quaternion NewRot = this->GetOrientation() * Quaternion(Angle,Vector3::Unit_Y());
            this->SetOrientation(NewRot);
        }

        void PhysicsProxy::Pitch(const Real Angle)
        {
            Quaternion NewRot = this->GetOrientation() * Quaternion(Angle,Vector3::Unit_X());
            this->SetOrientation(NewRot);
        }

        void PhysicsProxy::Roll(const Real Angle)
        {
            Quaternion NewRot = this->GetOrientation() * Quaternion(Angle,Vector3::Unit_Z());
            this->SetOrientation(NewRot);
        }

        void PhysicsProxy::Rotate(const Vector3& Axis, const Real Angle)
        {
            Quaternion NewRot = this->GetOrientation() * Quaternion(Angle,Axis);
            this->SetOrientation(NewRot);
        }

        void PhysicsProxy::Rotate(const Quaternion& Rotation)
        {
            Quaternion NewRot = this->GetOrientation() * Rotation;
            this->SetOrientation(NewRot);
        }

        void PhysicsProxy::Scale(const Vector3& Scale)
        {

        }

        void PhysicsProxy::Scale(const Real X, const Real Y, const Real Z)
        {

        }
    }// Physics
}// Mezzanine

#endif
