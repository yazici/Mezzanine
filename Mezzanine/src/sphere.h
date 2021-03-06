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
#ifndef _sphere_h
#define _sphere_h

/// @file
/// @brief This file contains a generic Sphere class for math and spacial query.

#ifndef SWIG
    #include "MathTools/mathtypes.h"
#endif

#include "vector3.h"

namespace Ogre
{
    class Sphere;
}

namespace Mezzanine
{
    class AxisAlignedBox;
    class Plane;
    class Ray;
    ///////////////////////////////////////////////////////////////////////////////
    /// @brief This is a generic sphere class used for spacial queries.
    ///////////////////////////////////////
    class MEZZ_LIB Sphere
    {
    public:
        /// @brief This is a type used for the return of a ray intersection test.
        /// @details This type provides more verbose return data that can be used for further tests.  @n @n
        /// The first member stores whether or not there was a hit.  The second member stores ray containing the points where the ray entered and exited the sphere.
        typedef MathTools::GeometryRayTestResult RayTestResult;

        ///////////////////////////////////////////////////////////////////////////////
        // Public Data Members

        /// @brief The point in world space that is the center of the sphere.
        Vector3 Center;
        /// @brief The radius of the sphere.
        Real Radius;

        ///////////////////////////////////////////////////////////////////////////////
        // Construction and Destruction

        /// @brief Blank constructor.
        Sphere();
        /// @brief Copy constructor.
        /// @param Other The other sphere to copy from.
        Sphere(const Sphere& Other) = default;
        /// @brief Move constructor.
        /// @param Other The other sphere to be moved.
        Sphere(Sphere&& Other) = default;
        /// @brief Radius constructor.
        /// @param SphereRadius The radius of the sphere.
        Sphere(const Real SphereRadius);
        /// @brief Descriptive constructor.
        /// @param SphereCenter The point in world space that is the center of the sphere.
        /// @param SphereRadius The radius of the sphere.
        Sphere(const Vector3& SphereCenter, const Real SphereRadius);
        /// @brief Internal constructor.
        /// @param InternalSphere The internal Ogre Sphere to construct this Sphere from.
        explicit Sphere(const Ogre::Sphere& InternalSphere);
        /// @brief Class destructor.
        ~Sphere();

        ///////////////////////////////////////////////////////////////////////////////
        // Utility

        /// @brief Checks to see if a point is inside this sphere.
        /// @param ToCheck The location to check to see if it is within this sphere.
        /// @return Returns true if the point provided is within this sphere, false otherwise.
        Boole IsInside(const Vector3& ToCheck) const;
        /// @brief Checks to see if another sphere overlaps with this one.
        /// @param ToCheck The other sphere to check for overlap.
        /// @return Returns true if the provided sphere overlaps with this sphere, false otherwise.
        Boole IsOverlapping(const Sphere& ToCheck) const;
        /// @brief Checks to see if an AABB overlaps with this sphere.
        /// @param ToCheck The AABB to check for overlap.
        /// @return Returns true if the AABB overlaps with this sphere, false otherwise.
        Boole IsOverlapping(const AxisAlignedBox& ToCheck) const;
        /// @brief Checks to see if a plane intersects this sphere.
        /// @param ToCheck The plane to check for intersection.
        /// @return Returns true if the provided plane intersects with this sphere, false otherwise.
        Boole IsOverlapping(const Plane& ToCheck) const;
        /// @brief Checks to see if a ray intersects this sphere.
        /// @param ToCheck The ray to check for a hit.
        /// @return Returns a std::pair containing whether or not the ray hit, and if it did the subsection of the ray that went through the sphere.
        RayTestResult Intersects(const Ray& ToCheck) const;

        ///////////////////////////////////////////////////////////////////////////////
        // Conversion Methods

        /// @brief Changes this Sphere to match the Ogre Sphere.
        /// @param InternalSphere The Ogre::Sphere to copy.
        void ExtractOgreSphere(const Ogre::Sphere& InternalSphere);
        /// @brief Gets an Ogre::Sphere that contains this Spheres information.
        /// @return This returns an Ogre::Sphere that contains the same information as this Spheres information.
        Ogre::Sphere GetOgreSphere() const;

        ///////////////////////////////////////////////////////////////////////////////
        // Serialization

        /// @brief Convert this class to an XML::Node ready for serialization.
        /// @param ParentNode The point in the XML hierarchy that all this shape should be appended to.
        void ProtoSerialize(XML::Node& ParentNode) const;
        /// @brief Take the data stored in an XML Node and overwrite this object with it.
        /// @param SelfRoot An XML::Node containing the data to populate the new instance with.
        void ProtoDeSerialize(const XML::Node& SelfRoot);

        /// @brief Get the name of the the XML tag this class will leave behind as its instances are serialized.
        /// @return A string containing the name of this class.
        static String GetSerializableName();

        ///////////////////////////////////////////////////////////////////////////////
        // Operators

        /// @brief Assignment operator.
        /// @param Other The other Sphere to copy from.
        /// @return Returns a reference to this.
        Sphere& operator=(const Sphere& Other) = default;
        /// @brief Move assignment operator.
        /// @param Other The other Sphere to be moved.
        /// @return Returns a reference to this.
        Sphere& operator=(Sphere&& Other) = default;

        /// @brief The assignment operator from Ogre::Sphere to Mezzanine::Sphere.
        /// @param InternalSphere The Ogre::Sphere to take data from.
        void operator=(const Ogre::Sphere& InternalSphere);

        /// @brief Greater-than operator.
        /// @note This operator compares the radii of both spheres.
        /// @param Other The other Sphere to compare with.
        /// @return Returns true if this Sphere is larger than the other provided Sphere, false otherwise.
        Boole operator>(const Sphere& Other) const;
        /// @brief Greater-than or equals-to operator.
        /// @note This operator compares the radii of both spheres.
        /// @param Other The other Sphere to compare with.
        /// @return Returns true if this Sphere is larger than or equal to the other provided Sphere, false otherwise.
        Boole operator<(const Sphere& Other) const;
        /// @brief Less-than operator.
        /// @note This operator compares the radii of both spheres.
        /// @param Other The other Sphere to compare with.
        /// @return Returns true if this Sphere is smaller than the other provided Sphere, false otherwise.
        Boole operator>=(const Sphere& Other) const;
        /// @brief Less-than or equals-to operator.
        /// @note This operator compares the radii of both spheres.
        /// @param Other The other Sphere to compare with.
        /// @return Returns true if this Sphere is smaller than or equal to the other provided Sphere, false otherwise.
        Boole operator<=(const Sphere& Other) const;

        /// @brief Equality operator.
        /// @param Other The other Sphere to compare with.
        /// @return Returns true if this Sphere is the same as the other provided Sphere, false otherwise.
        Boole operator==(const Sphere& Other) const;
        /// @brief Inequality operator.
        /// @param Other The other Sphere to compare with.
        /// @return Returns true if this Sphere is not the same as the other provided Sphere, false otherwise.
        Boole operator!=(const Sphere& Other) const;
    };//Sphere
}//Mezzanine

#endif
