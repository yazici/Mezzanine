// © Copyright 2010 - 2017 BlackTopp Studios Inc.
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
#ifndef _graphicsitemproxy_cpp
#define _graphicsitemproxy_cpp

/// @file
/// @brief This file contains the implementation for the World proxy wrapping basic entity(mesh) functionality.

#include "Graphics/itemproxy.h"
#include "Graphics/scenemanager.h"
#include "Graphics/meshmanager.h"
#include "Graphics/mesh.h"

#include "enumerations.h"
#include "exception.h"
#include "serialization.h"
#include "stringtool.h"

#include <OgreSceneManager.h>
#include <OgreSceneNode.h>
#include <OgreEntity.h>

namespace Mezzanine
{
    namespace Graphics
    {
        ItemProxy::ItemProxy(const UInt32 ID, SceneManager* Creator) :
            RenderableProxy(ID,Creator),
            GraphicsEntity(NULL),
            ProxyMesh(NULL),
            RenderDist(0),
            LightMask(0xFFFFFFFF),
            SceneVisible(true),
            CanCastShadows(true)
            { this->CreateEntity(NULL); }

        ItemProxy::ItemProxy(const UInt32 ID, Mesh* TheMesh, SceneManager* Creator) :
            RenderableProxy(ID,Creator),
            GraphicsEntity(NULL),
            ProxyMesh(NULL),
            RenderDist(0),
            LightMask(0xFFFFFFFF),
            SceneVisible(true),
            CanCastShadows(true)
            { this->CreateEntity(TheMesh); }

        ItemProxy::ItemProxy(const UInt32 ID, const String& MeshName, const String& GroupName, SceneManager* Creator) :
            RenderableProxy(ID,Creator),
            GraphicsEntity(NULL),
            ProxyMesh(NULL),
            RenderDist(0),
            LightMask(0xFFFFFFFF),
            SceneVisible(true),
            CanCastShadows(true)
            { this->CreateEntity(MeshName,GroupName); }

        ItemProxy::ItemProxy(const XML::Node& SelfRoot, SceneManager* Creator) :
            RenderableProxy(Creator),
            GraphicsEntity(NULL),
            ProxyMesh(NULL),
            RenderDist(0),
            LightMask(0xFFFFFFFF),
            SceneVisible(true),
            CanCastShadows(true)
            { this->ProtoDeSerialize(SelfRoot); }

        ItemProxy::~ItemProxy()
            { this->DestroyEntity(); }

        void ItemProxy::CreateEntity(Mesh* ObjectMesh)
        {
            if( ObjectMesh != NULL ) {
                this->GraphicsEntity = this->Manager->_GetGraphicsWorldPointer()->createEntity( ObjectMesh->_GetInternalMesh() );
                this->GraphicsNode->attachObject( this->GraphicsEntity );
                this->GraphicsEntity->getUserObjectBindings().setUserAny( Ogre::Any( static_cast<RenderableProxy*>( this ) ) );
                this->GraphicsEntity->setVisibilityFlags(0);
                this->GraphicsEntity->setQueryFlags(0);
            }

            this->ProxyMesh = ObjectMesh;
        }

        void ItemProxy::CreateEntity(const String& MeshName, const String& GroupName)
        {
            Mesh* TheMesh = MeshManager::GetSingletonPtr()->LoadMesh(MeshName,GroupName);
            this->CreateEntity(TheMesh);
        }

        void ItemProxy::DestroyEntity()
        {
            if( this->GraphicsEntity ) {
                this->GraphicsNode->detachObject( this->GraphicsEntity );
                this->Manager->_GetGraphicsWorldPointer()->destroyEntity( this->GraphicsEntity );
            }
        }

        ///////////////////////////////////////////////////////////////////////////////
        // Utility

        Mezzanine::ComponentType ItemProxy::GetComponentType() const
        {
            return Mezzanine::CT_Graphics_ItemProxy;
        }

        Boole ItemProxy::IsStatic() const
        {
            return false;
        }

