//© Copyright 2010 Joseph Toppi and John Blackwood
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
#ifndef PHYSEVENTUSERINPUT_CPP
#define PHYSEVENTUSERINPUT_CPP
///////////////////////////////////////////////////////////////////////////////
// This will expose all keyboard and mouse, joystick and other userinput events
// to developers, we are using the SDL keymap to get us started, large portions
// are directly copy/pasta'd, so we included their license too
///////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// Includes
#include "physeventuserinput.h"
#include "physevent.h"
#include <stdarg.h>
#include <vector>

#include "SDL.h"

using namespace std;

///////////////////////////////////////////////////////////////////////////////
// MetaCode

MetaCode::MetaCode()
{}

MetaCode::MetaCode(int MetaValue_, short unsigned int ID_, MetaCode::InputCode Code_)
{
    Construct(MetaValue_, ID_, Code_);
}

MetaCode::MetaCode(RawEvent _RawEvent)
{
    /// @todo TODO: Actually process each event
    //Make a metacode for each event and return it
    switch(_RawEvent.type)
    {
        case SDL_KEYDOWN:
            break;
        case SDL_KEYUP:
            break;
        case SDL_MOUSEMOTION:
            break;
        case SDL_MOUSEBUTTONDOWN:
            break;
        case SDL_MOUSEBUTTONUP:
            break;
        case SDL_JOYAXISMOTION:
            break;
        case SDL_JOYBUTTONDOWN:
            break;
        case SDL_JOYBUTTONUP:
            break;
        case SDL_JOYBALLMOTION:
            break;
        case SDL_JOYHATMOTION:
            break;
        default:
            throw ("Unknown SDL Event Inserted");
            break;
    }
}

void MetaCode::Construct(int MetaValue_, short unsigned int ID_, MetaCode::MetaCode::InputCode Code_)
{
    SetMetaValue(MetaValue_);
    SetID(ID_);
    SetCode(Code_);
}

int MetaCode::GetMetaValue()
{
    return this->MetaValue;
}

MetaCode::InputCode MetaCode::GetCode()
{
    return this->Code;
}

short unsigned int MetaCode::GetID()
{
    return this->ID;
}

void MetaCode::SetMetaValue(int MetaValue_)
{
    this->MetaValue=MetaValue_;
}

void MetaCode::SetCode(MetaCode::InputCode Code_)
{
    this->Code=Code_;
}

void MetaCode::SetID(short unsigned int ID_)
{
    this->ID=ID_;
}

bool MetaCode::operator==(const MetaCode &other) const
{
    if(this->Code == other.Code && this->MetaValue == other.MetaValue)
    {
        return true;
    }else{
        return false;
    }
}


///////////////////////////////////////////////////////////////////////////////
// PhysEventUserInput
PhysEventUserInput::PhysEventUserInput()
{

}

PhysEventUserInput::PhysEventUserInput(MetaCode Code_)
{
    Code.push_back(Code_);
}

PhysEventUserInput::PhysEventUserInput(vector<MetaCode> Code_)
{
    for(unsigned int c=0; Code_.size()>c ; c++)
    {
        Code.push_back(Code_.at(c));
    }
}

PhysEventUserInput::~PhysEventUserInput()
{

}

MetaCode PhysEventUserInput::GetCode(unsigned int Index)
{
    return Code.at(Index);
}

unsigned int PhysEventUserInput::GetCodeCount()
{
    return Code.size();
}

void PhysEventUserInput::AddCode(MetaCode _Code)
{
    Code.push_back(_Code);
}

void PhysEventUserInput::ToggleCode(MetaCode _Code)
{
    vector<MetaCode>::iterator iter;

    for(iter=Code.begin(); Code.end()!=iter ; iter++)
    {
        if(*iter == _Code)
        {
            Code.erase(iter);
        }else{
            Code.push_back(_Code);
        }
    }
}

void PhysEventUserInput::ToggleCode(unsigned int Index)
{
    Code.erase(Code.begin()+Index);
}

EventType PhysEventUserInput::getEventType()
{
    return UserInput;
}



#endif
