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
#ifndef _uipagedcontainer_cpp
#define _uipagedcontainer_cpp

#include "UI/pagedcontainer.h"
#include "UI/pageprovider.h"
#include "UI/screen.h"

#include <algorithm>

namespace Mezzanine
{
    namespace UI
    {
        ///////////////////////////////////////////////////////////////////////////////
        // PagedContainer Static Members

        const String PagedContainer::TypeName                    = "PagedContainer";
        const EventNameType PagedContainer::EventChildSelected  = "ChildSelected";

        ///////////////////////////////////////////////////////////////////////////////
        // PagedContainer Methods

        PagedContainer::PagedContainer(Screen* Parent) :
            Widget(Parent),
            LastSelectedChild(NULL),
            XProvider(NULL),
            YProvider(NULL)
            { this->AddSubscriptionTable(PagedContainer::EventChildSelected); }

        PagedContainer::PagedContainer(const String& RendName, Screen* Parent) :
            Widget(RendName,Parent),
            LastSelectedChild(NULL),
            XProvider(NULL),
            YProvider(NULL)
            { this->AddSubscriptionTable(PagedContainer::EventChildSelected); }

        PagedContainer::PagedContainer(const String& RendName, const UnifiedRect& RendRect, Screen* Parent) :
            Widget(RendName,RendRect,Parent),
            LastSelectedChild(NULL),
            XProvider(NULL),
            YProvider(NULL)
            { this->AddSubscriptionTable(PagedContainer::EventChildSelected); }

        PagedContainer::~PagedContainer()
        {
            if( this->XProvider != NULL )
                this->XProvider->_SetContainer(NULL);
            if( this->YProvider != NULL )
                this->YProvider->_SetContainer(NULL);
        }

        void PagedContainer::ProtoSerializeImpl(XML::Node& SelfRoot) const
        {
            this->Widget::ProtoSerializeImpl(SelfRoot);
            this->ProtoSerializePageData(SelfRoot);
        }

        void PagedContainer::ProtoDeSerializeImpl(const XML::Node& SelfRoot)
        {
            this->Widget::ProtoDeSerializeImpl(SelfRoot);
            this->ProtoDeSerializePageData(SelfRoot);
        }

        void PagedContainer::HandleChildStateChangeImpl(Widget* Child, const UInt32& OldState, const UInt32& NewState)
        {
            if( !( OldState & WS_Focused ) && ( NewState & WS_Focused ) ) {
                Widget* DirectChild = this->GetClosestChild(Child);
                if( this->LastSelectedChild != DirectChild ) {
                    if( this->LastSelectedChild != NULL ) {
                        ChildSelectedEventPtr DeselectArgs = std::make_shared<ChildSelectedEvent>(EventChildSelected,this->Name,this->LastSelectedChild->GetName(),false);
                        this->DispatchEvent(EventChildSelected,&SubscriberType::operator(),DeselectArgs);
                    }
                    if( DirectChild != NULL ) {
                        ChildSelectedEventPtr SelectArgs = std::make_shared<ChildSelectedEvent>(EventChildSelected,this->Name,DirectChild->GetName(),true);
                        this->DispatchEvent(EventChildSelected,&SubscriberType::operator(),SelectArgs);
                    }
                    this->LastSelectedChild = DirectChild;
                }
            }
        }

        ///////////////////////////////////////////////////////////////////////////////
        // Utility

        const Vector2& PagedContainer::GetWorkAreaSize() const
        {
            return this->WorkAreaSize;
        }

        void PagedContainer::UpdateDimensions(const Rect& OldSelfRect, const Rect& NewSelfRect)
        {
            // Update the personal data first
            this->ActDims = NewSelfRect;

            // Perform the container specific update logic
            this->UpdateContainerDimensionsImpl(OldSelfRect,NewSelfRect);

            // We done got icky
            this->_MarkAllLayersDirty();
        }

