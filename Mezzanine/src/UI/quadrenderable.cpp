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
#ifndef _uiquadrenderable_cpp
#define _uiquadrenderable_cpp

#include "UI/quadrenderable.h"

#include "UI/renderlayergroup.h"
#include "UI/layoutstrategy.h"
#include "UI/widget.h"
#include "UI/screen.h"

#include "UI/multiimagelayer.h"
#include "UI/singleimagelayer.h"
#include "UI/multilinetextlayer.h"
#include "UI/singlelinetextlayer.h"

#include "stringtool.h"
#include "serialization.h"

#include <algorithm>

namespace Mezzanine
{
    namespace UI
    {
        ///////////////////////////////////////////////////////////////////////////////
        // QuadRenderable Methods

        QuadRenderable::QuadRenderable(Screen* Parent) :
            Renderable(Parent),
            ParentQuad(NULL),
            ActiveGroup(NULL),
            LayoutStrat(NULL),
            VertexCache(NULL),
            ZOrder(0),
            MousePassthrough(false),
            ManualTransformUpdates(false),
            AllLayersDirty(false)
            { this->ActDims.SetIdentity(); }

        QuadRenderable::QuadRenderable(const String& RendName, Screen* Parent) :
            Renderable(RendName,Parent),
            ParentQuad(NULL),
            ActiveGroup(NULL),
            LayoutStrat(NULL),
            VertexCache(NULL),
            ZOrder(0),
            MousePassthrough(false),
            ManualTransformUpdates(false),
            AllLayersDirty(false)
            { this->ActDims.SetIdentity(); }

        QuadRenderable::QuadRenderable(const String& RendName, const UnifiedRect& RendRect, Screen* Parent) :
            Renderable(RendName,Parent),
            ParentQuad(NULL),
            ActiveGroup(NULL),
            LayoutStrat(NULL),
            VertexCache(NULL),
            ZOrder(0),
            MousePassthrough(false),
            ManualTransformUpdates(false),
            AllLayersDirty(false)
        {
            this->ActDims.SetIdentity();

            this->PositioningPolicy.UPosition = RendRect.Position;
            this->SizingPolicy.USize = RendRect.Size;
        }

        QuadRenderable::~QuadRenderable()
        {
            for( ChildIterator ChildIt = this->ChildWidgets.begin() ; ChildIt != this->ChildWidgets.end() ; ++ChildIt )
                { this->ParentScreen->DestroyWidget( (*ChildIt) ); }
            this->ChildWidgets.clear();
            this->DestroyAllRenderLayerGroups();
            this->DestroyAllRenderLayers();
            this->SetLocalVertexCaching(false);
            if( this->LayoutStrat != NULL ) {
                delete this->LayoutStrat;
                this->LayoutStrat = NULL;
            }
        }

        void QuadRenderable::ProtoSerializeImpl(XML::Node& SelfRoot) const
        {
            this->ProtoSerializeProperties(SelfRoot);
            this->ProtoSerializeRenderLayers(SelfRoot);
            this->ProtoSerializeRenderLayerGroups(SelfRoot);
        }

        void QuadRenderable::ProtoDeSerializeImpl(const XML::Node& SelfRoot)
        {
            // Get the render layers first in this case as our properties partially depend on them (ActiveGroup)
            this->ProtoDeSerializeRenderLayers(SelfRoot);
            this->ProtoDeSerializeRenderLayerGroups(SelfRoot);
            this->ProtoDeSerializeProperties(SelfRoot);
        }

        void QuadRenderable::AppendLayerVertices(std::vector<VertexData>& Vertices)
        {
            if( this->ActiveGroup != NULL ) {
                for( RenderLayerGroup::RenderLayerIterator It = this->ActiveGroup->RenderLayerBegin() ; It != this->ActiveGroup->RenderLayerEnd() ; ++It )
                    { (*It).second->_AppendVertices(Vertices); }
            }
        }

        void QuadRenderable::ResizeLayers(const Whole NewSize)
        {
            if( NewSize > this->RenderLayers.size() ) {
                Whole Pow2 = 1;
                while( Pow2 < NewSize )
                    Pow2 <<= 1;

                this->RenderLayers.resize(Pow2,NULL);
            }
        }

        RenderLayerGroup* QuadRenderable::CreateRenderLayerGroupNoCheck(const UInt16 ID)
        {
            RenderLayerGroup* NewGroup = new RenderLayerGroup(ID,this);
            if( this->RenderLayerGroups.empty() && this->ActiveGroup == NULL ) {
                this->SetActiveGroup( NewGroup );
            }
            this->RenderLayerGroups.push_back( NewGroup );
            return NewGroup;
        }

        ///////////////////////////////////////////////////////////////////////////////
        // Utility Methods

        const UInt16& QuadRenderable::GetZOrder() const
            { return this->ZOrder; }

        UInt16 QuadRenderable::GetHighestChildZOrder() const
            { return ( this->ChildWidgets.empty() ? 0 : this->ChildWidgets.back()->GetZOrder() ); }

        UInt16 QuadRenderable::GetLowestChildZOrder() const
            { return ( this->ChildWidgets.empty() ? 0 : this->ChildWidgets.front()->GetZOrder() ); }

        void QuadRenderable::UpdateChildOrder()
        {
            ChildContainer TempContainer;
            this->ChildWidgets.swap(TempContainer);
            for( ChildIterator TempIt = TempContainer.begin() ; TempIt != TempContainer.end() ; ++TempIt )
            {
                UInt16 Zorder = (*TempIt)->GetZOrder();
                for( ChildIterator ChildIt = this->ChildWidgets.begin() ; ChildIt != this->ChildWidgets.end() ; ++ChildIt )
                {
                    if( (*ChildIt)->GetZOrder() > Zorder ) {
                        this->ChildWidgets.insert(ChildIt,(*TempIt));
                        (*TempIt)->_MarkAllChildrenDirty();
                        return;
                    }
                }
                this->ChildWidgets.push_back(*TempIt);
                (*TempIt)->_MarkAllChildrenDirty();
            }
        }

        Boole QuadRenderable::CheckOverlap(const QuadRenderable* Quad) const
            { return this->GetRect().CheckOverlap(Quad->GetRect()); }

        Boole QuadRenderable::IsInside(const Vector2& Point) const
            { return this->GetRect().IsInside(Point); }

        Boole QuadRenderable::IsChildOfScreen() const
            { return (this->ParentScreen == this->ParentQuad); }

        void QuadRenderable::PopulateTextLinesInLayers(const Real MaxWidth)
        {
            for( RenderLayerIterator LayerIt = this->RenderLayers.begin() ; LayerIt != this->RenderLayers.end() ; ++LayerIt )
            {
                UI::RenderLayerType LayerType = (*LayerIt)->GetLayerType();
                if( LayerType == UI::RLT_MultiLineText || LayerType == UI::RLT_SingleLineText ) {
                    static_cast<TextLayer*>( *LayerIt )->PopulateTextLines(MaxWidth * (*LayerIt)->GetScale().X);
                }
            }
        }

