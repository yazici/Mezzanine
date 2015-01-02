// © Copyright 2010 - 2014 BlackTopp Studios Inc.
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
// Copyright (C) 2003, 2004 Jason Bevins
//
// This library is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation; either version 2.1 of the License, or (at
// your option) any later version.
//
// This library is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
// License (COPYING.txt) for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this library; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
// The developer's email is jlbezigvins@gmzigail.com (for great email, take
// off every 'zig'.)
//

#ifndef noisenoisegen_cpp
#define noisenoisegen_cpp

#include "noisegen.h"
#include "interpolate.h"
#include "vectortable.h"

// Specifies the version of the coherent-noise functions to use.
// - Set to 2 to use the current version.
// - Set to 1 to use the flawed version from the original version of libnoise.
// If your application requires coherent-noise values that were generated by
// an earlier version of libnoise, change this constant to the appropriate
// value and recompile libnoise.
#define NOISE_VERSION 2

namespace Mezzanine
{
    namespace Noise
    {
        // These constants control certain parameters that all coherent-noise
        // functions require.
        #if(NOISE_VERSION == 1)
        // Constants used by the original version of libnoise.
        // Because X_NOISE_GEN is not relatively prime to the other values, and
        // Z_NOISE_GEN is close to 256 (the number of random gradient vectors),
        // patterns show up in high-frequency coherent noise.
        const Integer X_NOISE_GEN = 1;
        const Integer Y_NOISE_GEN = 31337;
        const Integer Z_NOISE_GEN = 263;
        const Integer SEED_NOISE_GEN = 1013;
        const Integer SHIFT_NOISE_GEN = 13;
        #else
        // Constants used by the current version of libnoise.
        const Integer X_NOISE_GEN = 1619;
        const Integer Y_NOISE_GEN = 31337;
        const Integer Z_NOISE_GEN = 6971;
        const Integer SEED_NOISE_GEN = 1013;
        const Integer SHIFT_NOISE_GEN = 8;
        #endif

        PreciseReal GradientCoherentNoise3D(const PreciseReal X, const PreciseReal Y, const PreciseReal Z, const Integer Seed, NoiseQuality Quality)
        {
            // Create a unit-length cube aligned along an integer boundary.  This cube
            // surrounds the input point.
            Integer x0 = (X > 0.0? (Integer)X: (Integer)X - 1);
            Integer x1 = x0 + 1;
            Integer y0 = (Y > 0.0? (Integer)Y: (Integer)Y - 1);
            Integer y1 = y0 + 1;
            Integer z0 = (Z > 0.0? (Integer)Z: (Integer)Z - 1);
            Integer z1 = z0 + 1;

            // Map the difference between the coordinates of the input value and the
            // coordinates of the cube's outer-lower-left vertex onto an S-curve.
            PreciseReal xs = 0, ys = 0, zs = 0;
            switch( Quality )
            {
                case NQ_Fast:
                {
                    xs = (X - (PreciseReal)x0);
                    ys = (Y - (PreciseReal)y0);
                    zs = (Z - (PreciseReal)z0);
                    break;
                }
                case NQ_Standard:
                {
                    xs = SCurve3(X - (PreciseReal)x0);
                    ys = SCurve3(Y - (PreciseReal)y0);
                    zs = SCurve3(Z - (PreciseReal)z0);
                    break;
                }
                case NQ_Best:
                {
                    xs = SCurve5(X - (PreciseReal)x0);
                    ys = SCurve5(Y - (PreciseReal)y0);
                    zs = SCurve5(Z - (PreciseReal)z0);
                    break;
                }
            }

            // Now calculate the noise values at each vertex of the cube.  To generate
            // the coherent-noise value at the input point, interpolate these eight
            // noise values using the S-curve value as the interpolant (trilinear
            // interpolation.)
            PreciseReal n0, n1, ix0, ix1, iy0, iy1;
            n0   = GradientNoise3D(X, Y, Z, x0, y0, z0, Seed);
            n1   = GradientNoise3D(X, Y, Z, x1, y0, z0, Seed);
            ix0  = LinearInterpolate(n0, n1, xs);
            n0   = GradientNoise3D(X, Y, Z, x0, y1, z0, Seed);
            n1   = GradientNoise3D(X, Y, Z, x1, y1, z0, Seed);
            ix1  = LinearInterpolate(n0, n1, xs);
            iy0  = LinearInterpolate(ix0, ix1, ys);
            n0   = GradientNoise3D(X, Y, Z, x0, y0, z1, Seed);
            n1   = GradientNoise3D(X, Y, Z, x1, y0, z1, Seed);
            ix0  = LinearInterpolate(n0, n1, xs);
            n0   = GradientNoise3D(X, Y, Z, x0, y1, z1, Seed);
            n1   = GradientNoise3D(X, Y, Z, x1, y1, z1, Seed);
            ix1  = LinearInterpolate(n0, n1, xs);
            iy1  = LinearInterpolate(ix0, ix1, ys);

            return LinearInterpolate(iy0, iy1, zs);
        }

