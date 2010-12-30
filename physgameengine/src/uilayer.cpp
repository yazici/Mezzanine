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
#ifndef _uilayer_cpp
#define _uilayer_cpp

#include "uilayer.h"
#include "uitextbutton.h"
#include "uirectangle.h"
#include "uicaption.h"
#include "uiwidget.h"
#include "uicheckbox.h"
#include "uibuttonlistbox.h"
#include "uimarkuptext.h"
#include "uilistbox.h"
#include "uiscrollbar.h"
#include "uilinelist.h"
#include "uiwindow.h"

#include "graphicsmanager.h"
#include "world.h"

#include "internalGorilla.h.cpp"

namespace phys
{
    UILayer::UILayer(const String& name, Gorilla::Layer* GLayer, Gorilla::Screen* GScreen)
        : Name(name),
          GorillaLayer(GLayer),
          Parent(GScreen)
    {
        Manager = World::GetWorldPointer()->GetUIManager();
    }

    UILayer::~UILayer()
    {
        while(!Buttons.empty())
        {
            UI::Button* button = Buttons.back();
            delete button;
            Buttons.pop_back();
        }
        while(!Rectangles.empty())
        {
            UI::Rectangle* rectangle = Rectangles.back();
            delete rectangle;
            Rectangles.pop_back();
        }
        while(!Captions.empty())
        {
            UI::Caption* caption = Captions.back();
            delete caption;
            Captions.pop_back();
        }
        while(!MarkupTexts.empty())
        {
            UI::MarkupText* markup = MarkupTexts.back();
            delete markup;
            MarkupTexts.pop_back();
        }
        while(!LineLists.empty())
        {
            UI::LineList* linelist = LineLists.back();
            delete linelist;
            LineLists.pop_back();
        }
        while(!Widgets.empty())
        {
            UI::Widget* widget = Widgets.back();
            delete widget;
            Widgets.pop_back();
        }
        Parent->destroy(GorillaLayer);
    }

    String& UILayer::GetName()
    {
        return Name;
    }

    void UILayer::SetVisible(bool Visable)
    {
        GorillaLayer->setVisible(Visable);
    }

    bool UILayer::GetVisible()
    {
        return GorillaLayer->isVisible();
    }

    void UILayer::Show()
    {
        GorillaLayer->show();
    }

    void UILayer::Hide()
    {
        GorillaLayer->hide();
    }

    UI::Button* UILayer::CreateButton(ConstString& Name, Vector2 Position, Vector2 Size)
    {
        UI::Button* button = new UI::Button(Name, Position, Size, this);
        Buttons.push_back(button);
        return button;
    }

    UI::TextButton* UILayer::CreateTextButton(ConstString& Name, Vector2 Position, Vector2 Size, Whole Glyph, ConstString Text)
    {
        UI::TextButton* tbutton = new UI::TextButton(Name, Position, Size, Glyph, Text, this);
        Buttons.push_back(tbutton);
        return tbutton;
    }

    UI::Button* UILayer::GetButton(ConstString& Name)
    {
        for ( std::vector<UI::Button*>::iterator it = Buttons.begin() ; it != Buttons.end() ; it++ )
        {
            if ( Name == (*it)->GetName() )
            {
                UI::Button* button = (*it);
                return button;
            }
        }
        return 0;
    }

    UI::Button* UILayer::GetButton(Whole Index)
    {
        return Buttons[Index];
    }

    Whole UILayer::GetNumButtons()
    {
        return Buttons.size();
    }

    void UILayer::DestroyButton(UI::Button* ToBeDestroyed)
    {
        for ( std::vector<UI::Button*>::iterator it = Buttons.begin() ; it != Buttons.end() ; it++ )
        {
            if ( ToBeDestroyed == (*it) )
            {
                delete ToBeDestroyed;
                Buttons.erase(it);
                return;
            }
        }
    }

    UI::Rectangle* UILayer::CreateRectangle(Vector2 Position, Vector2 Size)
    {
        UI::Rectangle* rectangle = new UI::Rectangle(Position,Size, this);
        Rectangles.push_back(rectangle);
        return rectangle;
    }

    UI::Rectangle* UILayer::GetRectangle(Whole Index)
    {
        return Rectangles[Index];
    }

    Whole UILayer::GetNumRectangles()
    {
        return Rectangles.size();
    }