        Real QuadRenderable::GetIdealHeightForText() const
        {
            Real Ret = 0;
            for( ConstRenderLayerIterator LayerIt = this->RenderLayers.begin() ; LayerIt != this->RenderLayers.end() ; ++LayerIt )
            {
                UI::RenderLayerType LayerType = (*LayerIt)->GetLayerType();
                if( LayerType == UI::RLT_MultiLineText || LayerType == UI::RLT_SingleLineText ) {
                    Real LayerHeight = static_cast<TextLayer*>( *LayerIt )->GetTotalHeight();
                    if( LayerHeight > Ret )
                        Ret = LayerHeight;
                }
            }
            return Ret;
        }

        void QuadRenderable::UpdateDimensions()
        {
            if( this->ParentQuad ) {
                Rect ParentRect = this->ParentQuad->GetRect();
                this->ParentQuad->UpdateDimensions(ParentRect,ParentRect);
            }
        }

        void QuadRenderable::UpdateChildDimensions()
        {
            Rect TempRect = this->GetRect();
            this->UpdateDimensions(TempRect,TempRect);
        }

        void QuadRenderable::UpdateDimensions(const Rect& OldSelfRect, const Rect& NewSelfRect)
        {
            // Update the personal data first
            this->ActDims = NewSelfRect;

            // Update the children, if we have a layout strat and the chidlren exist
            if( this->LayoutStrat != NULL && this->ChildWidgets.empty() == false )
                this->LayoutStrat->Layout(OldSelfRect,NewSelfRect,this->ChildWidgets);

            // We done got icky
            this->_MarkAllLayersDirty();
        }

        void QuadRenderable::SetMousePassthrough(Boole Enable)
            { this->MousePassthrough = Enable; }

        Boole QuadRenderable::GetMousePassthrough() const
            { return this->MousePassthrough; }

        void QuadRenderable::SetManualTransformUpdates(Boole Enable)
            { this->ManualTransformUpdates = Enable; }

        Boole QuadRenderable::GetManualTransformUpdates() const
            { return this->ManualTransformUpdates; }

        ///////////////////////////////////////////////////////////////////////////////
        // Transform Policy Methods

        void QuadRenderable::SetPositioningPolicy(const PositioningInfo& Policy)
        {
            if( this->PositioningPolicy != Policy ) {
                this->PositioningPolicy = Policy;
            }
        }

        const PositioningInfo& QuadRenderable::GetPositioningPolicy() const
        {
            return this->PositioningPolicy;
        }

        void QuadRenderable::SetSizingPolicy(const SizingInfo& Policy)
        {
            if( this->SizingPolicy != Policy ) {
                this->SizingPolicy = Policy;
            }
        }

        const SizingInfo& QuadRenderable::GetSizingPolicy() const
        {
            return this->SizingPolicy;
        }

        void QuadRenderable::SetHorizontalPositioningRules(const Whole Rules)
        {
            if( this->PositioningPolicy.HorizontalRules != Rules ) {
                this->PositioningPolicy.HorizontalRules = Rules;
            }
        }

        Whole QuadRenderable::GetHorizontalPositioningRules() const
        {
            return this->PositioningPolicy.HorizontalRules;
        }

        void QuadRenderable::SetVerticalPositioningRules(const Whole Rules)
        {
            if( this->PositioningPolicy.VerticalRules != Rules ) {
                this->PositioningPolicy.VerticalRules = Rules;
            }
        }

        Whole QuadRenderable::GetVerticalPositioningRules() const
        {
            return this->PositioningPolicy.VerticalRules;
        }

        void QuadRenderable::SetHorizontalSizingRules(const Whole Rules)
        {
            if( this->SizingPolicy.HorizontalRules != Rules ) {
                this->SizingPolicy.HorizontalRules = Rules;
            }
        }

        Whole QuadRenderable::GetHorizontalSizingRules() const
        {
            return this->SizingPolicy.HorizontalRules;
        }

        void QuadRenderable::SetVerticalSizingRules(const Whole Rules)
        {
            if( this->SizingPolicy.VerticalRules != Rules ) {
                this->SizingPolicy.VerticalRules = Rules;
            }
        }

        Whole QuadRenderable::GetVerticalSizingRules() const
        {
            return this->SizingPolicy.VerticalRules;
        }

        void QuadRenderable::SetAspectRatioLock(const UI::AspectRatioLock Lock)
        {
            if( this->SizingPolicy.RatioLock != Lock ) {
                this->SizingPolicy.RatioLock = Lock;
            }
        }

        UI::AspectRatioLock QuadRenderable::GetAspectRationLock() const
        {
            return this->SizingPolicy.RatioLock;
        }

        void QuadRenderable::SetMinSize(const UnifiedVec2& Min)
        {
            if( this->SizingPolicy.MinSize != Min ) {
                this->SizingPolicy.MinSize = Min;
            }
        }

        UnifiedVec2 QuadRenderable::GetMinSize() const
        {
            return this->SizingPolicy.MinSize;
        }

        void QuadRenderable::SetMaxSize(const UnifiedVec2& Max)
        {
            if( this->SizingPolicy.MaxSize != Max ) {
                this->SizingPolicy.MaxSize = Max;
            }
        }

        UnifiedVec2 QuadRenderable::GetMaxSize() const
        {
            return this->SizingPolicy.MaxSize;
        }

        ///////////////////////////////////////////////////////////////////////////////
        // RenderLayer Management

        SingleImageLayer* QuadRenderable::CreateSingleImageLayer()
        {
            SingleImageLayer* NewLayer = new SingleImageLayer(this);
            NewLayer->_UpdateIndex(this->RenderLayers.size());
            this->RenderLayers.push_back(NewLayer);
            this->_MarkDirty();
            return NewLayer;
        }

        SingleImageLayer* QuadRenderable::CreateSingleImageLayer(const UInt16 NormalZ, const UInt16 HoveredZ)
        {
            SingleImageLayer* NewLayer = this->CreateSingleImageLayer();
            this->AddLayerToExistingGroup(NewLayer,NormalZ,Widget::WG_Normal);
            this->AddLayerToExistingGroup(NewLayer,HoveredZ,Widget::WG_Hovered);
            return NewLayer;
        }

        SingleImageLayer* QuadRenderable::CreateSingleImageLayer(const GroupOrderEntry& GroupAndZ)
        {
            SingleImageLayer* NewLayer = this->CreateSingleImageLayer();
            this->AddLayerToGroup(NewLayer,GroupAndZ);
            return NewLayer;
        }

        SingleImageLayer* QuadRenderable::CreateSingleImageLayer(const String& SpriteName)
        {
            SingleImageLayer* NewLayer = this->CreateSingleImageLayer();
            NewLayer->SetSprite(SpriteName);
            return NewLayer;
        }