        void ItemProxy::Activate()
        {
            if( this->GraphicsEntity && !this->InWorld ) {
                this->GraphicsEntity->setVisibilityFlags( this->VisibilityMask );
                this->GraphicsEntity->setQueryFlags( this->QueryMask );
                this->InWorld = true;
            }
        }

        void ItemProxy::Deactivate()
        {
            if( this->GraphicsEntity && this->InWorld ) {
                this->GraphicsEntity->setVisibilityFlags(0);
                this->GraphicsEntity->setQueryFlags(0);
                this->InWorld = false;
            }
        }

        ///////////////////////////////////////////////////////////////////////////////
        // Mesh Management

        void ItemProxy::SetMesh(const String& MeshName, const String& Group)
        {
            if( this->GraphicsEntity ) {
                this->DestroyEntity();
            }
            this->CreateEntity(MeshName,Group);

            this->GraphicsEntity->setVisible( this->SceneVisible );
            this->GraphicsEntity->setCastShadows( this->CanCastShadows );
            this->GraphicsEntity->setLightMask( this->LightMask );
            this->GraphicsEntity->setVisibilityFlags( this->VisibilityMask );
            this->GraphicsEntity->setQueryFlags( this->QueryMask );
            this->GraphicsEntity->setRenderingDistance( this->RenderDist );
        }

        void ItemProxy::SetMesh(Mesh* ObjectMesh)
        {
            if( this->GraphicsEntity ) {
                this->DestroyEntity();
            }
            this->CreateEntity(ObjectMesh);

            this->GraphicsEntity->setVisible( this->SceneVisible );
            this->GraphicsEntity->setCastShadows( this->CanCastShadows );
            this->GraphicsEntity->setLightMask( this->LightMask );
            this->GraphicsEntity->setVisibilityFlags( this->VisibilityMask );
            this->GraphicsEntity->setQueryFlags( this->QueryMask );
            this->GraphicsEntity->setRenderingDistance( this->RenderDist );
        }

        Mesh* ItemProxy::GetMesh() const
        {
            return this->ProxyMesh;
        }

        ///////////////////////////////////////////////////////////////////////////////
        // RenderableProxy Properties

        void ItemProxy::SetVisible(const Boole Visible)
        {
            this->SceneVisible = Visible;
            if( this->GraphicsEntity ) {
                this->GraphicsEntity->setVisible( this->SceneVisible );
            }
        }

        Boole ItemProxy::GetVisible() const
        {
            return this->SceneVisible;
        }

        void ItemProxy::SetCastShadows(const Boole CastShadows)
        {
            this->CanCastShadows = CastShadows;
            if( this->GraphicsEntity ) {
                this->GraphicsEntity->setCastShadows( this->CanCastShadows );
            }
        }

        Boole ItemProxy::GetCastShadows() const
        {
            return this->CanCastShadows;
        }

        Boole ItemProxy::GetReceiveShadows() const
        {
            return ( this->GraphicsEntity ? this->GraphicsEntity->getReceivesShadows() : false );
        }

        void ItemProxy::SetLightMask(const UInt32 Mask)
        {
            this->LightMask = Mask;
            if( this->GraphicsEntity ) {
                this->GraphicsEntity->setLightMask( this->LightMask );
            }
        }

        UInt32 ItemProxy::GetLightMask() const
        {
            return this->LightMask;
        }

        void ItemProxy::SetVisibilityMask(const UInt32 Mask)
        {
            this->VisibilityMask = Mask;
            if( this->GraphicsEntity && this->InWorld ) {
                this->GraphicsEntity->setVisibilityFlags( this->VisibilityMask );
            }
        }

        UInt32 ItemProxy::GetVisibilityMask() const
        {
            return this->VisibilityMask;
        }

        void ItemProxy::SetQueryMask(const UInt32 Mask)
        {
            this->QueryMask = Mask;
            if( this->GraphicsEntity && this->InWorld ) {
                this->GraphicsEntity->setQueryFlags( this->QueryMask );
            }
        }

        UInt32 ItemProxy::GetQueryMask() const
        {
            return this->QueryMask;
        }

