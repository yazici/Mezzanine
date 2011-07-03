//� Copyright 2010 - 2011 BlackTopp Studios Inc.
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
#ifndef _uimanager_h
#define _uimanager_h

#include "crossplatformexport.h"
#include "datatypes.h"
#include "managerbase.h"
#include "vector2.h"
#include "metacode.h"

#include <map>

namespace Gorilla
{
    class Silverback;
    class Layer;
    class Screen;
}

namespace phys
{
    class Viewport;
    class InputQueryTool;
    namespace UI
    {
        class Screen;
        class Layer;
        class Button;
        class Widget;
        class Scrollbar;
    }
    ///////////////////////////////////////////////////////////////////////////////
    /// @class UIManager
    /// @headerfile uimanager.h
    /// @brief This class is responsible for any and all user interactions with the User interface/HUD.
    /// @details Currently, you have to create the UI/HUD in code.  Font and sprite data is loaded through a premade Gorilla file(*.gorilla).
    ///////////////////////////////////////
    class PHYS_LIB UIManager : public ManagerBase
    {
        protected:
            /// @brief Pointer for the Gorilla core class, where this manager gets it's functionality.
            Gorilla::Silverback* Silver;
            std::vector< UI::Screen* > Screens;
            std::vector< UI::Button* > ActivatedButtons;
            std::vector< MetaCode::InputCode > AutoRegisterCodes;
            std::multimap< MetaCode::InputCode, UI::Button* > HotKeys;
            UI::Button* HoveredButton;
            UI::Button* HoveredWidgetButton;
            UI::Widget* HoveredWidget;
            UI::Widget* WidgetFocus;
            UI::Widget* InputCapture;
            UI::Widget* LastWidgetSelected;
            bool ButtonAutoRegister;
            void HoverChecks();
            void HotKeyAndInputCaptureChecks();
            void WidgetUpdates();
            void ClearButtonActivations();
            void ViewportUpdateChecks();
            void MouseActivationCheck(UI::Button* ToCheck);
            void HotKeyActivationCheck(const MetaCode::InputCode& Code);
        public:
            /// @brief Class Constructor.
            /// @details Standard class initialization constructor.
            UIManager();
            /// @brief Class Destructor.
            /// @details The class destructor.
            ~UIManager();
            /// @brief Inherited from ManagerBase.
            void Initialize();
            /// @brief Inherited from ManagerBase.
            void DoMainLoopItems();
            /// @brief Loads a Gorilla file for use with this manager.
            /// @param Name The name of the file to be loaded, not including the extension.
            void LoadGorilla(const String& Name);
            /// @brief Forces everything loaded into the UI system to be redrawn.
            /// @param Force If Force is set to true, it will redraw everything regardless of if it has changed.
            void RedrawAll(bool Force = false);
            /// @brief Binds a key to a button.
            /// @details This function allows buttons to behave like they are pressed without mouse input.
            /// @param HotKey The key or button (on the input device) to activate the button.
            /// @param BoundButton The button being bound to the hotkey.
            void BindHotKey(const MetaCode::InputCode& HotKey, UI::Button* BoundButton);
            /// @brief Removes a previously set hotkey binding.
            /// @param HotKey The key or button (on the input device) to activate the button.
            /// @param BoundButton The button currently bound to the hotkey.
            void UnbindHotKey(const MetaCode::InputCode& HotKey, UI::Button* BoundButton);
            /// @brief Clears all registered hotkeys.
            void RemoveAllHotKeys();
            /// @brief Enables whether or not to automatically set the activation key or button for UI buttons.
            /// @details If true, this will cause every UI button to have keyboard keys or mouse buttons added with
            /// AddButtonAutoRegister() to be added as activation keys or buttons immediately after they are created. @n
            /// Default: False.
            /// @param Enable Whether or not to enable this feature.
            void EnableButtonAutoRegister(bool Enable);
            /// @brief Gets whether or not the ButtonAutoRegister feature is enabled.
            /// @return Returns a bool indicating whether or not the ButtonAutoRegister feature is enabled.
            bool ButtonAutoRegisterEnabled();
            /// @brief Adds a key or button that will be auto-registered with every created UI button.
            /// @param Code The input code for the keyboard key or mouse button to be added to the list of codes to be auto-registered.
            void AddAutoRegisterCode(MetaCode::InputCode Code);
            /// @brief Removes a previously set auto-registering input code.
            /// @param Code The input code to be removed from the list of auto-registering codes.
            void RemoveAutoRegisterCode(MetaCode::InputCode Code);
            /// @brief Removes all auto-registering input codes.
            void RemoveAllAutoRegisterCodes();
            /// @brief Gets the list of codes that will be auto-registered with each UI button.
            /// @return Returns a pointer to the vector containing all the codes to be auto-registered with every UI button.
            std::vector<MetaCode::InputCode>* GetAutoRegisteredCodes();
            /// @brief Gets the button the mouse is hovering over.
            /// @details This function will report only standalone buttons, not widget buttons.
            /// @return Returns a pointer to the button, or NULL if it's not over any visable buttons.
            UI::Button* GetHoveredButton();
            /// @brief Gets the button the mouse is hovering over, if the button is inside a widget.
            /// @details This function will report only widget buttons, not standalone buttons.
            /// @return Returns a pointer to the button, or NULL if it's not over any visable buttons.
            UI::Button* GetHoveredWidgetButton();
            /// @brief Gets the Widget the mouse is hovering over.
            /// @details If the widget found during widget checks belongs to a widget, this will get that widget.
            /// @return Returns a pointer to the widget, or NULL if it's not over any visable buttons.
            UI::Widget* GetHoveredWidget();
            /// @brief Gets the current widget being controlled.
            /// @details The widget control is used mostly for manipulating widgets while the mouse is not
            /// currently hovering over them, such as the click and drag action of scrollbars and resizing windows.
            /// @return Returns a pointer to the currently controlled widget, or NULL if none are being controlled this frame.
            UI::Widget* GetWidgetFocus();
            /// @brief Gets the widget that is currently capturing input.
            /// @details A widget that is currently capturing input will prevent hotkeys from working while active.
            /// @return Returns a pointer to the widget that currently capturing input, or NULL if no widget is capturing input.
            UI::Widget* GetWidgetCapturingInput();
            /// @brief Creates an internal HUD screen.
            /// @details Screens are the base set of renderable UI you can use, allowing you to switch entire sets of UI's
            /// on the fly if needed.  For performance reasons you should always keep the number of screens you create to a minimum.
            /// @param ScreenName The name to be given to the screen.
            /// @param Atlas The name of a previously loaded Gorilla file to be used with this screen.
            /// @param WindowViewport The viewport to create this screen in.
            UI::Screen* CreateScreen(const String& ScreenName, const String& Atlas, Viewport* WindowViewport);
            /// @brief Gets an already created screen by name.
            /// @return Returns a pointer to the screen of the specified name.
            UI::Screen* GetScreen(const String& Name);
            /// @brief Gets an already created screen by index.
            /// @return Returns a pointer to the screen at the specified index.
            UI::Screen* GetScreen(Whole Index);
            /// @brief Gets the number of screens created and stored in this manager.
            /// @return Returns the number of screens this manager is storing.
            Whole GetNumScreens();
            /// @brief Deletes a screen and removes all trace of it from the manager.
            /// @details Destroying a screen will also destroy all of it's layers, and everything contained in those layers.
            /// @param Screen The screen to be destroyed.
            void DestroyScreen(UI::Screen* Screen);
            /// @brief Deletes all screens stored in this manager.
            void DestroyAllScreens();
            /// @brief Searches all screens and gets the named Layer.
            /// @return Returns the named layer if found, NULL if not.
            UI::Layer* GetLayer(const String& Name);
            /// @brief Searches all visible screens and layers to see if a button was clicked.
            /// @details This is called automatically once every frame.  Should only be called on manually if
            /// you need more then one check per frame.
            /// @return Returns the button clicked if there is one, NULL if not.
            UI::Button* CheckButtonMouseIsOver();
            /// @brief Searches all visable screens and layers to see if a widget was clicked.
            /// @details This is called automatically once every frame.  Should only be called on manually if
            /// you need more then one check per frame.
            /// @return Returns the widget clicked if there is one, NULL if not.
            UI::Widget* CheckWidgetMouseIsOver();
            /// @brief Checks to see if the mouse is over a UI element.
            /// @details This should only be called on after this manager does it's main loop items for best results.
            /// @return Returns true if the mouse is over a visable UI element, false if not.
            bool MouseIsInUISystem();
            /// @brief Suggests a glyph index based on the desired actual height.
            /// @param Height The desired Height of the glyph wanted in pixels.
            /// @param Atlas The altas to search.
            /// @return Returns a std::pair, First is a whole for the Glyph index and second is a Real for the scaling that should be provided to it.
            std::pair<Whole,Real> SuggestGlyphIndex(const Whole& Height, const String& Atlas);
            /// @brief Gets the type of manager that this manager is.
            /// @return Returns an enum value representing the type of manager that this manager is.
            ManagerBase::ManagerTypeName GetType() const;
            /// @internal
            /// @brief Gets the internal silverback pointer.
            /// @return Returns a pointer to the internal silverback class.
            Gorilla::Silverback* GetSilverbackPointer();
    };//uimanager
}//phys

#endif