        SingleImageLayer* QuadRenderable::CreateSingleImageLayer(const String& SpriteName, const UInt16 NormalZ, const UInt16 HoveredZ)
        {
            SingleImageLayer* NewLayer = this->CreateSingleImageLayer(SpriteName);
            this->AddLayerToExistingGroup(NewLayer,NormalZ,Widget::WG_Normal);
            this->AddLayerToExistingGroup(NewLayer,HoveredZ,Widget::WG_Hovered);
            return NewLayer;
        }

        SingleImageLayer* QuadRenderable::CreateSingleImageLayer(const String& SpriteName, const GroupOrderEntry& GroupAndZ)
        {
            SingleImageLayer* NewLayer = this->CreateSingleImageLayer(GroupAndZ);
            NewLayer->SetSprite(SpriteName);
            return NewLayer;
        }

        MultiImageLayer* QuadRenderable::CreateMultiImageLayer()
        {
            MultiImageLayer* NewLayer = new MultiImageLayer(this);
            NewLayer->_UpdateIndex(this->RenderLayers.size());
            this->RenderLayers.push_back(NewLayer);
            this->_MarkDirty();
            return NewLayer;
        }

        MultiImageLayer* QuadRenderable::CreateMultiImageLayer(const UInt16 NormalZ, const UInt16 HoveredZ)
        {
            MultiImageLayer* NewLayer = this->CreateMultiImageLayer();
            this->AddLayerToExistingGroup(NewLayer,NormalZ,Widget::WG_Normal);
            this->AddLayerToExistingGroup(NewLayer,HoveredZ,Widget::WG_Hovered);
            return NewLayer;
        }

        MultiImageLayer* QuadRenderable::CreateMultiImageLayer(const GroupOrderEntry& GroupAndZ)
        {
            MultiImageLayer* NewLayer = this->CreateMultiImageLayer();
            this->AddLayerToGroup(NewLayer,GroupAndZ);
            return NewLayer;
        }

        SingleLineTextLayer* QuadRenderable::CreateSingleLineTextLayer()
        {
            SingleLineTextLayer* NewLayer = new SingleLineTextLayer(this);
            NewLayer->_UpdateIndex(this->RenderLayers.size());
            this->RenderLayers.push_back(NewLayer);
            this->_MarkDirty();
            return NewLayer;
        }

        SingleLineTextLayer* QuadRenderable::CreateSingleLineTextLayer(const UInt16 NormalZ, const UInt16 HoveredZ)
        {
            SingleLineTextLayer* NewLayer = this->CreateSingleLineTextLayer();
            this->AddLayerToExistingGroup(NewLayer,NormalZ,Widget::WG_Normal);
            this->AddLayerToExistingGroup(NewLayer,HoveredZ,Widget::WG_Hovered);
            return NewLayer;
        }

        SingleLineTextLayer* QuadRenderable::CreateSingleLineTextLayer(const GroupOrderEntry& GroupAndZ)
        {
            SingleLineTextLayer* NewLayer = this->CreateSingleLineTextLayer();
            this->AddLayerToGroup(NewLayer,GroupAndZ);
            return NewLayer;
        }

        SingleLineTextLayer* QuadRenderable::CreateSingleLineTextLayer(const String& FontName)
        {
            SingleLineTextLayer* NewLayer = new SingleLineTextLayer(FontName,this);
            NewLayer->_UpdateIndex(this->RenderLayers.size());
            this->RenderLayers.push_back(NewLayer);
            this->_MarkDirty();
            return NewLayer;
        }

        SingleLineTextLayer* QuadRenderable::CreateSingleLineTextLayer(const String& FontName, const UInt16 NormalZ, const UInt16 HoveredZ)
        {
            SingleLineTextLayer* NewLayer = this->CreateSingleLineTextLayer(FontName);
            this->AddLayerToExistingGroup(NewLayer,NormalZ,Widget::WG_Normal);
            this->AddLayerToExistingGroup(NewLayer,HoveredZ,Widget::WG_Hovered);
            return NewLayer;
        }

        SingleLineTextLayer* QuadRenderable::CreateSingleLineTextLayer(const String& FontName, const GroupOrderEntry& GroupAndZ)
        {
            SingleLineTextLayer* NewLayer = this->CreateSingleLineTextLayer(FontName);
            this->AddLayerToGroup(NewLayer,GroupAndZ);
            return NewLayer;
        }

        MultiLineTextLayer* QuadRenderable::CreateMultiLineTextLayer()
        {
            MultiLineTextLayer* NewLayer = new MultiLineTextLayer(this);
            NewLayer->_UpdateIndex(this->RenderLayers.size());
            this->RenderLayers.push_back(NewLayer);
            this->_MarkDirty();
            return NewLayer;
        }

        MultiLineTextLayer* QuadRenderable::CreateMultiLineTextLayer(const UInt16 NormalZ, const UInt16 HoveredZ)
        {
            MultiLineTextLayer* NewLayer = this->CreateMultiLineTextLayer();
            this->AddLayerToExistingGroup(NewLayer,NormalZ,Widget::WG_Normal);
            this->AddLayerToExistingGroup(NewLayer,HoveredZ,Widget::WG_Hovered);
            return NewLayer;
        }

        MultiLineTextLayer* QuadRenderable::CreateMultiLineTextLayer(const GroupOrderEntry& GroupAndZ)
        {
            MultiLineTextLayer* NewLayer = this->CreateMultiLineTextLayer();
            this->AddLayerToGroup(NewLayer,GroupAndZ);
            return NewLayer;
        }

        MultiLineTextLayer* QuadRenderable::CreateMultiLineTextLayer(const String& FontName)
        {
            MultiLineTextLayer* NewLayer = new MultiLineTextLayer(FontName,this);
            NewLayer->_UpdateIndex(this->RenderLayers.size());
            this->RenderLayers.push_back(NewLayer);
            this->_MarkDirty();
            return NewLayer;
        }

        MultiLineTextLayer* QuadRenderable::CreateMultiLineTextLayer(const String& FontName, const UInt16 NormalZ, const UInt16 HoveredZ)
        {
            MultiLineTextLayer* NewLayer = this->CreateMultiLineTextLayer(FontName);
            this->AddLayerToExistingGroup(NewLayer,NormalZ,Widget::WG_Normal);
            this->AddLayerToExistingGroup(NewLayer,HoveredZ,Widget::WG_Hovered);
            return NewLayer;
        }

        MultiLineTextLayer* QuadRenderable::CreateMultiLineTextLayer(const String& FontName, const GroupOrderEntry& GroupAndZ)
        {
            MultiLineTextLayer* NewLayer = this->CreateMultiLineTextLayer(FontName);
            this->AddLayerToGroup(NewLayer,GroupAndZ);
            return NewLayer;
        }

        RenderLayer* QuadRenderable::GetRenderLayer(const UInt32& Index) const
            { return this->RenderLayers.at(Index); }

