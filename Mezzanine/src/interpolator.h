// © Copyright 2010 - 2013 BlackTopp Studios Inc.
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
#ifndef _interpolator_h
#define _interpolator_h

/// @file
/// @brief Helper classes to assist in generating data points between two other data points.

#include "datatypes.h"

namespace Mezzanine
{

    /// @brief An empty class to prevent classes without explicit interpolater traits from compiling.
    template <typename T>
    class NotAnInterpolator {};

    /// @brief The generic Interpolatable Traits, intended to catch all class without explicit traits set and server as and example.
    template<typename T>
    class InterpolatableTraits
    {
        public:
            /// @brief If something specifically needs the linear interpolator for T they should use this.
            /// @details This interpolator provides certain guarantees.
            ///     - All data points will be valid interpolated values.
            ///     - There are "corners".
            ///     - Interpolation will be constant/O(1) time (and as fast as reasonable).
            ///     - This shape defined by interpolating a set of these will not leave a Convex Hull(or Axis Aligned Bounding Box) that could contain the data
            ///     - Will not take multiple pair of points into account.
            /// Corners can be thought of as any non-smooth change, and may not be intuitively in some interpolatable
            /// types.
            typedef NotAnInterpolator<T> LinearInterpolator;

            /// @brief If something specifically needs the Bezier interpolator for T they should use this.
            /// @details This interpolator provides different guarantees different from the linear one:
            ///     - Data points, might not be valid interpolated values
            ///     - There are no "Corners".
            ///     - Execution time will be (N)log(N) or better.
            ///     - This shape defined by interpolating a set of these will not leave a Convex Hull(or Axis Aligned Bounding Box) that could contain the data.
            ///     - Will be able to provide interpolated values for a small set of data points.
            /// There might be corners when connecting 2 different bezier curves if not careful, any
            /// bezier implementation taking a finite amount of point cannot help this.
            typedef NotAnInterpolator<T> BezierInterpolator;

            /// @brief If something specifically needs the linear interpolator for T they should use this.
            /// @details This is with be a cubic spline where applicable, and will be
            /// more smooth that the others, and be at least as intuitive as the linear version:
            ///     - Data points, will be valid interpolated values.
            ///     - There are no "Corners".
            ///     - Execution time is better than N^2.
            ///     - This shape defined by interpolating a set of these *will* leave a Convex Hull(or Axis Aligned Bounding Box) that could contain the data.
            ///     - Will be able to interpolated arbitrary sets of data points.
            typedef NotAnInterpolator<T> SplineInterpolator;
    };

} // /namespace Mezzanine

#endif // Include guard
