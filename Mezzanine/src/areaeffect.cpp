//� Copyright 2010 - 2011 BlackTopp Studios Inc.
/* This file is part of The Mezzanine Engine.

    The Mezzanine Engine is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    The Mezzanine Engine is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with The Mezzanine Engine.  If not, see <http://www.gnu.org/licenses/>.
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
#ifndef _areaeffect_cpp
#define _areaeffect_cpp

#include "areaeffect.h"
#include "actorrigid.h"
#include "physicsmanager.h"
#include "collisionshape.h"
#include "collisionshapemanager.h"
#include "mesh.h"
#include "meshmanager.h"
#include "objectreference.h"
#include "internalmeshtools.h.cpp"
#include "world.h"

#include <BulletCollision/CollisionDispatch/btGhostObject.h>
#include <BulletCollision/Gimpact/btGImpactShape.h>
#include <BulletSoftBody/btSoftRigidDynamicsWorld.h>

#ifdef GetObject
#undef GetObject
#endif

namespace Mezzanine
{
    AreaEffect::AreaEffect(const String &name, const Vector3& Location)
    {
        Name = name;
        this->GraphicsNode = SceneManager::GetSingletonPtr()->GetGraphicsWorldPointer()->getRootSceneNode()->createChildSceneNode();
        CreateGhostObject(Location);

        short CollisionGroup = btBroadphaseProxy::SensorTrigger;
        short CollisionMask = GetPhysicsSettings()->IsStatic() ? btBroadphaseProxy::AllFilter ^ (btBroadphaseProxy::SensorTrigger|btBroadphaseProxy::StaticFilter) : btBroadphaseProxy::AllFilter ^ btBroadphaseProxy::SensorTrigger;
        PhysicsSettings->SetCollisionGroupAndMask(CollisionGroup,CollisionMask);
    }

    AreaEffect::~AreaEffect()
    {
        delete (ObjectReference*)Ghost->getUserPointer();
        delete Ghost;
        delete GraphicsSettings;
        delete PhysicsSettings;

        Ogre::SceneManager* OgreManager = SceneManager::GetSingletonPtr()->GetGraphicsWorldPointer();
        if(GraphicsObject)
            OgreManager->destroyEntity(GraphicsObject);
        OgreManager->destroySceneNode(GraphicsNode);
    }

    void AreaEffect::CreateGhostObject(const Vector3& Location)
    {
        Ghost = new btPairCachingGhostObject();
        Ghost->setCollisionFlags(Ghost->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
        Ghost->getWorldTransform().setOrigin(Location.GetBulletVector3());
        ObjectReference* AERef = new ObjectReference(this->GetType(),this);
        Ghost->setUserPointer(AERef);

        PhysicsObject = Ghost;
        this->GraphicsSettings = new WorldObjectGraphicsSettings(this,GraphicsObject);
        this->PhysicsSettings = new WorldObjectPhysicsSettings(this,PhysicsObject);
    }

    void AreaEffect::AddActorToList(ActorBase* Actor)
    {
        AddedActors.push_back(Actor);
        OverlappingActors.push_back(Actor);
    }

    void AreaEffect::RemoveActorFromList(ActorBase* Actor)
    {
        RemovedActors.push_back(Actor);
        for( std::list<ActorBase*>::iterator c=OverlappingActors.begin(); c!=OverlappingActors.end(); c++)
        {
            if ( Actor == (*c) )
            {
                OverlappingActors.erase(c);
            }
        }
    }

    Whole AreaEffect::GetNumOverlappingActors()
    {
        return OverlappingActors.size();
    }

    Whole AreaEffect::GetNumAddedActors()
    {
        return AddedActors.size();
    }

    Whole AreaEffect::GetNumRemovedActors()
    {
        return RemovedActors.size();
    }

    std::list<ActorBase*>& AreaEffect::GetOverlappingActors()
    {
        return OverlappingActors;
    }

    std::vector<ActorBase*>& AreaEffect::GetAddedActors()
    {
        return AddedActors;
    }

    std::vector<ActorBase*>& AreaEffect::GetRemovedActors()
    {
        return RemovedActors;
    }

    ConstString& AreaEffect::GetName() const
    {
        return WorldObject::GetName();
    }

    void AreaEffect::SetLocation(const Vector3& Location)
    {
        WorldObject::SetLocation(Location);
        LocalTransformDirty = true;

        _RecalculateLocalTransform();
        _RecalculateAllChildTransforms();
    }

    Vector3 AreaEffect::GetLocation() const
    {
        return WorldObject::GetLocation();
    }

    void AreaEffect::SetOrientation(const Quaternion& Rotation)
    {
        NonStaticWorldObject::InternalSetOrientation(Rotation);
        LocalTransformDirty = true;

        _RecalculateLocalTransform();
        _RecalculateAllChildTransforms();
    }

    Quaternion AreaEffect::GetOrientation() const
    {
        return NonStaticWorldObject::GetOrientation();
    }

    void AreaEffect::SetScaling(const Vector3& Scale)
    {
        WorldObject::SetScaling(Scale);
        LocalTransformDirty = true;

        _RecalculateLocalTransform();
        _RecalculateAllChildTransforms();
    }

    Vector3 AreaEffect::GetScaling() const
    {
        return WorldObject::GetScaling();
    }

    void AreaEffect::SetLocalLocation(const Vector3& Location)
    {
        LocalXform.Location = Location;
        GlobalTransformDirty = true;

        _RecalculateGlobalTransform();
        _RecalculateAllChildTransforms();
    }

    void AreaEffect::SetLocalOrientation(const Quaternion& Orientation)
    {
        LocalXform.Rotation = Orientation;
        GlobalTransformDirty = true;

        _RecalculateGlobalTransform();
        _RecalculateAllChildTransforms();
    }

    WorldAndSceneObjectType AreaEffect::GetType() const
    {
        return Mezzanine::WSO_AEUnknown;
    }

    void AreaEffect::AddToWorld()
    {
        PhysicsManager::GetSingletonPtr()->GetPhysicsWorldPointer()->addCollisionObject(this->PhysicsObject,GetPhysicsSettings()->GetCollisionGroup(),GetPhysicsSettings()->GetCollisionMask());
        this->AttachToGraphics();
    }

    void AreaEffect::RemoveFromWorld()
    {
        PhysicsManager* PhysMan = PhysicsManager::GetSingletonPtr();
        btSoftRigidDynamicsWorld* BWorld = PhysMan->GetPhysicsWorldPointer();
        //first remove any collision metadata
        /*if( !CurrentCollisions.empty() )
        {
            PhysMan->RemoveCollisionsContainingActor(this);
        }// */
        BWorld->removeCollisionObject(this->PhysicsObject);
        this->DetachFromGraphics();
    }

    void AreaEffect::_Update()
    {
        if ( !AddedActors.empty() )
            AddedActors.clear();
        if ( !RemovedActors.empty() )
            RemovedActors.clear();
        btSoftRigidDynamicsWorld* PhysWorld = PhysicsManager::GetSingletonPtr()->GetPhysicsWorldPointer();

        std::list<ActorBase*>::iterator it = OverlappingActors.begin();
        // Make a bool vector to keep track of which actors to keep when updating.
        std::vector<bool> Tracker;
        Tracker.resize(OverlappingActors.size());
        std::vector<bool>::iterator bit;
        for ( bit = Tracker.begin() ; bit != Tracker.end() ; bit++ )
        {
            (*bit) = false;
        }

        btManifoldArray manifoldArray;
        btBroadphasePairArray& pairArray = Ghost->getOverlappingPairCache()->getOverlappingPairArray();
        int numPairs = pairArray.size();
        for (int i=0;i<numPairs;i++)
        {
            manifoldArray.clear();
            const btBroadphasePair& pair = pairArray[i];
            btBroadphasePair* collisionPair = PhysWorld->getPairCache()->findPair(pair.m_pProxy0,pair.m_pProxy1);
            if (!collisionPair)
                continue;
            if (collisionPair->m_algorithm)
                collisionPair->m_algorithm->getAllContactManifolds(manifoldArray);

            for (int j=0;j<manifoldArray.size();j++)
            {
                btPersistentManifold* manifold = manifoldArray[j];
                for (int p=0;p<manifold->getNumContacts();p++)
                {
                    const btManifoldPoint& pt = manifold->getContactPoint(p);
                    //if(pt.m_distance1 > 0)
                    //    continue;
                    btCollisionObject* ColObj = manifold->getBody0() != Ghost ? (btCollisionObject*)(manifold->getBody0()) : (btCollisionObject*)(manifold->getBody1());

                    ObjectReference* ActorRef = (ObjectReference*)(ColObj->getUserPointer());
                    ActorBase* Actor = NULL;
                    if(Mezzanine::WSO_TerrainFirst > ActorRef->GetType())
                        Actor = (ActorBase*)ActorRef->GetObject();
                    else
                        continue;
                    // Check list for the actor in the pair.
                    for( it = OverlappingActors.begin(), bit = Tracker.begin() ; it != OverlappingActors.end() ; it++, bit++ )
                    {
                        if ( Actor == (*it) )
                        {
                            (*bit) = true;
                            break;
                        }
                    }
                    if ( it == OverlappingActors.end() )
                    {
                        AddActorToList(Actor);
                        Tracker.push_back(true);
                    }
                }
            }
        }

        // Verify they are the same size.  Then remove items from the list as necessary.
        if ( OverlappingActors.size() == Tracker.size() )
        {
            std::list<ActorBase*>::iterator sit = OverlappingActors.begin();
            for ( bit = Tracker.begin() ; bit != Tracker.end() ; )
            {
                if ( (*bit) == false )
                {
                    bit = Tracker.erase(bit);
                    ActorBase* Act = (*sit);
                    RemovedActors.push_back(Act);
                    sit = OverlappingActors.erase(sit);
                }else{
                    sit++;
                    bit++;
                }
            }
        }// */
    }

