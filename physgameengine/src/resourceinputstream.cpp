//© Copyright 2010 BlackTopp Studios Inc.
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
#ifndef _resourceinputstream_cpp
#define _resourceinputstream_cpp

#include "resourceinputstream.h"

#include "internalogredatastreambuf.h.cpp"

#ifdef PHYSDEBUG
    #include "world.h"
#endif

namespace phys
{
	void ResourceInputStream::Construct(std::streambuf *InputBuffer, ResourceManager* ResourceManager_)
	{
		#ifdef PHYSDEBUG
		World::GetWorldPointer()->Log("Entering ResourceInputStream::Construct()");
		#endif
		this->OgreBuffer=static_cast <internal::OgreDataStreamBuf*>(InputBuffer);
		this->Manager=ResourceManager_;
		 #ifdef PHYSDEBUG
		World::GetWorldPointer()->Log("Exiting ResourceInputStream::Construct()");
		#endif
	}

	ResourceInputStream::~ResourceInputStream()
	{
		#ifdef PHYSDEBUG
		World::GetWorldPointer()->Log("Entering ResourceInputStream::~ResourceInputStream()");
		#endif
		delete this->OgreBuffer;
		#ifdef PHYSDEBUG
		World::GetWorldPointer()->Log("Exiting ResourceInputStream::~ResourceInputStream()");
		#endif
	}

	bool ResourceInputStream::Readable()
	{
		return this->OgreBuffer->Readable();
	}

	bool ResourceInputStream::Writeable()
	{
		return this->OgreBuffer->Writeable();
	}

/*	std::istream& ResourceInputStream::operator >> ( String& Data )
		{ return (this->OgreBuffer) >> Data; }

	std::istream& ResourceInputStream::operator >> ( Whole& Data )
		{ return (this->OgreBuffer) >> Data; }

	std::istream& ResourceInputStream::operator >> ( Real& Data )
		{ return (staticcastthis >> Data); }*/
}

#endif