        RenderLayer* QuadRenderable::GetRenderLayer(const Whole Which, const UI::RenderLayerType Type)
        {
            Whole Num = 0;
            for( RenderLayerIterator RendIt = this->RenderLayers.begin() ; RendIt != this->RenderLayers.end() ; ++RendIt )
            {
                if( (*RendIt)->GetLayerType() == Type ) {
                    if( Num == Which ) {
                        return (*RendIt);
                    }
                    ++Num;
                }
            }
            return NULL;
        }

        UInt32 QuadRenderable::GetNumRenderLayers() const
            { return this->RenderLayers.size(); }

        UInt32 QuadRenderable::GetNumVisibleRenderLayers() const
            { return ( this->ActiveGroup != NULL ? this->ActiveGroup->GetNumRenderLayers() : 0 ); }

        void QuadRenderable::DestroyRenderLayer(RenderLayer* ToBeDestroyed)
        {
            for( RenderLayerGroupIterator GroupIt = this->RenderLayerGroups.begin() ; GroupIt != this->RenderLayerGroups.end() ; ++GroupIt )
            {
                (*GroupIt)->RemoveLayer(ToBeDestroyed);
            }
            for( Whole Index = 0 ; Index < this->RenderLayers.size() ; ++Index )
            {
                if( ToBeDestroyed == this->RenderLayers[Index] ) {
                    if( Index != this->RenderLayers.size() - 1 ) {
                        // swap...
                        std::swap(this->RenderLayers[Index],this->RenderLayers[this->RenderLayers.size()-1]);
                        this->RenderLayers[Index]->_UpdateIndex(Index);
                    }
                    // ...and pop
                    this->RenderLayers.pop_back();
                    break;
                }
            }
            delete ToBeDestroyed;
        }

        void QuadRenderable::DestroyAllRenderLayers()
        {
            for( RenderLayerGroupIterator GroupIt = this->RenderLayerGroups.begin() ; GroupIt != this->RenderLayerGroups.end() ; ++GroupIt )
            {
                (*GroupIt)->RemoveAllLayers();
            }
            for( RenderLayerIterator It = this->RenderLayers.begin() ; It != this->RenderLayers.end() ; ++It )
            {
                delete (*It);
            }
            this->RenderLayers.clear();
        }

        QuadRenderable::RenderLayerIterator QuadRenderable::RenderLayerBegin()
            { return this->RenderLayers.begin(); }

        QuadRenderable::RenderLayerIterator QuadRenderable::RenderLayerEnd()
            { return this->RenderLayers.end(); }

        QuadRenderable::ConstRenderLayerIterator QuadRenderable::RenderLayerBegin() const
            { return this->RenderLayers.begin(); }

        QuadRenderable::ConstRenderLayerIterator QuadRenderable::RenderLayerEnd() const
            { return this->RenderLayers.end(); }

        ///////////////////////////////////////////////////////////////////////////////
        // RenderLayerGroup Management

        void QuadRenderable::SetActiveGroup(const UInt16 GroupID)
        {
            RenderLayerGroup* ToSet = this->GetRenderLayerGroup(GroupID);
            if( ToSet != NULL ) {
                this->SetActiveGroup( ToSet );
            }else{
                MEZZ_EXCEPTION(ExceptionBase::II_IDENTITY_NOT_FOUND_EXCEPTION,"RenderLayerGroup named \"" + Name + "\" does not exist in QuadRenderable: \"" + this->GetName() + "\"." );
            }
        }

        void QuadRenderable::SetActiveGroup(RenderLayerGroup* Group)
        {
            if( this->ActiveGroup != Group ) {
                // Out with the old
                if( this->ActiveGroup != NULL ) {
                    this->ActiveGroup->NotifyInactive();
                }
                // In with the new
                this->ActiveGroup = Group;
                if( this->ActiveGroup != NULL ) {
                    this->ActiveGroup->NotifyActive();
                }
                this->_MarkDirty();
            }
        }

        RenderLayerGroup* QuadRenderable::GetActiveGroup() const
            { return this->ActiveGroup; }

        Boole QuadRenderable::RenderLayerGroupExists(const UInt16 GroupID) const
            { return ( this->GetRenderLayerGroup(GroupID) != NULL ); }

        UInt32 QuadRenderable::GetNumRenderLayerGroups() const
            { return this->RenderLayerGroups.size(); }

        void QuadRenderable::AddLayerToGroup(RenderLayer* Layer, const UInt16 LayerZOrder, const UInt16 GroupID)
            { this->CreateOrRetrieveRenderLayerGroup(GroupID)->AddLayer(Layer,LayerZOrder); }

        void QuadRenderable::AddLayerToGroup(RenderLayer* Layer, const GroupOrderEntry& GroupAndZ)
            { this->AddLayerToGroup(Layer,GroupAndZ.LayerZOrder,GroupAndZ.GroupID); }

        void QuadRenderable::AddLayerToExistingGroup(RenderLayer* Layer, const UInt16 LayerZOrder, const UInt16 GroupID)
        {
            RenderLayerGroup* ExistingGroup = this->GetRenderLayerGroup(GroupID);
            if( ExistingGroup != NULL ) {
                ExistingGroup->AddLayer(Layer,LayerZOrder);
            }
        }

        void QuadRenderable::AddLayerToGroups(RenderLayer* Layer, const GroupOrderEntryVector& Entrys)
        {
            for( GroupOrderEntryVector::const_iterator It = Entrys.begin() ; It != Entrys.end() ; ++It )
                { this->CreateOrRetrieveRenderLayerGroup( (*It).GroupID )->AddLayer( Layer, (*It).LayerZOrder ); }
        }

        void QuadRenderable::RemoveLayerFromGroup(RenderLayer* Layer, const UInt16 GroupID)
        {
            RenderLayerGroup* GroupCheck = this->GetRenderLayerGroup(GroupID);
            if( GroupCheck != NULL ) {
                GroupCheck->RemoveLayer(Layer);
            }
        }

        void QuadRenderable::RemoveLayerFromAllGroups(RenderLayer* Layer)
        {
            for( RenderLayerGroupIterator GroupIt = this->RenderLayerGroups.begin() ; GroupIt != this->RenderLayerGroups.end() ; ++GroupIt )
                { (*GroupIt)->RemoveLayer(Layer); }
        }

        RenderLayerGroup* QuadRenderable::CreateRenderLayerGroup(const UInt16 GroupID)
        {
            RenderLayerGroup* GroupCheck = this->GetRenderLayerGroup(GroupID);
            if( GroupCheck == NULL ) {
                return this->CreateRenderLayerGroupNoCheck(GroupID);
            }else{
                MEZZ_EXCEPTION(ExceptionBase::II_DUPLICATE_IDENTITY_EXCEPTION,"RenderLayerGroup named \"" + Name + "\" already exists in QuadRenderable: \"" + this->GetName() + "\"." );
            }
            return NULL;//This should never happen, but compilation warnings are annoying.
        }

