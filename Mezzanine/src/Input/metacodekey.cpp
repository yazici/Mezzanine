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
#ifndef _inputmetacodekey_cpp
#define _inputmetacodekey_cpp

#include "Input/metacodekey.h"

namespace Mezzanine
{
    namespace Input
    {
        ///////////////////////////////////////////////////////////////////////////////
        // Compare Methods

        Boole KeyboardCodeCompare(const MetaCode& First, const MetaCode& Second)
        {
            return First.GetCode() < Second.GetCode();
        }

        Boole MouseCodeCompare(const MetaCode& First, const MetaCode& Second)
        {
            return First.GetCode() < Second.GetCode();
        }

        Boole MultitouchCodeCompare(const MetaCode& First, const MetaCode& Second)
        {
            /// @todo When our support for Multitouch devices expands this may need to be expanded as well.
            return First.GetCode() < Second.GetCode();
        }

        Boole ControllerCodeCompare(const MetaCode& First, const MetaCode& Second)
        {
            if( First.GetDeviceID() == Second.GetDeviceID() ) {
                return First.GetCode() < Second.GetCode();
            }else{
                return First.GetDeviceID() < Second.GetDeviceID();
            }
        }

        Boole SequenceCodeCompare(const MetaCode& First, const MetaCode& Second)
        {
            if( First.GetDeviceID() == Second.GetDeviceID() ) {
                return First.GetMetaValue() < Second.GetMetaValue();
            }else{
                return First.GetDeviceID() < Second.GetDeviceID();
            }
        }

        Boole MultiDeviceCompare(const MetaCode& First, const MetaCode& Second)
        {
            if( First.GetDeviceType() == Second.GetDeviceType() ) {
                if( First.GetDeviceID() == Second.GetDeviceID() ) {
                    return First.GetCode() < Second.GetCode();
                }else{
                    return First.GetDeviceID() < Second.GetDeviceID();
                }
            }else{
                return First.GetDeviceType() < Second.GetDeviceType();
            }
        }

        ///////////////////////////////////////////////////////////////////////////////
        // MetaCodeKey Methods

        MetaCodeKey::MetaCodeKey(const MetaCode& Code) :
            MCode(Code)
        {
            switch( this->MCode.GetDeviceType() )
            {
                case Input::DEVICE_KEYBOARD:   this->Compare = &KeyboardCodeCompare;   break;
                case Input::DEVICE_MOUSE:      this->Compare = &MouseCodeCompare;      break;
                case Input::DEVICE_MULTITOUCH: this->Compare = &MultitouchCodeCompare; break;
                case Input::DEVICE_CONTROLLER: this->Compare = &ControllerCodeCompare; break;
                default:
                {
                    if( Input::COMPOUNDINPUT_CUSTOMSEQUENCE == this->MCode.GetCode() ) {
                        this->Compare = &SequenceCodeCompare;
                    }else{
                        MEZZ_EXCEPTION(ExceptionBase::PARAMETERS_EXCEPTION,"Attempting to construct a MetaCodeKey representing an unknown type.");
                    }
                }
            }
        }

        ///////////////////////////////////////////////////////////////////////////////
        // Operators

        Boole MetaCodeKey::operator<(const MetaCodeKey& Other) const
        {
            if( this->MCode.GetDeviceType() == Other.MCode.GetDeviceType() ) {
                return Compare(MCode,Other.MCode);
            }else{
                return this->MCode.GetDeviceType() < Other.MCode.GetDeviceType();
            }
        }

        ///////////////////////////////////////////////////////////////////////////////
        // Utility

        const MetaCode& MetaCodeKey::GetKeyData() const
            { return this->MCode; }
    }
}

#endif