        void PagedContainer::UpdateVisibleChildren()
        {
            Rect CurrDims = this->GetRect();
            this->UpdateContainerDimensionsImpl(CurrDims,CurrDims);
        }

        Widget* PagedContainer::GetLastSelectedChild() const
        {
            return this->LastSelectedChild;
        }

        void PagedContainer::ClearSelectedChild()
        {
            if( this->LastSelectedChild != NULL ) {
                ChildSelectedEventPtr DeselectArgs = std::make_shared<ChildSelectedEvent>(EventChildSelected,this->Name,this->LastSelectedChild->GetName(),false);
                this->DispatchEvent(EventChildSelected,&SubscriberType::operator(),DeselectArgs);
                this->LastSelectedChild = NULL;
            }
        }

        PagedContainer::ProviderMode PagedContainer::GetProviderConfig() const
        {
            if( this->XProvider != NULL && this->YProvider != NULL ) {
                return ( this->XProvider == this->YProvider ? PagedContainer::PM_Single_XY : PagedContainer::PM_Dual_XY );
            }else if( this->XProvider != NULL ) {
                return PagedContainer::PM_Single_X;
            }else if( this->YProvider != NULL ) {
                return PagedContainer::PM_Single_Y;
            }else{
                return PagedContainer::PM_Error;
            }
        }

        PagedContainer::ProviderMode PagedContainer::GetProviderConfig(const PageProvider* Prov) const
        {
            if( this->XProvider == Prov && this->YProvider == Prov ) {
                return PagedContainer::PM_Single_XY;
            }else if( this->XProvider == Prov ) {
                return PagedContainer::PM_Single_X;
            }else if( this->YProvider == Prov ) {
                return PagedContainer::PM_Single_Y;
            }else{
                return PagedContainer::PM_Error;
            }
        }

        ///////////////////////////////////////////////////////////////////////////////
        // Visibility and Priority Methods

        void PagedContainer::SetVisible(Boole CanSee)
        {
            if( this->Visible != CanSee ) {
                if(CanSee) {
                    this->_OnVisibilityShown();
                }else{
                    this->_OnVisibilityHidden();
                }

                for( VisibleChildIterator It = this->VisibleChildren.begin() ; It != this->VisibleChildren.end() ; ++It )
                    { (*It)->SetVisible(CanSee); }
            }
        }

        Boole PagedContainer::GetVisible() const
        {
            return this->Visible;
        }

        Boole PagedContainer::IsVisible() const
        {
            if( this->ParentQuad ) {
                return this->Visible && this->ParentQuad->IsVisible();
            }else{
                return this->Visible;
            }
        }

        void PagedContainer::Show()
        {
            if( this->Visible == false ) {
                this->_OnVisibilityShown();
                for( VisibleChildIterator It = this->VisibleChildren.begin() ; It != this->VisibleChildren.end() ; ++It )
                    { (*It)->Show(); }
            }
        }

        void PagedContainer::Hide()
        {
            if( this->Visible == true ) {
                this->_OnVisibilityHidden();
                for( VisibleChildIterator It = this->VisibleChildren.begin() ; It != this->VisibleChildren.end() ; ++It )
                    { (*It)->Hide(); }
            }
        }

        ///////////////////////////////////////////////////////////////////////////////
        // PagedContainer Configuration

        void PagedContainer::SetXYProvider(PageProvider* Prov)
            { this->SetProviders(Prov,Prov); }

        PageProvider* PagedContainer::GetXProvider() const
            { return this->XProvider; }

        PageProvider* PagedContainer::GetYProvider() const
            { return this->YProvider; }

        void PagedContainer::UnbindProvider(PageProvider* Prov)
        {
            if( this->XProvider == Prov ) {
                if( this->YProvider != Prov ) {
                    this->XProvider->_SetContainer(NULL);
                }
                this->XProvider = NULL;
            }
            if( this->YProvider == Prov ) {
                if( this->XProvider != Prov ) {
                    this->YProvider->_SetContainer(NULL);
                }
                this->YProvider = NULL;
            }
        }