        RenderLayerGroup* QuadRenderable::CreateOrRetrieveRenderLayerGroup(const UInt16 GroupID)
        {
            RenderLayerGroup* Ret = this->GetRenderLayerGroup(GroupID);
            if( Ret == NULL ) {
                Ret = this->CreateRenderLayerGroupNoCheck(GroupID);
            }
            return Ret;
        }

        RenderLayerGroup* QuadRenderable::GetRenderLayerGroup(const UInt16 GroupID) const
        {
            for( ConstRenderLayerGroupIterator GroupIt = this->RenderLayerGroups.begin() ; GroupIt != this->RenderLayerGroups.end() ; ++GroupIt )
            {
                if( (*GroupIt)->GetGroupID() == GroupID ) {
                    return (*GroupIt);
                }
            }
            return NULL;
        }

        void QuadRenderable::DestroyRenderLayerGroup(const UInt16 GroupID)
        {
            for( RenderLayerGroupIterator GroupIt = this->RenderLayerGroups.begin() ; GroupIt != this->RenderLayerGroups.end() ; ++GroupIt )
            {
                if( (*GroupIt)->GetGroupID() == GroupID ) {
                    delete (*GroupIt);
                    this->RenderLayerGroups.erase(GroupIt);
                    return;
                }
            }
        }

        void QuadRenderable::DestroyRenderLayerGroup(RenderLayerGroup* ToBeDestroyed)
        {
            this->DestroyRenderLayerGroup(ToBeDestroyed->GetGroupID());
        }

        void QuadRenderable::DestroyAllRenderLayerGroups()
        {
            for( RenderLayerGroupIterator GroupIt = this->RenderLayerGroups.begin() ; GroupIt != this->RenderLayerGroups.end() ; ++GroupIt )
                { delete (*GroupIt); }
            this->RenderLayerGroups.clear();
        }

        QuadRenderable::RenderLayerGroupIterator QuadRenderable::RenderLayerGroupBegin()
            { return this->RenderLayerGroups.begin(); }

        QuadRenderable::RenderLayerGroupIterator QuadRenderable::RenderLayerGroupEnd()
            { return this->RenderLayerGroups.end(); }

        QuadRenderable::ConstRenderLayerGroupIterator QuadRenderable::RenderLayerGroupBegin() const
            { return this->RenderLayerGroups.begin(); }

        QuadRenderable::ConstRenderLayerGroupIterator QuadRenderable::RenderLayerGroupEnd() const
            { return this->RenderLayerGroups.end(); }

        ///////////////////////////////////////////////////////////////////////////////
        // Child Management

        void QuadRenderable::AddChild(Widget* Child)
        {
            if( this->GetRenderableType() == Renderable::RT_Screen ) {
                // If this is the screen adding a direct child, enable vertex caching
                Child->SetLocalVertexCaching(true);
            }

            UInt16 Zorder = Child->GetZOrder();
            for( ChildIterator It = this->ChildWidgets.begin() ; It != this->ChildWidgets.end() ; ++It )
            {
                if( (*It)->GetZOrder() > Zorder ) {
                    this->ChildWidgets.insert(It,Child);
                    Child->SetVisible( this->GetVisible() );
                    Child->_NotifyParenthood(this);
                    Child->_MarkAllChildrenDirty();
                    this->_MarkDirty();
                    return;
                }
            }
            this->ChildWidgets.push_back(Child);
            Child->SetVisible( this->GetVisible() );
            Child->_NotifyParenthood(this);
            Child->_MarkAllChildrenDirty();
            this->_MarkDirty();
        }

        void QuadRenderable::AddChild(Widget* Child, const UInt16 ZOrder)
        {
            Child->_SetZOrder(ZOrder);
            this->AddChild(Child);
        }

        Widget* QuadRenderable::GetChild(const UInt16 Zorder) const
        {
            for( ConstChildIterator ChildIt = this->ChildWidgets.begin() ; ChildIt != this->ChildWidgets.end() ; ++ChildIt )
            {
                if( Zorder == (*ChildIt)->GetZOrder() )
                    return (*ChildIt);
            }
            return NULL;
        }

        Widget* QuadRenderable::GetChild(const String& RendName) const
        {
            for( ConstChildIterator ChildIt = this->ChildWidgets.begin() ; ChildIt != this->ChildWidgets.end() ; ++ChildIt )
            {
                if( RendName == (*ChildIt)->GetName() )
                    return (*ChildIt);
            }
            return NULL;
        }

        Whole QuadRenderable::GetNumChildren() const
        {
            return this->ChildWidgets.size();
        }

        void QuadRenderable::RemoveChild(Widget* Child)
        {
            for( ChildIterator It = this->ChildWidgets.begin() ; It != this->ChildWidgets.end() ; ++It )
            {
                if( Child == (*It) ) {
                    Child->_NotifyParenthood(NULL);
                    this->ChildWidgets.erase(It);
                    this->_MarkDirty();
                    return;
                }
            }
        }

        void QuadRenderable::RemoveAllChildren()
        {
            for( ChildIterator It = this->ChildWidgets.begin() ; It != this->ChildWidgets.end() ; ++It )
            {
                (*It)->_NotifyParenthood(NULL);
            }
            this->ChildWidgets.clear();
            this->_MarkDirty();
        }

        void QuadRenderable::DestroyChild(Widget* ToBeDestroyed)
        {
            for( ChildIterator It = this->ChildWidgets.begin() ; It != this->ChildWidgets.end() ; ++It )
            {
                if( ToBeDestroyed == (*It) ) {
                    ToBeDestroyed->_NotifyParenthood(NULL);
                    this->ChildWidgets.erase(It);
                    this->ParentScreen->DestroyWidget(ToBeDestroyed);
                    this->_MarkDirty();
                    return;
                }
            }
        }

        void QuadRenderable::DestroyAllChildren()
        {
            for( ChildIterator It = this->ChildWidgets.begin() ; It != this->ChildWidgets.end() ; ++It )
            {
                (*It)->_NotifyParenthood(NULL);
                this->ParentScreen->DestroyWidget( (*It) );
            }
            this->ChildWidgets.clear();
            this->_MarkDirty();
        }

        QuadRenderable::ChildIterator QuadRenderable::ChildrenBegin()
            { return this->ChildWidgets.begin(); }

        QuadRenderable::ChildIterator QuadRenderable::ChildrenEnd()
            { return this->ChildWidgets.end(); }

        QuadRenderable::ConstChildIterator QuadRenderable::ChildrenBegin() const
            { return this->ChildWidgets.begin(); }

        QuadRenderable::ConstChildIterator QuadRenderable::ChildrenEnd() const
            { return this->ChildWidgets.end(); }

        QuadRenderable::ReverseChildIterator QuadRenderable::RChildrenBegin()
            { return this->ChildWidgets.rbegin(); }

        QuadRenderable::ReverseChildIterator QuadRenderable::RChildrenEnd()
            { return this->ChildWidgets.rend(); }

        QuadRenderable::ConstReverseChildIterator QuadRenderable::RChildrenBegin() const
            { return this->ChildWidgets.rbegin(); }