    void UILayer::DestroyRectangle(UI::Rectangle* ToBeDestroyed)
    {
        for ( std::vector<UI::Rectangle*>::iterator it = Rectangles.begin() ; it != Rectangles.end() ; it++ )
        {
            if ( ToBeDestroyed == (*it) )
            {
                delete ToBeDestroyed;
                Rectangles.erase(it);
                return;
            }
        }
    }

    UI::Caption* UILayer::CreateCaption(ConstString& Name, Vector2 Position, Vector2 Size, Whole Glyph, String Text)
    {
        UI::Caption* caption = new UI::Caption(Name,Position,Size,Glyph,Text,this);
        Captions.push_back(caption);
        return caption;
    }

    UI::Caption* UILayer::GetCaption(ConstString& Name)
    {
        for ( std::vector<UI::Caption*>::iterator it = Captions.begin() ; it != Captions.end() ; it++ )
        {
            if ( Name == (*it)->GetName() )
            {
                UI::Caption* caption = (*it);
                return caption;
            }
        }
        return 0;
    }

    UI::Caption* UILayer::GetCaption(Whole Index)
    {
        return Captions[Index];
    }

    Whole UILayer::GetNumCaptions()
    {
        return Captions.size();
    }

    void UILayer::DestroyCaption(UI::Caption* ToBeDestroyed)
    {
        for ( std::vector<UI::Caption*>::iterator it = Captions.begin() ; it != Captions.end() ; it++ )
        {
            if ( ToBeDestroyed == (*it) )
            {
                delete ToBeDestroyed;
                Captions.erase(it);
                return;
            }
        }
    }

    UI::MarkupText* UILayer::CreateMarkupText(ConstString& Name, Vector2 Position, Whole Glyph, String Text)
    {
        UI::MarkupText* markup = new UI::MarkupText(Name,Position,Glyph,Text,this);
        MarkupTexts.push_back(markup);
        return markup;
    }

    UI::MarkupText* UILayer::GetMarkupText(ConstString& Name)
    {
        for ( std::vector<UI::MarkupText*>::iterator it = MarkupTexts.begin() ; it != MarkupTexts.end() ; it++ )
        {
            if ( Name == (*it)->GetName() )
            {
                UI::MarkupText* markup = (*it);
                return markup;
            }
        }
        return 0;
    }

    UI::MarkupText* UILayer::GetMarkupText(Whole Index)
    {
        return MarkupTexts[Index];
    }

    Whole UILayer::GetNumMarkupTexts()
    {
        return MarkupTexts.size();
    }

    void UILayer::DestroyMarkupText(UI::MarkupText* ToBeDestroyed)
    {
        for ( std::vector<UI::MarkupText*>::iterator it = MarkupTexts.begin() ; it != MarkupTexts.end() ; it++ )
        {
            if ( ToBeDestroyed == (*it) )
            {
                delete ToBeDestroyed;
                MarkupTexts.erase(it);
                return;
            }
        }
    }

    UI::LineList* UILayer::CreateLineList()
    {
        UI::LineList* linelist = new UI::LineList(this);
        LineLists.push_back(linelist);
        return linelist;
    }

    UI::LineList* UILayer::GetLineList(Whole Index)
    {
        return LineLists[Index];
    }

    Whole UILayer::GetNumLineLists()
    {
        return LineLists.size();
    }

    void UILayer::DestroyLineList(UI::LineList* ToBeDestroyed)
    {
        for ( std::vector<UI::LineList*>::iterator it = LineLists.begin() ; it != LineLists.end() ; it++ )
        {
            if ( ToBeDestroyed == (*it) )
            {
                delete ToBeDestroyed;
                LineLists.erase(it);
                return;
            }
        }
    }

    UI::Widget* UILayer::GetWidget(ConstString& Name)
    {
        for ( std::vector<UI::Widget*>::iterator it = Widgets.begin() ; it != Widgets.end() ; it++ )
        {
            if ( Name == (*it)->GetName() )
            {
                UI::Widget* widget = (*it);
                return widget;
            }
        }
        return 0;
    }

    UI::Widget* UILayer::GetWidget(Whole Index)
    {
        return Widgets[Index];
    }

    Whole UILayer::GetNumWidgets()
    {
        return Widgets.size();
    }