#ifdef MEZZXML
    void AreaEffect::ThrowSerialError(const String& Fail) const
        { SerializeError(Fail, SerializableName()); }

    String AreaEffect::GraphicsSettingsSerializableName() const
        { return String("WorldObjectGraphicsSettings"); }

    String AreaEffect::PhysicsSettingsSerializableName() const
        { return String("WorldObjectBasePhysicsSettings"); }

    void AreaEffect::ProtoSerialize(xml::Node& CurrentRoot) const
    {
        xml::Node AreaEffectNode = CurrentRoot.AppendChild("AreaEffect");
        if (!AreaEffectNode) { ThrowSerialError("create AreaEffectNode");}

        xml::Attribute AreaEffectVersion = AreaEffectNode.AppendAttribute("Version");
        AreaEffectVersion.SetValue(1);

        NonStaticWorldObject::ProtoSerialize(AreaEffectNode);
    }

    void AreaEffect::ProtoDeSerialize(const xml::Node& OneNode)
    {
        if ( Mezzanine::String(OneNode.Name())==this->AreaEffect::SerializableName() )
        {
            if(OneNode.GetAttribute("Version").AsInt() == 1)
            {
                NonStaticWorldObject::ProtoDeSerialize(OneNode.GetChild(this->NonStaticWorldObject::SerializableName()));
            }
        }
    }

    String AreaEffect::SerializableName()
        { return "AreaEffect"; }
