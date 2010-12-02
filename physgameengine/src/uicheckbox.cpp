//� Copyright 2010 BlackTopp Studios Inc.
/* This file is part of The PhysGame Engine.

    The PhysGame Engine is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    The PhysGame Engine is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with The PhysGame Engine.  If not, see <http://www.gnu.org/licenses/>.
*/
/* The original authors have included a copy of the license specified above in the
   'Docs' folder. See 'gpl.txt'
*/
/* We welcome the use of The PhysGame anyone, including companies who wish to
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
#ifndef _uicheckbox_cpp
#define _uicheckbox_cpp

#include "uicheckbox.h"
#include "uimanager.h"
#include "uilayer.h"
#include "uimarkuptext.h"
#include "uibutton.h"
#include "inputquerytool.h"
#include "metacode.h"
#include "world.h"

namespace phys
{
    namespace UI
    {
        CheckBox::CheckBox(String& name, Vector2 Position, Vector2 Size, Whole Glyph, String &LabelText, UILayer* Layer)
            : Widget(name,Layer),
              GlyphIndex(Glyph),
              Checked(false),
              CheckLock(true)
        {
            Manager = World::GetWorldPointer()->GetUIManager();
            RelPosition = Position;
            RelSize = Size;

            Box = Parent->CreateButton(Name+"CB",Position,Size);
            if(LabelText.empty())
            {
                Label = NULL;
            }else{
                Position.X+=Size.X;
                Label = Parent->CreateMarkupText(Name+"CM",Position,GlyphIndex,LabelText);
            }
        }

        CheckBox::~CheckBox()
        {
            delete Box;
            if(Label)
                delete Label;
        }

        void CheckBox::SetSpriteSet(std::pair<String,String>& SpriteSet)
        {
            Box->SetBackgroundSprite(SpriteSet.first);
            if(!SpriteSet.second.empty())
                Box->SetHoveredSprite(SpriteSet.second);
        }

        void CheckBox::Update(bool Force)
        {
            if(Box->GetMouseHover() || Force)
            {
                MetaCode::ButtonState State = Manager->GetInputQueryer()->GetMouseButtonState(1);
                if(MetaCode::BUTTON_PRESSING == State)
                {
                    CheckLock = false;
                }
                else if(MetaCode::BUTTON_LIFTING && !CheckLock)
                {
                    Checked = !Checked;
                    if(Checked)
                    {
                        SetSpriteSet(CheckedSet);
                    }else{
                        SetSpriteSet(UncheckedSet);
                    }
                    CheckLock = true;
                }
            }
        }

        bool CheckBox::IsChecked()
        {
            return Checked;
        }

        bool CheckBox::CheckMouseHover()
        {
            if(Box->CheckMouseHover())
            {
                Update();
                return true;
            }
            return false;
        }

        void CheckBox::SetLabelText(String &LabelText)
        {
            if(!Label)
            {
                Vector2 Position = Box->GetPosition();
                Position.X+=Box->GetSize().X;
                Label = Parent->CreateMarkupText(Name+"CM",Position,GlyphIndex,LabelText);
            }else{
                Label->SetText(LabelText);
            }
        }

        String CheckBox::GetLabelText()
        {
            if(!Label)
            {
                String Text = "";
                return Text;
            }else{
                return Label->GetText();
            }
        }

        void CheckBox::SetUncheckedSprite(String& Unchecked, String& Hovered)
        {
            //UncheckedSet.first.assign(Unchecked);
            //UncheckedSet.second = Hovered;
        }

        void CheckBox::SetCheckedSprite(String& Checked, String& Hovered)
        {
            //CheckedSet.first = Checked;
            //CheckedSet.second = Hovered;
        }

        void CheckBox::SetPosition(Vector2 Position)
        {
            RelPosition = Position;
            Box->SetPosition(Position);
            if(Label)
            {
                Position.X+=Box->GetSize().X;
                Label->SetPosition(Position);
            }
        }

        Vector2 CheckBox::GetPosition()
        {
            return RelPosition;
        }

        void CheckBox::SetActualPosition(Vector2 Position)
        {
            RelPosition = Position / Manager->GetWindowDimensions();
            Box->SetActualPosition(Position);
            if(Label)
            {
                Position.X+=Box->GetSize().X;
                Label->SetActualPosition(Position);
            }
        }

        Vector2 CheckBox::GetActualPosition()
        {
            return RelPosition * Manager->GetWindowDimensions();
        }

        void CheckBox::SetSize(Vector2 Size)
        {
            RelSize = Size;
            Box->SetSize(Size);
            if(Label)
            {
                Size.X+=Box->GetPosition().X;
                Label->SetPosition(Size);
            }
        }

        Vector2 CheckBox::GetSize()
        {
            return RelSize;
        }

        void CheckBox::SetActualSize(Vector2 Size)
        {
            RelSize = Size / Manager->GetWindowDimensions();
            Box->SetActualSize(Size);
            if(Label)
            {
                Size.X+=Box->GetActualPosition().X;
                Label->SetActualPosition(Size);
            }
        }

        Vector2 CheckBox::GetActualSize()
        {
            return RelSize * Manager->GetWindowDimensions();
        }

        Button* CheckBox::GetCheckBoxButton()
        {
            return Box;
        }

        MarkupText* CheckBox::GetCheckBoxLabel()
        {
            return Label;
        }
    }//UT
}//phys

#endif