        void ItemProxy::SetRenderDistance(const Real Distance)
        {
            this->RenderDist = Distance;
            if( this->GraphicsEntity ) {
                this->GraphicsEntity->setRenderingDistance( this->RenderDist );
            }
        }

        Real ItemProxy::GetRenderDistance() const
        {
            return this->RenderDist;
        }

        ///////////////////////////////////////////////////////////////////////////////
        // Entity Properties

        ///////////////////////////////////////////////////////////////////////////////
        // Serialization

        void ItemProxy::ProtoSerialize(XML::Node& ParentNode) const
        {
            XML::Node SelfRoot = ParentNode.AppendChild(this->GetDerivedSerializableName());
            this->ProtoSerializeIsActivated(SelfRoot);
            this->ProtoSerializeProperties(SelfRoot);
            this->ProtoSerializeMesh(SelfRoot);
        }

        void ItemProxy::ProtoSerializeProperties(XML::Node& SelfRoot) const
        {
            this->RenderableProxy::ProtoSerializeProperties(SelfRoot);
        }

        void ItemProxy::ProtoSerializeMesh(XML::Node& SelfRoot) const
        {
            XML::Node MeshNode = SelfRoot.AppendChild( ItemProxy::GetSerializableName() + "Mesh" );

            if( MeshNode.AppendAttribute("Version").SetValue("1") &&
                MeshNode.AppendAttribute("ProxyMeshName").SetValue( this->ProxyMesh ? this->ProxyMesh->GetName() : "" ) &&
                MeshNode.AppendAttribute("ProxyMeshGroup").SetValue( this->ProxyMesh ? this->ProxyMesh->GetGroup() : "" ) )
            {
                return;
            }else{
                SerializeError("Create XML Attribute Values",ItemProxy::GetSerializableName() + "Mesh",true);
            }
        }

        void ItemProxy::ProtoDeSerialize(const XML::Node& SelfRoot)
        {
            this->ProtoDeSerializeProperties(SelfRoot);
            this->ProtoDeSerializeMesh(SelfRoot);
            this->ProtoDeSerializeIsActivated(SelfRoot);
        }

        void ItemProxy::ProtoDeSerializeProperties(const XML::Node& SelfRoot)
        {
            this->RenderableProxy::ProtoDeSerializeProperties(SelfRoot);
        }

        void ItemProxy::ProtoDeSerializeMesh(const XML::Node& SelfRoot)
        {
            XML::Attribute CurrAttrib;
            XML::Node MeshNode = SelfRoot.GetChild( ItemProxy::GetSerializableName() + "Mesh" );

            if( !MeshNode.Empty() ) {
                if(MeshNode.GetAttribute("Version").AsInt() == 1) {
                    String MeshName, MeshGroup = Ogre::ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME;

                    CurrAttrib = MeshNode.GetAttribute("ProxyMeshName");
                    if( !CurrAttrib.Empty() )
                        MeshName = CurrAttrib.AsString();

                    CurrAttrib = MeshNode.GetAttribute("ProxyMeshGroup");
                    if( !CurrAttrib.Empty() )
                        MeshGroup = CurrAttrib.AsString();

                    if( !MeshName.empty() && !MeshGroup.empty() ) {
                        Mesh* NewMesh = MeshManager::GetSingletonPtr()->LoadMesh( MeshName, MeshGroup );
                        this->SetMesh( NewMesh );
                    }else{
                        this->SetMesh( NULL );
                    }
                }
            }
        }

        String ItemProxy::GetDerivedSerializableName() const
            { return ItemProxy::GetSerializableName(); }

        String ItemProxy::GetSerializableName()
            { return "ItemProxy"; }

        ///////////////////////////////////////////////////////////////////////////////
        // Internal Methods

        Ogre::Entity* ItemProxy::_GetGraphicsObject() const
            { return this->GraphicsEntity; }

        Ogre::MovableObject* ItemProxy::_GetBaseGraphicsObject() const
            { return this->GraphicsEntity; }
    }//Graphics
}//Mezzanine

#endif