        QuadRenderable::ConstReverseChildIterator QuadRenderable::RChildrenEnd() const
            { return this->ChildWidgets.rend(); }

        ///////////////////////////////////////////////////////////////////////////////
        // Transform Methods

        void QuadRenderable::SetUnifiedPosition(const UnifiedVec2& Position)
        {
            if( this->PositioningPolicy.UPosition != Position ) {
                this->PositioningPolicy.UPosition = Position;
            }
        }

        void QuadRenderable::SetUnifiedSize(const UnifiedVec2& Size)
        {
            if( this->SizingPolicy.USize != Size ) {
                this->SizingPolicy.USize = Size;
            }
        }

        UnifiedVec2 QuadRenderable::GetUnifiedPosition() const
            { return this->PositioningPolicy.UPosition; }

        UnifiedVec2 QuadRenderable::GetUnifiedSize() const
            { return this->SizingPolicy.USize; }

        UnifiedRect QuadRenderable::GetUnifiedRect() const
            { return UnifiedRect(this->PositioningPolicy.UPosition,this->SizingPolicy.USize); }

        Vector2 QuadRenderable::GetActualPosition() const
            { return this->ActDims.Position; }

        Vector2 QuadRenderable::GetActualSize() const
            { return this->ActDims.Size; }

        Rect QuadRenderable::GetRect() const
            { return this->ActDims; }

        ///////////////////////////////////////////////////////////////////////////////
        // Fetch Methods

        QuadRenderable* QuadRenderable::GetParent() const
            { return this->ParentQuad; }

        QuadRenderable* QuadRenderable::GetNextSibling(Boole Wrap)
        {
            if( this->ParentQuad ) {
                ConstChildIterator ParentEnd = this->ParentQuad->ChildrenEnd();
                ConstChildIterator ParentBegin = this->ParentQuad->ChildrenBegin();
                for( ConstChildIterator ChildIt = ParentBegin ; ChildIt != ParentEnd ; ++ChildIt )
                {
                    if( this == (*ChildIt) ) {
                        if( ParentEnd != (++ChildIt) ) return (*ChildIt);
                        else return ( Wrap ? (*ParentBegin) : this );
                    }
                }
            }
            return NULL;
        }

        QuadRenderable* QuadRenderable::GetPrevSibling(Boole Wrap)
        {
            if( this->ParentQuad ) {
                ConstChildIterator ParentEnd = this->ParentQuad->ChildrenEnd();
                ConstChildIterator ParentBegin = this->ParentQuad->ChildrenBegin();
                for( ConstChildIterator ChildIt = ParentBegin ; ChildIt != ParentEnd ; ++ChildIt )
                {
                    if( this == (*ChildIt) ) {
                        if( ParentBegin != (--ChildIt) ) return (*ChildIt);
                        else return ( Wrap ? (*ParentEnd) : this );
                    }
                }
            }
            return NULL;
        }

        QuadRenderable* QuadRenderable::GetTopMostQuad()
        {
            if( this->IsChildOfScreen() ) {
                return this;
            }else{
                return (this->ParentQuad ? this->ParentQuad->GetTopMostQuad() : this );
            }
        }

        Widget* QuadRenderable::GetClosestChild(Widget* Child)
        {
            if( Child != NULL ) {
                QuadRenderable* RetTest = Child;
                while( RetTest->GetParent() != NULL && RetTest->GetParent() != this )
                    { RetTest = RetTest->GetParent(); }
                // By this point the return value will either be NULL or a widget
                // since it'll hit the ceiling (screen) or find this.
                // So a blind cast should be safe.
                return static_cast<Widget*>( RetTest );
            }
            return NULL;
        }

        ///////////////////////////////////////////////////////////////////////////////
        // VertexCaching Methods

        void QuadRenderable::SetLocalVertexCaching(Boole Enable)
        {
            if(Enable && !VertexCache) {
                this->VertexCache = new ScreenRenderData();
            }else if(!Enable && VertexCache) {
                this->VertexCache->Clear();
                delete this->VertexCache;
                this->VertexCache = NULL;
            }
        }

        Boole QuadRenderable::IsVertexCachingEnabled() const
        {
            return this->VertexCache != NULL;
        }

        ///////////////////////////////////////////////////////////////////////////////
        // Serialization

        void QuadRenderable::ProtoSerialize(XML::Node& ParentNode) const
        {
            XML::Node SelfRoot = ParentNode.AppendChild(this->GetDerivedSerializableName());

            this->ProtoSerializeImpl(SelfRoot);

            // Child quads always get serialized last
            this->ProtoSerializeChildQuads(SelfRoot);
        }

        void QuadRenderable::ProtoSerializeProperties(XML::Node& SelfRoot) const
        {
            this->Renderable::ProtoSerializeProperties(SelfRoot);
            XML::Node PropertiesNode = SelfRoot.AppendChild( QuadRenderable::GetSerializableName() + "Properties" );

            if( PropertiesNode.AppendAttribute("Version").SetValue("1") &&
                PropertiesNode.AppendAttribute("MousePassthrough").SetValue( this->MousePassthrough ? "true" : "false" ) &&
                PropertiesNode.AppendAttribute("ManualTransformUpdates").SetValue( this->ManualTransformUpdates ? "true" : "false" ) &&
                PropertiesNode.AppendAttribute("ZOrder").SetValue(this->ZOrder) &&
                PropertiesNode.AppendAttribute("VertexCache").SetValue( this->IsVertexCachingEnabled() ? "true" : "false" ) )
            {
                if( this->ActiveGroup != NULL ) {
                    PropertiesNode.AppendAttribute("ActiveGroup").SetValue( ActiveGroup->GetGroupID() );
                }

                XML::Node ActDimsNode = PropertiesNode.AppendChild("Dimensions");
                this->ActDims.ProtoSerialize( ActDimsNode );
                XML::Node PositioningPolicyNode = PropertiesNode.AppendChild("PositioningPolicy");
                this->PositioningPolicy.ProtoSerialize( PositioningPolicyNode );
                XML::Node SizingPolicyNode = PropertiesNode.AppendChild("SizingPolicy");
                this->SizingPolicy.ProtoSerialize( SizingPolicyNode );

                return;
            }else{
                SerializeError("Create XML Attribute Values",QuadRenderable::GetSerializableName() + "Properties",true);
            }
        }

        void QuadRenderable::ProtoSerializeRenderLayers(XML::Node& SelfRoot) const
        {
            XML::Node LayersNode = SelfRoot.AppendChild( "RenderLayers" );
            if( LayersNode.AppendAttribute("Version").SetValue("1") == false ) {
                SerializeError("Create XML Version Attribute","RenderLayers",true);
            }

            for( ConstRenderLayerIterator LayerIt = this->RenderLayers.begin() ; LayerIt != this->RenderLayers.end() ; ++LayerIt )
            {
                (*LayerIt)->ProtoSerialize(LayersNode);
            }
        }

