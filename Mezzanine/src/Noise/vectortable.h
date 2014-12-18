// � Copyright 2010 - 2014 BlackTopp Studios Inc.
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
// Written by Jason Bevins.  Actually it's the output of a program written
// by me.  I'm not going to copyright a bunch of random numbers (although
// you could probably do so in the States, the way things are going down
// there :-)
//
// This file is in the public domain.
//

#ifndef noisevectortable_h
#define noisevectortable_h

#include "datatypes.h"

namespace Mezzanine
{
    namespace Noise
    {
        /// @brief A table of 256 random normalized vectors.
        /// @details Each row is an (X, Y, Z, 0)
        /// coordinate.  The 0 is used as padding so we can use bit shifts to index
        /// any row in the table.  These vectors have an even statistical
        /// distribution, which improves the quality of the coherent noise
        /// generated by these vectors.  For more information, see "GPU Gems",
        /// Chapter 5 - Implementing Improved Perlin Noise by Ken Perlin,
        /// specifically page 76.
        extern PreciseReal PrefabVectors[256 * 4];
    }//Noise
}//Mezzanine

#endif