#endif

    ///////////////////////////////////
    // TestAE functions

    TestAE::TestAE(const String& name, const Vector3& Location) : AreaEffect(name, Location)
    {
    }

    TestAE::~TestAE()
    {
    }

    void TestAE::ApplyEffect()
    {
        World* TheWorld = World::GetWorldPointer();
        std::vector<ActorBase*>::iterator AaRIt;
        std::list<ActorBase*>::iterator CurrIt;
        ActorBase* Act = NULL;

        if ( !AddedActors.empty() )
        {
            TheWorld->Log("Actors Added to field this frame:");
            for ( AaRIt = AddedActors.begin() ; AaRIt != AddedActors.end() ; AaRIt++ )
            {
                Act = (*AaRIt);
                TheWorld->Log(Act);
            }
        }
        if ( !RemovedActors.empty() )
        {
            TheWorld->Log("Actors Removed from field this frame:");
            for ( AaRIt = RemovedActors.begin() ; AaRIt != RemovedActors.end() ; AaRIt++ )
            {
                Act = (*AaRIt);
                TheWorld->Log(Act);
            }
        }
        if ( !OverlappingActors.empty() )
        {
            TheWorld->Log("Actors Currently in field this frame:");
            for ( CurrIt = OverlappingActors.begin() ; CurrIt != OverlappingActors.end() ; CurrIt++ )
            {
                Act = (*CurrIt);
                TheWorld->Log(Act);
            }
        }
    }

    ///////////////////////////////////
    // GravityField functions

    GravityField::GravityField(const String &name, const Vector3& Location) : AreaEffect(name, Location)
    {
    }

    GravityField::~GravityField()
    {
    }

    void GravityField::ApplyEffect()
    {
        std::vector<ActorBase*>::iterator It;
        PhysicsManager* Physics = PhysicsManager::GetSingletonPtr();
        ActorBase* Act = NULL;

        if ( !AddedActors.empty() )
        {
            for ( It = AddedActors.begin() ; It != AddedActors.end() ; It++ )
            {
                Act = (*It);
                Physics->SetIndividualGravity(Act, Grav);
            }
        }
        if ( !RemovedActors.empty() )
        {
            for ( It = RemovedActors.begin() ; It != RemovedActors.end() ; It++ )
            {
                Act = (*It);
                Physics->SetIndividualGravity(Act, Physics->GetGravity());
            }
        }
    }

    void GravityField::SetFieldGravity(const Vector3& Gravity)
    {
        Grav = Gravity;
    }

    Vector3 GravityField::GetFieldGravity() const
    {
        return Grav;
    }

    WorldAndSceneObjectType GravityField::GetType() const
    {
        return Mezzanine::WSO_AEGravityField;
    }

    ///////////////////////////////////
    // GravityWell functions

    GravityWell::GravityWell(const String &name, const Vector3& Location)
        : AreaEffect(name, Location),
          AllowWorldGrav(true),
          Strength(0),
          AttenAmount(0),
          AttenStyle(Mezzanine::Att_None)
    {
    }

    GravityWell::~GravityWell()
    {
    }

    void GravityWell::ApplyEffect()
    {
        if(0 == Strength)
            return;
        ActorBase* Act = NULL;
        ActorRigid* ActRig = NULL;
        if(!AllowWorldGrav && !AddedActors.empty())
        {
            for ( std::vector<ActorBase*>::iterator AA = AddedActors.begin() ; AA != AddedActors.end() ; AA++ )
            {
                if(Mezzanine::WSO_ActorRigid != (*AA)->GetType())
                    continue;
                ActRig = dynamic_cast<ActorRigid*>(*AA);
                ActRig->GetPhysicsSettings()->SetIndividualGravity(Vector3());
            }
        }
        if(!OverlappingActors.empty())
        {
            Vector3 ActorLoc, Direction;
            Real Distance, AppliedStrength, InvMass;
            Vector3 GhostLoc = this->GetLocation();
            for ( std::list<ActorBase*>::iterator OA = OverlappingActors.begin() ; OA != OverlappingActors.end() ; OA++ )
            {
                if(Mezzanine::WSO_ActorRigid != (*OA)->GetType())
                    continue;
                //Collect necessary data
                ActorLoc = (*OA)->GetLocation();
                Distance = ActorLoc.Distance(GhostLoc);
                Direction = (GhostLoc - ActorLoc) / Distance;
                switch(AttenStyle)
                {
                    case Mezzanine::Att_Linear:
                        AppliedStrength = Strength - (AttenAmount * Distance);
                        break;
                    case Mezzanine::Att_Quadratic:
                        AppliedStrength = Strength - (AttenAmount * (Distance * Distance));
                        break;
                    default:
                        AppliedStrength = Strength;
                        break;
                }
                ActRig = static_cast<ActorRigid*>(*OA);
                InvMass = ActRig->GetBulletObject()->getInvMass();
                if(0 != InvMass)
                    AppliedStrength *= (1 / ActRig->GetBulletObject()->getInvMass());
                else
                    AppliedStrength = 0;
                if(0 > AppliedStrength)
                    AppliedStrength = 0;
                //Apply the Force
                ActRig->GetBulletObject()->applyCentralForce((Direction * AppliedStrength).GetBulletVector3());
            }
        }
        if(!AllowWorldGrav && !RemovedActors.empty())
        {
            Vector3 WorldGrav = PhysicsManager::GetSingletonPtr()->GetGravity();
            for ( std::vector<ActorBase*>::iterator RA = RemovedActors.begin() ; RA != RemovedActors.end() ; RA++ )
            {
                if(Mezzanine::WSO_ActorRigid != (*RA)->GetType())
                    continue;
                ActRig = dynamic_cast<ActorRigid*>(*RA);
                ActRig->GetPhysicsSettings()->SetIndividualGravity(WorldGrav);
            }
        }
    }

    void GravityWell::SetFieldStrength(const Real& FieldStrength)
    {
        Strength = FieldStrength;
    }

    Real GravityWell::GetFieldStrength() const
    {
        return Strength;
    }

    void GravityWell::SetAllowWorldGravity(bool WorldGravity)
    {
        AllowWorldGrav = WorldGravity;
    }

    bool GravityWell::GetAllowWorldGravity() const
    {
        return AllowWorldGrav;
    }

    void GravityWell::SetAttenuation(const Real& Amount, const Mezzanine::AttenuationStyle& Style)
    {
        AttenAmount = Amount;
        AttenStyle = Style;
    }

    Mezzanine::AttenuationStyle GravityWell::GetAttenuationStyle() const
    {
        return AttenStyle;
    }

    Real GravityWell::GetAttenuationAmount() const
    {
        return AttenAmount;
    }

    WorldAndSceneObjectType GravityWell::GetType() const
    {
        return Mezzanine::WSO_AEGravityWell;
    }

    ///////////////////////////////////
    // FieldOfForce functions

    FieldOfForce::FieldOfForce(const String &name, const Vector3& Location)
        : AreaEffect(name, Location),
          Strength(0),
          AttenAmount(0),
          AttenStyle(Mezzanine::Att_None),
          AttenSource(Vector3(0,0,0)),
          Direction(Vector3(0,1,0))
    {
    }

    FieldOfForce::~FieldOfForce()
    {
    }

    void FieldOfForce::ApplyEffect()
    {
        if(0 == Strength)
            return;
        ActorBase* Act = NULL;
        ActorRigid* ActRig = NULL;
        if(!OverlappingActors.empty())
        {
            Vector3 ActorLoc;
            Real Distance, AppliedStrength, InvMass;
            for ( std::list<ActorBase*>::iterator OA = OverlappingActors.begin() ; OA != OverlappingActors.end() ; OA++ )
            {
                if(Mezzanine::WSO_ActorRigid != (*OA)->GetType())
                    continue;
                ActorLoc = (*OA)->GetLocation();
                switch(AttenStyle)
                {
                    case Mezzanine::Att_Linear:
                    {
                        Distance = ActorLoc.Distance(AttenSource);
                        AppliedStrength = Strength - (AttenAmount * Distance);
                        break;
                    }
                    case Mezzanine::Att_Quadratic:
                    {
                        Distance = ActorLoc.Distance(AttenSource);
                        AppliedStrength = Strength - (AttenAmount * (Distance * Distance));
                        break;
                    }
                    case Mezzanine::Att_None:
                    {
                        AppliedStrength = Strength;
                        break;
                    }
                }
                //Collect necessary data
                ActRig = static_cast<ActorRigid*>(*OA);
                //InvMass = ActRig->GetBulletObject()->getInvMass();
                //if(0 != InvMass)
                //    AppliedStrength *= (1 / ActRig->GetBulletObject()->getInvMass());
                //else
                //    AppliedStrength = 0;
                if(0 > AppliedStrength)
                    AppliedStrength = 0;
                //Apply the Force
                ActRig->GetBulletObject()->applyCentralForce((Direction * AppliedStrength).GetBulletVector3());
                //ActRig->GetBulletObject()->applyCentralImpulse((Direction * AppliedStrength).GetBulletVector3());
            }
        }
    }

    void FieldOfForce::SetFieldStrength(const Real& FieldStrength)
    {
        Strength = FieldStrength;
    }

    Real FieldOfForce::GetFieldStrength() const
    {
        return Strength;
    }

    void FieldOfForce::SetDirectionOfForce(const Vector3& ForceDirection)
    {
        Direction = ForceDirection;
    }

    Vector3 FieldOfForce::GetDirectionOfForce()
    {
        return Direction;
    }

    void FieldOfForce::SetAttenuation(const Real& Amount, const Mezzanine::AttenuationStyle& Style, const Vector3& Source)
    {
        AttenAmount = Amount;
        AttenStyle = Style;
        AttenSource = Source;
    }

    Mezzanine::AttenuationStyle FieldOfForce::GetAttenuationStyle() const
    {
        return AttenStyle;
    }

    Real FieldOfForce::GetAttenuationAmount() const
    {
        return AttenAmount;
    }

    Vector3 FieldOfForce::GetAttenuationSource() const
    {
        return AttenSource;
    }

    WorldAndSceneObjectType FieldOfForce::GetType() const
    {
        return Mezzanine::WSO_AEFieldOfForce;
    }
}

#endif
