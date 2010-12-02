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
#ifndef _uibutton_h
#define _uibutton_h

#include "datatypes.h"
#include "enumerations.h"
#include "colourvalue.h"
#include "vector2.h"

namespace Gorilla
{
    class Caption;
    class Layer;
    class Rectangle;
    class Sprite;
}

namespace phys
{
    class UIManager;
    class UILayer;
    namespace UI
    {
        ///////////////////////////////////////////////////////////////////////////////
        /// @class Button
        /// @headerfile uibutton.h
        /// @brief This class is a helper class, specifically for use as a button.
        /// @details Unlike rectangles and captions, this class can be interacted with by clicking.
        /// It is important to understand what you want your space to do when selecting the class to use.
        ///////////////////////////////////////
        class Button
        {
            protected:
                Gorilla::Rectangle* GorillaRectangle;
                UILayer* Parent;
                UIManager* Manager;
                Gorilla::Sprite* NormalSprite;
                Gorilla::Sprite* HoveredSprite;
                Gorilla::Sprite* UserSprite;
                bool MouseHover;
                Vector2 RelPosition;
                Vector2 RelSize;
                String Name;
            public:
                /// @brief Internal constructor
                /// @param name The name of the button.
                /// @param Position The top left position of the button.
                /// @param Size The size of the Button.
                /// @param Layer Pointer to the Layer that created this button.
                Button(String& name, Vector2 Position, Vector2 Size, UILayer* Layer);
                /// @brief Class destructor.
                ~Button();
                /// @brief Gets the name of this button.
                /// @return Returns a string containing the name of this button.
                virtual String& GetName();
                /// @brief Determines whether the mouse is over this button.
                /// @return Returns a bool indicating whether the mouse is over this button.
                virtual bool CheckMouseHover();
                /// @brief Gets the stored value of whether or not the mouse is over the button.
                /// @details This function does not perform any checks.  If you want to do a manual check, call CheckMouseHover().
                /// @return Returns the stored value of whether or not the mouse is over the button.
                virtual bool GetMouseHover();
                /// @brief Sets the background colour of the button.
                /// @param Colour A colour value representing the colour to be set.
                virtual void SetBackgroundColour(ColourValue& Colour);
                /// @brief Sets the background image(if provided in the atlas) of the button.
                /// @param Name The name of the sprite to set as the background.
                virtual void SetBackgroundSprite(const String& Name);
                /// @brief Sets an alternate background image that will be applied when the mouse is over this button.
                /// @param Name The name of the sprite to set as the alternate background.
                virtual void SetHoveredSprite(const String& Name);
                /// @brief Sets an alternate background image that is stored and can be quickly swapped with the active sprite.
                /// @param Name The name of the sprite to set as the alternate background.
                virtual void SetUserSprite(const String& Name);
                /// @brief Enables(or disables) the currently set User sprite.
                /// @param Enable If true, this will swap the current sprite with the user sprite, if false
                /// it will swap the User sprite for the normal sprite.
                virtual void EnableUserSprite(bool Enable);
                /// @brief Enables a border and sets it's colour.
                /// @param Colour A colour value representing the colour to be set.
                virtual void SetBorder(Real Width, ColourValue& Colour);
                /// @brief Sets the relative top left position of this button.
                /// @param Position A Vector2 representing the location of this button.
                virtual void SetPosition(Vector2 Position);
                /// @brief Gets the relative top left position of this button.
                /// @return Returns a Vector2 representing the location of this button.
                virtual Vector2 GetPosition();
                /// @brief Sets the top left position of this button in pixels.
                /// @param Position A Vector2 representing the location of this button.
                virtual void SetActualPosition(Vector2 Position);
                /// @brief Gets the top left position of this button in pixels.
                /// @return Returns a Vector2 representing the location of this button.
                virtual Vector2 GetActualPosition();
                /// @brief Sets the relative size of this button.
                /// @param Size A vector2 representing the size of this button.
                virtual void SetSize(Vector2 Size);
                /// @brief Gets the relative size of this button.
                /// @return Returns a vector2 representing the size of this button.
                virtual Vector2 GetSize();
                /// @brief Sets the size of this button in pixels.
                /// @param Size A vector2 representing the size of this button.
                virtual void SetActualSize(Vector2 Size);
                /// @brief Gets the size of this button in pixels.
                /// @return Returns a vector2 representing the size of this button.
                virtual Vector2 GetActualSize();
                /// @brief Sets the priority this button should be rendered with.
                /// @details The default value for this is Medium.
                /// @param Priority The priority level to be used when rendering this button.
                virtual void SetRenderPriority(UI::RenderPriority Priority);
                /// @brief Gets the priority this button should be rendered with.
                /// @return Returns an enum value representing this button's priority level.
                virtual UI::RenderPriority GetRenderPriority();
        };//button
    }//UI
}//phys

#endif