    void UILayer::DestroyWidget(UI::Widget* ToBeDestroyed)
    {
        for ( std::vector<UI::Widget*>::iterator it = Widgets.begin() ; it != Widgets.end() ; it++ )
        {
            if ( ToBeDestroyed == (*it) )
            {
                delete ToBeDestroyed;
                /*UI::Widget::WidgetType Type = ToBeDestroyed->GetType();
                switch (Type)
                {
                    case UI::Widget::Scrollbar:
                    {
                        UI::Scrollbar* Scroll = static_cast<UI::Scrollbar*> (ToBeDestroyed);
                        delete Scroll;
                        return;
                    }
                    case UI::Widget::CheckBox:
                    {
                        UI::CheckBox* Check = static_cast<UI::CheckBox*> (ToBeDestroyed);
                        delete Check;
                        return;
                    }
                    case UI::Widget::ButtonListBox:
                    {
                        UI::ButtonListBox* ButtonList = static_cast<UI::ButtonListBox*> (ToBeDestroyed);
                        delete ButtonList;
                        return;
                    }
                    case UI::Widget::ListBox:
                    {
                        UI::ListBox* List = static_cast<UI::ListBox*> (ToBeDestroyed);
                        delete List;
                        return;
                    }
                    case UI::Widget::Window:
                    {
                        UI::Window* Win = static_cast<UI::Window*> (ToBeDestroyed);
                        delete Win;
                        return;
                    }
                    default:
                        return;
                }*/
                Widgets.erase(it);
            }
        }
    }

    UI::Scrollbar* UILayer::CreateScrollbar(ConstString& Name, Vector2 Position, Vector2 Size, UI::ScrollbarStyle Style)
    {
        UI::Scrollbar* Scroll = new UI::Scrollbar(Name,Position,Size,Style,this);
        Widgets.push_back(Scroll);
        return Scroll;
    }

    UI::CheckBox* UILayer::CreateCheckBox(ConstString& Name, Vector2 Position, Vector2 Size, Whole Glyph, String &LabelText)
    {
        UI::CheckBox* Check = new UI::CheckBox(Name,Position,Size,Glyph,LabelText,this);
        Widgets.push_back(Check);
        return Check;
    }

    UI::ButtonListBox* UILayer::CreateButtonListBox(ConstString& Name, Vector2 Position, Vector2 Size, Real ScrollbarWidth, UI::ScrollbarStyle ScrollStyle)
    {
        UI::ButtonListBox* BLB = new UI::ButtonListBox(Name,Position,Size,ScrollbarWidth,ScrollStyle,this);
        Widgets.push_back(BLB);
        return BLB;
    }

    UI::ListBox* UILayer::CreateListBox(ConstString& Name, Vector2 Position, Vector2 Size, Real ScrollbarWidth, UI::ScrollbarStyle ScrollStyle)
    {
        UI::ListBox* LB = new UI::ListBox(Name,Position,Size,ScrollbarWidth,ScrollStyle,this);
        Widgets.push_back(LB);
        return LB;
    }

    UI::Window* UILayer::CreateWidgetWindow(ConstString& Name, Vector2 Position, Vector2 Size)
    {
        UI::Window* Win = new UI::Window(Name,Position,Size,this);
        Widgets.push_back(Win);
        return Win;
    }

    UI::Button* UILayer::CheckButtonMouseIsOver()
    {
        if(Buttons.empty())
            return 0;
        if(!GetVisible())
            return 0;
        UI::Button* button = NULL;
        for( std::vector<UI::Button*>::iterator it = Buttons.begin() ; it != Buttons.end() ; it++ )
        {
            button = (*it);
            if(button->IsVisible())
            {
                if(button->CheckMouseHover())
                {
                    return button;
                }
            }
        }
        return 0;
    }

    UI::Widget* UILayer::CheckWidgetMouseIsOver()
    {
        if(Widgets.empty())
            return 0;
        UI::Widget* widget = NULL;
        for( std::vector<UI::Widget*>::iterator it = Widgets.begin() ; it != Widgets.end() ; it++ )
        {
            widget = (*it);
            if(widget->IsVisible())
            {
                if(widget->CheckMouseHover())
                {
                    return widget;
                }
            }
        }
        return 0;
    }

    Gorilla::Layer* UILayer::GetGorillaLayer()
    {
        return GorillaLayer;
    }
}//phys

#endif