        ///////////////////////////////////////////////////////////////////////////////
        // Child Management

        void PagedContainer::AddChild(Widget* Child)
        {
            this->QuadRenderable::AddChild(Child);
            this->QuickUpdateWorkAreaSize(Child->GetUnifiedSize(),true);
        }

        void PagedContainer::AddChild(Widget* Child, const UInt16 ZOrder)
        {
            this->QuadRenderable::AddChild(Child,ZOrder);
        }

        void PagedContainer::RemoveChild(Widget* ToBeRemoved)
        {
            if( this->LastSelectedChild == ToBeRemoved ) {
                this->LastSelectedChild = NULL;
            }
            this->QuickUpdateWorkAreaSize(ToBeRemoved->GetUnifiedSize(),false);
            this->QuadRenderable::RemoveChild(ToBeRemoved);

            VisibleChildIterator ChildIt = std::find(this->VisibleChildren.begin(),this->VisibleChildren.end(),ToBeRemoved);
            if( ChildIt != this->VisibleChildren.end() ) {
                this->VisibleChildren.erase(ChildIt);
            }
        }

        void PagedContainer::RemoveAllChildren()
        {
            this->LastSelectedChild = NULL;
            for( ChildIterator It = this->ChildWidgets.begin() ; It != this->ChildWidgets.end() ; ++It )
            {
                (*It)->_NotifyParenthood(NULL);
            }
            this->ChildWidgets.clear();
            this->VisibleChildren.clear();
            this->UpdateWorkAreaSize();
            this->_MarkDirty();
        }

        void PagedContainer::DestroyChild(Widget* ToBeDestroyed)
        {
            if( this->LastSelectedChild == ToBeDestroyed ) {
                this->LastSelectedChild = NULL;
            }
            this->QuickUpdateWorkAreaSize(ToBeDestroyed->GetUnifiedSize(),false);
            this->QuadRenderable::DestroyChild(ToBeDestroyed);

            VisibleChildIterator ChildIt = std::find(this->VisibleChildren.begin(),this->VisibleChildren.end(),ToBeDestroyed);
            if( ChildIt != this->VisibleChildren.end() ) {
                this->VisibleChildren.erase(ChildIt);
            }
        }

        void PagedContainer::DestroyAllChildren()
        {
            this->LastSelectedChild = NULL;
            for( ChildIterator It = this->ChildWidgets.begin() ; It != this->ChildWidgets.end() ; ++It )
            {
                (*It)->_NotifyParenthood(NULL);
                this->ParentScreen->DestroyWidget( (*It) );
            }
            this->ChildWidgets.clear();
            this->VisibleChildren.clear();
            this->UpdateWorkAreaSize();
            this->_MarkDirty();
        }

        ///////////////////////////////////////////////////////////////////////////////
        // Serialization

        void PagedContainer::ProtoSerializePageData(XML::Node& SelfRoot) const
        {
            if( this->XProvider != NULL || this->YProvider != NULL ) {
                XML::Node PageDataNode = SelfRoot.AppendChild( PagedContainer::GetSerializableName() + "PageData" );
                if( PageDataNode.AppendAttribute("Version").SetValue("1") )
                {
                    if( this->XProvider != NULL ) {
                        XML::Node XProviderNode = PageDataNode.AppendChild("XProvider");
                        if( XProviderNode.AppendAttribute("Name").SetValue( this->XProvider->GetName() ) &&
                            XProviderNode.AppendAttribute("Config").SetValue( this->GetProviderConfig( this->XProvider ) ) )
                        {
                            return;
                        }else{
                            SerializeError("Create XML Attribute Values",PagedContainer::GetSerializableName() + "PageData",true);
                        }
                    }

                    if( this->YProvider != NULL ) {
                        XML::Node YProviderNode = PageDataNode.AppendChild("YProvider");
                        if( YProviderNode.AppendAttribute("Name").SetValue( this->YProvider->GetName() ) &&
                            YProviderNode.AppendAttribute("Config").SetValue( this->GetProviderConfig( this->YProvider ) ) )
                        {
                            return;
                        }else{
                            SerializeError("Create XML Attribute Values",PagedContainer::GetSerializableName() + "PageData",true);
                        }
                    }

                    return;
                }else{
                    SerializeError("Create XML Attribute Values",PagedContainer::GetSerializableName() + "PageData",true);
                }
            }
        }