        void QuadRenderable::ProtoSerializeRenderLayerGroups(XML::Node& SelfRoot) const
        {
            XML::Node GroupsNode = SelfRoot.AppendChild( "RenderLayerGroups" );
            if( GroupsNode.AppendAttribute("Version").SetValue("1") == false ) {
                SerializeError("Create XML Version Attribute","RenderLayerGroups",true);
            }

            for( ConstRenderLayerGroupIterator GroupIt = this->RenderLayerGroups.begin() ; GroupIt != this->RenderLayerGroups.end() ; ++GroupIt )
            {
                XML::Node CurrGroupNode = GroupsNode.AppendChild( "RenderLayerGroup" );
                if( CurrGroupNode.AppendAttribute( "GroupID" ).SetValue( (*GroupIt)->GetGroupID() ) ) {
                    for( RenderLayerGroup::RenderLayerIterator LayerIt = (*GroupIt)->RenderLayerBegin() ; LayerIt != (*GroupIt)->RenderLayerEnd() ; ++LayerIt )
                    {
                        XML::Node CurrLayerNode = CurrGroupNode.AppendChild( "RenderLayer" );

                        if( CurrLayerNode.AppendAttribute( "Index" ).SetValue( (*LayerIt).second->GetIndex() ) == false ) {
                            SerializeError("Create XML Attribute Values","Index",true);
                        }
                        if( CurrLayerNode.AppendAttribute( "ZOrder" ).SetValue( (*LayerIt).first ) == false ) {
                            SerializeError("Create XML Attribute Values","ZOrder",true);
                        }
                    }
                }else{
                    SerializeError("Create XML Attribute Values","GroupID",true);
                }
            }
        }

        void QuadRenderable::ProtoSerializeChildQuads(XML::Node& SelfRoot) const
        {
            XML::Node ChildrenNode = SelfRoot.AppendChild( "Children" );
            if( ChildrenNode.AppendAttribute("Version").SetValue("1") ) {
                for( ConstChildIterator ChildIt = this->ChildWidgets.begin() ; ChildIt != this->ChildWidgets.end() ; ++ChildIt )
                {
                    (*ChildIt)->ProtoSerialize(ChildrenNode);
                }
            }else{
                SerializeError("Create XML Version Attribute","Children",true);
            }
        }

        void QuadRenderable::ProtoDeSerialize(const XML::Node& SelfRoot)
        {
            this->ProtoDeSerializeImpl(SelfRoot);

            // Child quads update is always last
            this->ProtoDeSerializeChildQuads(SelfRoot);
        }

        void QuadRenderable::ProtoDeSerializeProperties(const XML::Node& SelfRoot)
        {
            this->Renderable::ProtoDeSerializeProperties(SelfRoot);

            XML::Attribute CurrAttrib;
            XML::Node PropertiesNode = SelfRoot.GetChild( QuadRenderable::GetSerializableName() + "Properties" );

            if( !PropertiesNode.Empty() ) {
                if(PropertiesNode.GetAttribute("Version").AsInt() == 1) {
                    // Get the single data type properties
                    CurrAttrib = PropertiesNode.GetAttribute("MousePassthrough");
                    if( !CurrAttrib.Empty() )
                        this->MousePassthrough = StringTools::ConvertToBool( CurrAttrib.AsString() );

                    CurrAttrib = PropertiesNode.GetAttribute("ManualTransformUpdates");
                    if( !CurrAttrib.Empty() )
                        this->ManualTransformUpdates = StringTools::ConvertToBool( CurrAttrib.AsString() );

                    CurrAttrib = PropertiesNode.GetAttribute("ZOrder");
                    if( !CurrAttrib.Empty() )
                        this->ZOrder = CurrAttrib.AsWhole();

                    CurrAttrib = PropertiesNode.GetAttribute("VertexCache");
                    if( !CurrAttrib.Empty() )
                        this->SetLocalVertexCaching( StringTools::ConvertToBool( CurrAttrib.AsString() ) );

                    CurrAttrib = PropertiesNode.GetAttribute("ActiveGroupName");
                    if( !CurrAttrib.Empty() )
                        this->SetActiveGroup( CurrAttrib.AsUint() );

                    // Get the properties that need their own nodes
                    XML::Node DimsNode = PropertiesNode.GetChild("Dimensions").GetFirstChild();
                    if( !DimsNode.Empty() )
                        this->ActDims.ProtoDeSerialize(DimsNode);

                    XML::Node PositioningNode = PropertiesNode.GetChild("PositioningPolicy").GetFirstChild();
                    if( !PositioningNode.Empty() )
                        this->PositioningPolicy.ProtoDeSerialize(PositioningNode);

                    XML::Node SizingNode = PropertiesNode.GetChild("SizingPolicy").GetFirstChild();
                    if( !SizingNode.Empty() )
                        this->SizingPolicy.ProtoDeSerialize(SizingNode);
                }else{
                    MEZZ_EXCEPTION(ExceptionBase::INVALID_VERSION_EXCEPTION,"Incompatible XML Version for " + (QuadRenderable::GetSerializableName() + "Properties") + ": Not Version 1.");
                }
            }else{
                MEZZ_EXCEPTION(ExceptionBase::II_IDENTITY_NOT_FOUND_EXCEPTION,QuadRenderable::GetSerializableName() + "Properties" + " was not found in the provided XML node, which was expected.");
            }
        }

        void QuadRenderable::ProtoDeSerializeRenderLayers(const XML::Node& SelfRoot)
        {
            this->DestroyAllRenderLayerGroups();
            this->DestroyAllRenderLayers();
            XML::Attribute CurrAttrib;
            XML::Node LayersNode = SelfRoot.GetChild( "RenderLayers" );

            if( !LayersNode.Empty() ) {
                if(LayersNode.GetAttribute("Version").AsInt() == 1) {
                    for( XML::NodeIterator LayerNodeIt = LayersNode.begin() ; LayerNodeIt != LayersNode.end() ; ++LayerNodeIt )
                    {
                        RenderLayer* CurrLayer = NULL;

                        if( (*LayerNodeIt).Name() == String("SingleImageLayer") ) {
                            CurrLayer = new SingleImageLayer(this);
                            CurrLayer->ProtoDeSerialize( (*LayerNodeIt) );
                            this->ResizeLayers( CurrLayer->GetIndex() );
                            this->RenderLayers[ CurrLayer->GetIndex() ] = CurrLayer;
                        }else if( (*LayerNodeIt).Name() == String("MultiImageLayer") ) {
                            CurrLayer = new MultiImageLayer(this);
                            CurrLayer->ProtoDeSerialize( (*LayerNodeIt) );
                            this->ResizeLayers( CurrLayer->GetIndex() );
                            this->RenderLayers[ CurrLayer->GetIndex() ] = CurrLayer;
                        }else if( (*LayerNodeIt).Name() == String("SingleLineTextLayer") ) {
                            CurrLayer = new SingleLineTextLayer(this);
                            CurrLayer->ProtoDeSerialize( (*LayerNodeIt) );
                            this->ResizeLayers( CurrLayer->GetIndex() );
                            this->RenderLayers[ CurrLayer->GetIndex() ] = CurrLayer;
                        }else if( (*LayerNodeIt).Name() == String("MultiLineTextLayer") ) {
                            CurrLayer = new MultiLineTextLayer(this);
                            CurrLayer->ProtoDeSerialize( (*LayerNodeIt) );
                            this->ResizeLayers( CurrLayer->GetIndex() );
                            this->RenderLayers[ CurrLayer->GetIndex() ] = CurrLayer;
                        }else{
                            MEZZ_EXCEPTION(ExceptionBase::II_IDENTITY_INVALID_EXCEPTION,"Unknown render layer name provided when deserializing.");
                        }
                    }
                }else{
                    MEZZ_EXCEPTION(ExceptionBase::INVALID_VERSION_EXCEPTION,"Incompatible XML Version for RenderLayers: Not Version 1.");
                }
            }
        }

