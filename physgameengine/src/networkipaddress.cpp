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
#ifndef _networkipaddress_cpp
#define _networkipaddress_cpp

#include "networkipaddress.h"

namespace phys
{
    namespace Network
    {
        ///////////////////////////////////////////////////////////////////////////////
        // IPAddress member functions

        IPAddress::IPAddress()
        {
        }

        IPAddress::~IPAddress()
        {
        }

        void IPAddress::SetAddress(const String& Address)
        {
            //if(VerifyAddress(Address))
            //    this->SocAddress = Address;
        }

        String IPAddress::GetAddressAsString() const
        {
            //return SocAddress;
        }

        char* IPAddress::GetAddressAsCString() const
        {
            //return SocAddress.c_str();
        }

        const String& IPAddress::GetHostName() const
        {
            return HostName;
        }

        ///////////////////////////////////////////////////////////////////////////////
        // IPAddress_4 member functions

        IPAddress_4::IPAddress_4()
        {

        }

        IPAddress_4::~IPAddress_4()
        {

        }

        bool IPAddress_4::VerifyAddress(const String& Addr)
        {

        }

        IPAddress::IPVersion IPAddress_4::GetVersion() const
        {
            return IPAddress::IP_v4;
        }

        ///////////////////////////////////////////////////////////////////////////////
        // IPAddress_6 member functions

        IPAddress_6::IPAddress_6()
        {

        }

        IPAddress_6::~IPAddress_6()
        {

        }

        bool IPAddress_6::VerifyAddress(const String& Addr)
        {

        }

        IPAddress::IPVersion IPAddress_6::GetVersion() const
        {
            return IPAddress::IP_v6;
        }
    }//Network
}//phys

#endif