        void PagedContainer::ProtoSerializeProperties(XML::Node& SelfRoot) const
        {
            this->Widget::ProtoSerializeProperties(SelfRoot);

            XML::Node PropertiesNode = SelfRoot.AppendChild( PagedContainer::GetSerializableName() + "Properties" );

            if( PropertiesNode.AppendAttribute("Version").SetValue("1") )
            {
                XML::Node WorkAreaSizeNode = PropertiesNode.AppendChild("WorkAreaSize");
                this->WorkAreaSize.ProtoSerialize(WorkAreaSizeNode);

                return;
            }else{
                SerializeError("Create XML Attribute Values",PagedContainer::GetSerializableName() + "Properties",true);
            }
        }

        void PagedContainer::ProtoDeSerializePageData(const XML::Node& SelfRoot)
        {
            XML::Attribute CurrAttrib;
            XML::Node PageDataNode = SelfRoot.GetChild( PagedContainer::GetSerializableName() + "PageData" );

            if( !PageDataNode.Empty() ) {
                if(PageDataNode.GetAttribute("Version").AsInt() == 1) {
                    XML::Node XProviderNode = PageDataNode.GetChild("XProvider");
                    if( !XProviderNode.Empty() ) {
                        String ProviderName;
                        ProviderMode ProviderConfig;

                        CurrAttrib = XProviderNode.GetAttribute("Name");
                        if( !CurrAttrib.Empty() )
                            ProviderName = CurrAttrib.AsString();

                        CurrAttrib = XProviderNode.GetAttribute("Config");
                        if( !CurrAttrib.Empty() )
                            ProviderConfig = static_cast<ProviderMode>( CurrAttrib.AsWhole() );

                        if( !ProviderName.empty() ) {
                            /// @todo This is a blind cast and can cause some issues if the configuration is edited externally such that the named
                            /// widget isn't a provider.  Some check may being added may be warrented.
                            PageProvider* Casted = static_cast<PageProvider*>( this->ParentScreen->GetWidget(ProviderName) );
                            if( Casted ) {
                                switch( ProviderConfig )
                                {
                                    case PagedContainer::PM_Single_X:    this->SetXProvider(Casted);          break;
                                    case PagedContainer::PM_Single_Y:    this->SetYProvider(Casted);          break;
                                    case PagedContainer::PM_Single_XY:   this->SetProviders(Casted,Casted);   break;
                                    default:                             /* Do Nothing */                     break;
                                }
                            }
                        }
                    }

                    XML::Node YProviderNode = PageDataNode.GetChild("YProvider");
                    if( !YProviderNode.Empty() ) {
                        String ProviderName;
                        ProviderMode ProviderConfig;

                        CurrAttrib = YProviderNode.GetAttribute("Name");
                        if( !CurrAttrib.Empty() )
                            ProviderName = CurrAttrib.AsString();

                        CurrAttrib = YProviderNode.GetAttribute("Config");
                        if( !CurrAttrib.Empty() )
                            ProviderConfig = static_cast<ProviderMode>( CurrAttrib.AsWhole() );

                        if( !ProviderName.empty() ) {
                            /// @todo This is a blind cast and can cause some issues if the configuration is edited externally such that the named
                            /// widget isn't a provider.  Some check may being added may be warrented.
                            PageProvider* Casted = static_cast<PageProvider*>( this->ParentScreen->GetWidget(ProviderName) );
                            if( Casted ) {
                                switch( ProviderConfig )
                                {
                                    case PagedContainer::PM_Single_X:    this->SetXProvider(Casted);          break;
                                    case PagedContainer::PM_Single_Y:    this->SetYProvider(Casted);          break;
                                    case PagedContainer::PM_Single_XY:   this->SetProviders(Casted,Casted);   break;
                                    default:                             /* Do Nothing */                     break;
                                }
                            }
                        }
                    }
                }else{
                    MEZZ_EXCEPTION(ExceptionBase::INVALID_VERSION_EXCEPTION,"Incompatible XML Version for " + (PagedContainer::GetSerializableName() + "PageData") + ": Not Version 1.");
                }
            }else{
                MEZZ_EXCEPTION(ExceptionBase::II_IDENTITY_NOT_FOUND_EXCEPTION,PagedContainer::GetSerializableName() + "PageData" + " was not found in the provided XML node, which was expected.");
            }
        }