        void QuadRenderable::ProtoDeSerializeRenderLayerGroups(const XML::Node& SelfRoot)
        {
            this->DestroyAllRenderLayerGroups();
            XML::Attribute CurrAttrib;
            XML::Node GroupsNode = SelfRoot.GetChild( "RenderLayerGroups" );

            if( !GroupsNode.Empty() ) {
                if(GroupsNode.GetAttribute("Version").AsInt() == 1) {
                    for( XML::NodeIterator GroupNodeIt = GroupsNode.begin() ; GroupNodeIt != GroupsNode.end() ; ++GroupNodeIt )
                    {
                        RenderLayerGroup* CurrGroup = NULL;

                        CurrAttrib = (*GroupNodeIt).GetAttribute("GroupID");
                        if( !CurrAttrib.Empty() )
                            CurrGroup = this->CreateRenderLayerGroup( CurrAttrib.AsUint() );

                        if( CurrGroup ) {
                            for( XML::NodeIterator LayerNodeIt = (*GroupNodeIt).begin() ; LayerNodeIt != (*GroupNodeIt).end() ; ++LayerNodeIt )
                            {
                                XML::Attribute IndexAttrib = (*LayerNodeIt).GetAttribute("Index");
                                XML::Attribute ZOrderAttrib = (*LayerNodeIt).GetAttribute("ZOrder");

                                if( IndexAttrib && ZOrderAttrib ) {
                                    CurrGroup->AddLayer( this->GetRenderLayer( IndexAttrib.AsWhole() ), ZOrderAttrib.AsWhole() );
                                }else{
                                    MEZZ_EXCEPTION(ExceptionBase::II_IDENTITY_NOT_FOUND_EXCEPTION,"Index and/or ZOrder attributes were not found in the provided XML node, which was expected.");
                                }
                            }
                        }
                    }
                }else{
                    MEZZ_EXCEPTION(ExceptionBase::INVALID_VERSION_EXCEPTION,"Incompatible XML Version for RenderLayerGroups: Not Version 1.");
                }
            }
        }

        void QuadRenderable::ProtoDeSerializeChildQuads(const XML::Node& SelfRoot)
        {
            XML::Node ChildrenNode = SelfRoot.GetChild( "Children" );
            if( !ChildrenNode.Empty() ) {
                if( ChildrenNode.GetAttribute("Version").AsInt() == 1 ) {
                    for( XML::NodeIterator ChildNodeIt = ChildrenNode.begin() ; ChildNodeIt != ChildrenNode.end() ; ++ChildNodeIt )
                    {
                        this->AddChild( this->ParentScreen->CreateWidget( (*ChildNodeIt) ) );
                    }
                }else{
                    MEZZ_EXCEPTION(ExceptionBase::INVALID_VERSION_EXCEPTION,"Incompatible XML Version for Children: Not Version 1.");
                }
            }
        }

        String QuadRenderable::GetDerivedSerializableName() const
            { return QuadRenderable::GetSerializableName(); }

        String QuadRenderable::GetSerializableName()
            { return "QuadRenderable"; }

        ///////////////////////////////////////////////////////////////////////////////
        // Internal Methods

        void QuadRenderable::_SetZOrder(const UInt16& Zorder)
            { this->ZOrder = Zorder; }

        void QuadRenderable::_NotifyParenthood(QuadRenderable* NewParent)
            { this->ParentQuad = NewParent; }

        void QuadRenderable::_SetLayoutStrat(LayoutStrategy* ToSet)
        {
            if( this->LayoutStrat != NULL ) {
                delete this->LayoutStrat;
            }
            this->LayoutStrat = ToSet;
        }

        void QuadRenderable::_Clean()
        {
            if( this->Dirty || this->AllLayersDirty ) {
                for( RenderLayerIterator LayerIt = this->RenderLayers.begin() ; LayerIt != this->RenderLayers.end() ; ++LayerIt )
                    { (*LayerIt)->_Redraw(this->AllLayersDirty); }

                this->Dirty = this->AllLayersDirty = false;
            }
        }

        void QuadRenderable::_MarkDirty()
        {
            if( this->Dirty )
                return;

            this->Dirty = true;
            if( this->ParentQuad )
                this->ParentQuad->_MarkDirty();
        }

        void QuadRenderable::_MarkAllChildrenDirty()
        {
            this->_MarkDirty();
            for( ChildIterator It = this->ChildWidgets.begin() ; It != this->ChildWidgets.end() ; ++It )
                { (*It)->_MarkAllChildrenDirty(); }
        }

        void QuadRenderable::_MarkAllLayersDirty()
        {
            if( this->Dirty && this->AllLayersDirty )
                return;

            this->Dirty = true;
            this->AllLayersDirty = true;
            this->ParentQuad->_MarkDirty();
        }

        void QuadRenderable::_AppendRenderData(ScreenRenderData& RenderData)
        {
            this->_Clean();
            if( this->GetVisible() ) {
                this->AppendLayerVertices(RenderData.Vertices);
            }
        }

        void QuadRenderable::_AppendRenderDataCascading(ScreenRenderData& RenderData)
        {
            if( this->VertexCache ) {
                if( this->Dirty || this->AllLayersDirty ) {
                    this->VertexCache->Clear();
                    this->_AppendRenderData(*VertexCache);
                    for( ChildIterator ChildIt = this->ChildWidgets.begin() ; ChildIt != this->ChildWidgets.end() ; ++ChildIt )
                        { (*ChildIt)->_AppendRenderDataCascading(*VertexCache); }
                }
                RenderData.Append(*VertexCache);
            }else{
                this->_AppendRenderData(RenderData);
                for( ChildIterator ChildIt = this->ChildWidgets.begin() ; ChildIt != this->ChildWidgets.end() ; ++ChildIt )
                    { (*ChildIt)->_AppendRenderDataCascading(RenderData); }
            }
        }
    }//UI
}//Mezzanine

#endif