        PreciseReal GradientNoise3D(const PreciseReal fX, const PreciseReal fY, const PreciseReal fZ, const Integer iX, const Integer iY, const Integer iZ, const Integer Seed)
        {
            // Randomly generate a gradient vector given the integer coordinates of the
            // input value.  This implementation generates a random number and uses it
            // as an index into a normalized-vector lookup table.
            Integer vectorIndex = (
                  X_NOISE_GEN    * iX
                + Y_NOISE_GEN    * iY
                + Z_NOISE_GEN    * iZ
                + SEED_NOISE_GEN * Seed)
                & 0xffffffff;
            vectorIndex ^= (vectorIndex >> SHIFT_NOISE_GEN);
            vectorIndex &= 0xff;

            PreciseReal xvGradient = PrefabVectors[(vectorIndex << 2)    ];
            PreciseReal yvGradient = PrefabVectors[(vectorIndex << 2) + 1];
            PreciseReal zvGradient = PrefabVectors[(vectorIndex << 2) + 2];

            // Set up us another vector equal to the distance between the two vectors
            // passed to this function.
            PreciseReal xvPoint = (fX - (PreciseReal)iX);
            PreciseReal yvPoint = (fY - (PreciseReal)iY);
            PreciseReal zvPoint = (fZ - (PreciseReal)iZ);

            // Now compute the dot product of the gradient vector with the distance
            // vector.  The resulting value is gradient noise.  Apply a scaling value
            // so that this noise value ranges from -1.0 to 1.0.
            return ((xvGradient * xvPoint) + (yvGradient * yvPoint) + (zvGradient * zvPoint)) * 2.12;
        }

        Integer IntValueNoise3D(const Integer X, const Integer Y, const Integer Z, const Integer Seed)
        {
            // All constants are primes and must remain prime in order for this noise
            // function to work correctly.
            Integer n = (
                  X_NOISE_GEN    * X
                + Y_NOISE_GEN    * Y
                + Z_NOISE_GEN    * Z
                + SEED_NOISE_GEN * Seed)
                & 0x7fffffff;
            n = (n >> 13) ^ n;
            return (n * (n * n * 60493 + 19990303) + 1376312589) & 0x7fffffff;
        }

        PreciseReal ValueCoherentNoise3D(const PreciseReal X, const PreciseReal Y, const PreciseReal Z, const Integer Seed, const NoiseQuality Quality)
        {
            // Create a unit-length cube aligned along an integer boundary.  This cube
            // surrounds the input point.
            Integer x0 = (X > 0.0? (Integer)X: (Integer)X - 1);
            Integer x1 = x0 + 1;
            Integer y0 = (Y > 0.0? (Integer)Y: (Integer)Y - 1);
            Integer y1 = y0 + 1;
            Integer z0 = (Z > 0.0? (Integer)Z: (Integer)Z - 1);
            Integer z1 = z0 + 1;

            // Map the difference between the coordinates of the input value and the
            // coordinates of the cube's outer-lower-left vertex onto an S-curve.
            PreciseReal xs = 0, ys = 0, zs = 0;
            switch( Quality )
            {
                case NQ_Fast:
                {
                    xs = (X - (PreciseReal)x0);
                    ys = (Y - (PreciseReal)y0);
                    zs = (Z - (PreciseReal)z0);
                    break;
                }
                case NQ_Standard:
                {
                    xs = SCurve3(X - (PreciseReal)x0);
                    ys = SCurve3(Y - (PreciseReal)y0);
                    zs = SCurve3(Z - (PreciseReal)z0);
                    break;
                }
                case NQ_Best:
                {
                    xs = SCurve5(X - (PreciseReal)x0);
                    ys = SCurve5(Y - (PreciseReal)y0);
                    zs = SCurve5(Z - (PreciseReal)z0);
                    break;
                }
            }

            // Now calculate the noise values at each vertex of the cube.  To generate
            // the coherent-noise value at the input point, interpolate these eight
            // noise values using the S-curve value as the interpolant (trilinear
            // interpolation.)
            PreciseReal n0, n1, ix0, ix1, iy0, iy1;
            n0   = ValueNoise3D(x0, y0, z0, Seed);
            n1   = ValueNoise3D(x1, y0, z0, Seed);
            ix0  = LinearInterpolate(n0, n1, xs);
            n0   = ValueNoise3D(x0, y1, z0, Seed);
            n1   = ValueNoise3D(x1, y1, z0, Seed);
            ix1  = LinearInterpolate(n0, n1, xs);
            iy0  = LinearInterpolate(ix0, ix1, ys);
            n0   = ValueNoise3D(x0, y0, z1, Seed);
            n1   = ValueNoise3D(x1, y0, z1, Seed);
            ix0  = LinearInterpolate(n0, n1, xs);
            n0   = ValueNoise3D(x0, y1, z1, Seed);
            n1   = ValueNoise3D(x1, y1, z1, Seed);
            ix1  = LinearInterpolate(n0, n1, xs);
            iy1  = LinearInterpolate(ix0, ix1, ys);
            return LinearInterpolate(iy0, iy1, zs);
        }

        PreciseReal ValueNoise3D(const Integer X, const Integer Y, const Integer Z, const Integer Seed)
        {
            return 1.0 - ( (PreciseReal)IntValueNoise3D(X, Y, Z, Seed) / 1073741824.0 );
        }
    }//Noise
}//Mezzanine

#endif