        void PagedContainer::ProtoDeSerializeProperties(const XML::Node& SelfRoot)
        {
            this->Widget::ProtoDeSerializeProperties(SelfRoot);

            //XML::Attribute CurrAttrib;
            XML::Node PropertiesNode = SelfRoot.GetChild( PagedContainer::GetSerializableName() + "Properties" );

            if( !PropertiesNode.Empty() ) {
                if(PropertiesNode.GetAttribute("Version").AsInt() == 1) {
                    XML::Node WorkAreaSizeNode = PropertiesNode.GetChild("WorkAreaSize");
                    this->WorkAreaSize.ProtoDeSerialize(WorkAreaSizeNode);
                }else{
                    MEZZ_EXCEPTION(ExceptionBase::INVALID_VERSION_EXCEPTION,"Incompatible XML Version for " + (PagedContainer::GetSerializableName() + "Properties") + ": Not Version 1.");
                }
            }else{
                MEZZ_EXCEPTION(ExceptionBase::II_IDENTITY_NOT_FOUND_EXCEPTION,PagedContainer::GetSerializableName() + "Properties" + " was not found in the provided XML node, which was expected.");
            }
        }

        String PagedContainer::GetSerializableName()
        {
            return PagedContainer::TypeName;
        }

        ///////////////////////////////////////////////////////////////////////////////
        // Internal Event Methods

        void PagedContainer::_OnChildSelected(const String& ChildName, const Boole Selected)
        {
            ChildSelectedEventPtr Args = std::make_shared<ChildSelectedEvent>(EventChildSelected,this->Name,ChildName,Selected);
            this->DispatchEvent(EventChildSelected,&SubscriberType::operator(),Args);
        }

        ///////////////////////////////////////////////////////////////////////////////
        // Internal Methods

        void PagedContainer::_AppendRenderDataCascading(ScreenRenderData& RenderData)
        {
            // Update the children based on page positions and the container size so we can grab the proper vertices.
            if(this->VertexCache) {
                if( this->Dirty || this->AllLayersDirty ) {
                    this->VertexCache->Clear();
                    this->_AppendRenderData(*VertexCache);
                    for( VisibleChildIterator ChildIt = this->VisibleChildren.begin() ; ChildIt != this->VisibleChildren.end() ; ++ChildIt )
                        { (*ChildIt)->_AppendRenderDataCascading(*VertexCache); }
                }
                RenderData.Append(*VertexCache);
            }else{
                this->_AppendRenderData(RenderData);
                for( VisibleChildIterator It = this->VisibleChildren.begin() ; It != this->VisibleChildren.end() ; ++It )
                    { (*It)->_AppendRenderDataCascading(RenderData); }
            }
        }
    }//UI
}//Mezzanine

#endif
