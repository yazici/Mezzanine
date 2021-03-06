// � Copyright 2010 - 2018 BlackTopp Studios Inc.
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
#ifndef _staticanytests_h
#define _staticanytests_h

#include "mezztest.h"
#include "staticany.h"

#include <stdexcept> //only used to throw for TEST_THROW

/// @file
/// @brief This file contains all the tests for the StaticAny container class.

using namespace Mezzanine;
using namespace Mezzanine::Testing;

/// @brief A series of tests for the StaticAny container class.
class staticanytests : public UnitTestGroup
{
public:
    /// @copydoc Mezzanine::Testing::UnitTestGroup::Name
    /// @return Returns a String containing "StaticAny".
    virtual String Name()
        { return String("StaticAny"); }

    /// @brief This is called when Automatic tests are run.
    void RunAutomaticTests()
    {
        {// Traits
            using AnyType = StaticAny<12,4>;
            static_assert( is_static_any<AnyType>::value, "Supposed to Pass." );
            static_assert( !is_static_any<int>::value, "Supposed to Fail." );
            static_assert( is_static_any_v<AnyType>, "Supposed to Pass." );
            static_assert( !is_static_any_v<float>, "Supposed to Fail." );
            static_assert( is_static_any_decayed<AnyType&>::value, "Supposed to Pass." );
            static_assert( !is_static_any_decayed<int>::value, "Supposed to Fail." );
            static_assert( is_static_any_decayed_v<const AnyType&>, "Supposed to Pass." );
            static_assert( !is_static_any_decayed_v<float>, "Supposed to Fail." );

            TEST( is_static_any<AnyType>::value,
                  "is_static_any<ElementType>-Pass" );
            TEST( !is_static_any<int>::value,
                  "is_static_any<ElementType>-Fail" );
            TEST( is_static_any_v<AnyType>,
                  "is_static_any_v<ElementType>-Pass" );
            TEST( !is_static_any_v<const AnyType>,
                  "is_static_any_v<ElementType>-Fail" );
            TEST( is_static_any_decayed<AnyType&>::value,
                  "is_static_any_decayed<ElementType>-Pass" );
            TEST( !is_static_any_decayed<int>::value,
                  "is_static_any_decayed<ElementType>-Fail" );
            TEST( is_static_any_decayed_v<const AnyType&>,
                  "is_static_any_decayed_v<ElementType>-Pass" );
            TEST( !is_static_any_decayed_v<float>,
                  "is_static_any_decayed_v<ElementType>-Fail" );
        }// Traits

        {// Casting
            using AnyType = StaticAny<40>;

            AnyType StringAny( String("Test") );
            const AnyType ConstStringAny( String("ConstTest") );
            TEST( StaticAnyCast<String>(StringAny) == "Test",
                  "StaticAnyCast<String>(StaticAny<>)-Ref-Pass");
            TEST( StaticAnyCast<String>(ConstStringAny) == "ConstTest",
                  "StaticAnyCast<String>(StaticAny<>)-ConstRef-Pass");
            TEST_THROW( std::bad_cast,
                        StaticAnyCast< std::vector<char> >(StringAny),
                        "StaticAnyCast<String>(StaticAny<>)-Ref-Fail");
            TEST_THROW( std::bad_cast,
                        StaticAnyCast< std::vector<char> >(ConstStringAny),
                        "StaticAnyCast<String>(StaticAny<>)-ConstRef-Fail");

            double Archimedes(3.1415926);
            const double Eulers(2.7182818);
            AnyType DoubleAny( Archimedes );
            const AnyType ConstDoubleAny( Eulers );
            TEST_EQUAL_EPSILON( *StaticAnyCast<double>(&DoubleAny),Archimedes,
                                "StaticAnyCast<double>(StaticAny<>)-Ptr-Pass");
            TEST_EQUAL_EPSILON( *StaticAnyCast<double>(&ConstDoubleAny),Eulers,
                                "StaticAnyCast<double>(StaticAny<>)-ConstPtr-Pass");
            TEST_THROW( std::bad_cast,
                        StaticAnyCast<float>(&DoubleAny),
                        "StaticAnyCast<double>(StaticAny<>)-Ptr-Fail");
            TEST_THROW( std::bad_cast,
                        StaticAnyCast<float>(&ConstDoubleAny),
                        "StaticAnyCast<double>(StaticAny<>)-ConstPtr-Fail");

            AnyType SharedPtrAny( std::make_shared<int>(123) );
            const AnyType ConstSharedPtrAny( std::make_shared<short>( short(12345) ) );
            TEST( *( StaticAnyCast< std::shared_ptr<int> >(SharedPtrAny) ) == int(123),
                  "StaticAnyCast<_std::shared_ptr<int>_>(StaticAny<>)-Pass");
            TEST( *( StaticAnyCast< std::shared_ptr<short> >(ConstSharedPtrAny) ) == short(12345),
                  "StaticAnyCast<_std::shared_ptr<int>_>(StaticAny<>)-Pass");
            TEST_THROW( std::bad_cast,
                        StaticAnyCast< std::shared_ptr<char> >(SharedPtrAny),
                        "StaticAnyCast<_std::shared_ptr<int>_>(StaticAny<>)-Fail");
            TEST_THROW( std::bad_cast,
                        StaticAnyCast< std::shared_ptr<char> >(ConstSharedPtrAny),
                        "StaticAnyCast<_std::shared_ptr<int>_>(StaticAny<>)-Fail");


        }// Casting

        {// Construct / Destruct
            using AnyType = StaticAny<32,alignof(std::shared_ptr<String>)>;

            std::shared_ptr<String> StringPtr = std::make_shared<String>("Test");
            AnyType FirstAny( StringPtr );
            TEST( StringPtr.use_count() == 2,
                  "StaticAny-ConstructElement");
            FirstAny.clear();
            TEST( StringPtr.use_count() == 1,
                  "StaticAny-DestructElement");
        }// Construct / Destruct

        {// Integer
            using AnyType = StaticAny<sizeof(unsigned),alignof(unsigned)>;
            using SmallerAnyType = StaticAny<sizeof(short),alignof(short)>;

            //
            // Constructors
            //

            AnyType EmptyAny;
            TEST( EmptyAny.empty(),
                  "StaticAny::StaticAny()-Unsigned" );

            AnyType CopyAnySource( unsigned(321) );
            AnyType CopyAnyDest(CopyAnySource);
            TEST( StaticAnyCast<unsigned>(CopyAnySource) == unsigned(321),
                  "StaticAny::StaticAny(const_StaticAny&)-Unsigned" );

            AnyType MoveAnySource( unsigned(12345) );
            AnyType MoveAnyDest( std::move(MoveAnySource) );
            TEST( StaticAnyCast<unsigned>(MoveAnyDest) == unsigned(12345),
                  "StaticAny::StaticAny(StaticAny&&)-Dest-Unsigned" );
            TEST( MoveAnySource.empty(),
                  "StaticAny::StaticAny(StaticAny&&)-Source-Unsigned" );

            SmallerAnyType SmallerAny( short(54321) );
            AnyType CopySmallerAny(SmallerAny);
            TEST( StaticAnyCast<short>(CopySmallerAny) == short(54321),
                  "StaticAny::StaticAny(const_StaticAny<size_t>&)-Unsigned" );

            SmallerAnyType SmallerMoveSource( short(123) );
            AnyType SmallerMoveDest( std::move(SmallerMoveSource) );
            TEST( StaticAnyCast<short>(SmallerMoveDest) == short(123),
                  "StaticAny::StaticAny(StaticAny<size_t>&&)-Dest-Unsigned" );
            TEST( SmallerMoveSource.empty(),
                  "StaticAny::StaticAny(StaticAny<size_t>&&)-Source-Unsigned" );

            AnyType AnythingAny( unsigned(1337) );
            TEST( StaticAnyCast<unsigned>(AnythingAny) == unsigned(1337),
                  "StaticAny::StaticAny(ElementType&&)-Unsigned" );

            //
            // Assignment operators
            //

            AnyType CopyAssignDest;
            TEST( CopyAssignDest.empty(),
                  "StaticAny::operator=(const_StaticAny&)-Empty-Unsigned" );
            AnyType CopyAssignSource( unsigned(144) );
            CopyAssignDest = CopyAssignSource;
            TEST( StaticAnyCast<unsigned>(CopyAssignDest) == unsigned(144),
                  "StaticAny::operator=(const_StaticAny&)-Unsigned" );

            AnyType MoveAssignDest;
            TEST( MoveAssignDest.empty(),
                  "StaticAny::operator=(StaticAny&&)-Empty-Unsigned" );
            AnyType MoveAssignSource( unsigned(288) );
            MoveAssignDest = std::move( MoveAssignSource );
            TEST( StaticAnyCast<unsigned>(MoveAssignDest) == unsigned(288),
                  "StaticAny::operator=(StaticAny&&)-Dest-Unsigned" );
            TEST( MoveAssignSource.empty(),
                  "StaticAny::operator=(StaticAny&&)-Source-Unsigned" );

            AnyType CopyAssignSmallerDest;
            TEST( CopyAssignSmallerDest.empty(),
                  "StaticAny::operator=(const_StaticAny<size_t>&)-Empty-Unsigned" );
            SmallerAnyType CopyAssignSmallerSource( short(576) );
            CopyAssignSmallerDest = CopyAssignSmallerSource;
            TEST( StaticAnyCast<short>(CopyAssignSmallerDest) == short(576),
                  "StaticAny::operator=(const_StaticAny<size_t>&)-Unsigned" );

            AnyType MoveAssignSmallerDest;
            TEST( MoveAssignSmallerDest.empty(),
                  "StaticAny::operator=(StaticAny<size_t>&&)-Empty-Unsigned" );
            SmallerAnyType MoveAssignSmallerSource( short(1152) );
            MoveAssignSmallerDest = std::move( MoveAssignSmallerSource );
            TEST( StaticAnyCast<short>(MoveAssignSmallerDest) == short(1152),
                  "StaticAny::operator=(StaticAny<size_t>&&)-Dest-Unsigned" );
            TEST( MoveAssignSmallerSource.empty(),
                  "StaticAny::operator=(StaticAny<size_t>&&)-Source-Unsigned" );

            AnyType AnythingAnyAssignDest;
            TEST( AnythingAnyAssignDest.empty(),
                  "StaticAny::operator=(ElementType&&)-Empty-Unsigned" );
            AnythingAnyAssignDest = unsigned(8675309);
            TEST( StaticAnyCast<unsigned>(AnythingAnyAssignDest) == unsigned(8675309),
                  "StaticAny::operator=(ElementType&&)-Unsigned" );

            //
            // Utility
            //

            AnyType UtilityAny;
            TEST( AnyType::capacity() == sizeof(unsigned),
                  "StaticAny::capacity()-Unsigned" );
            TEST( SmallerAnyType::capacity() == sizeof(short),
                  "StaticAny::capacity()-Short" );

            TEST( AnyType::align() == alignof(unsigned),
                  "StaticAny::align()-Unsigned" );
            TEST( SmallerAnyType::align() == alignof(short),
                  "StaticAny::align()-Short" );

            TEST( UtilityAny.empty(),
                  "StaticAny::empty()_const-Unassigned-Unsigned" );
            TEST( UtilityAny.size() == 0,
                  "StaticAny::size()_const-Unassigned-Unsigned" );
            TEST( UtilityAny.get_type() == typeid(void),
                  "StaticAny::get_type()_const-Unassigned-Unsigned" );

            UtilityAny.emplace<unsigned>(2304);
            TEST( StaticAnyCast<unsigned>(UtilityAny) == unsigned(2304),
                  "StaticAny::emplace(ArgTypes&&...)-Unsigned" );

            TEST( !UtilityAny.empty(),
                  "StaticAny::empty()_const-Assigned-Unsigned" );
            TEST( UtilityAny.size() == sizeof(unsigned),
                  "StaticAny::size()_const-Assigned-Unsigned" );
            TEST( UtilityAny.get_type() == typeid(unsigned),
                  "StaticAny::get_type()_const-Assigned-Unsigned" );

            UtilityAny = char('A');
            TEST( StaticAnyCast<char>(UtilityAny) == char('A'),
                  "StaticAny::emplace(ArgTypes&&...)-Unsigned-Char" );

            TEST( !UtilityAny.empty(),
                  "StaticAny::empty()_const-Unsigned-Char" );
            TEST( UtilityAny.size() == sizeof(char),
                  "StaticAny::size()_const-Unsigned-Char" );
            TEST( UtilityAny.get_type() == typeid(char),
                  "StaticAny::get_type()_const-Unsigned-Char" );

            UtilityAny.clear();
            TEST( UtilityAny.empty(),
                  "StaticAny::clear()-Unsigned" );
        }// Integer

        {// String
            using AnyType = StaticAny<sizeof(String),alignof(String)>;
            using SmallerAnyType = StaticAny<(sizeof(char) * 3)>;

            //
            // Constructors
            //

            AnyType EmptyAny;
            TEST( EmptyAny.empty(),
                  "StaticAny::StaticAny()-String" );

            AnyType CopyAnySource( String("321") );
            AnyType CopyAnyDest(CopyAnySource);
            TEST( StaticAnyCast<String>(CopyAnySource) == String("321"),
                  "StaticAny::StaticAny(const_StaticAny&)-String" );

            AnyType MoveAnySource( String("1234") );
            AnyType MoveAnyDest( std::move(MoveAnySource) );
            TEST( StaticAnyCast<String>(MoveAnyDest) == String("1234"),
                  "StaticAny::StaticAny(StaticAny&&)-Dest-String" );
            TEST( MoveAnySource.empty(),
                  "StaticAny::StaticAny(StaticAny&&)-Source-String" );

            SmallerAnyType SmallerAny( char('$') );
            AnyType CopySmallerAny(SmallerAny);
            TEST( StaticAnyCast<char>(CopySmallerAny) == char('$'),
                  "StaticAny::StaticAny(const_StaticAny<size_t>&)-String" );

            SmallerAnyType SmallerMoveSource( char('@') );
            AnyType SmallerMoveDest( std::move(SmallerMoveSource) );
            TEST( StaticAnyCast<char>(SmallerMoveDest) == char('@'),
                  "StaticAny::StaticAny(StaticAny<size_t>&&)-Dest-String" );
            TEST( SmallerMoveSource.empty(),
                  "StaticAny::StaticAny(StaticAny<size_t>&&)-Source-String" );

            AnyType AnythingAny( String("1337") );
            TEST( StaticAnyCast<String>(AnythingAny) == String("1337"),
                  "StaticAny::StaticAny(ElementType&&)-String" );

            //
            // Assignment operators
            //

            AnyType CopyAssignDest;
            TEST( CopyAssignDest.empty(),
                  "StaticAny::operator=(const_StaticAny&)-Empty-String" );
            AnyType CopyAssignSource( String("144") );
            CopyAssignDest = CopyAssignSource;
            TEST( StaticAnyCast<String>(CopyAssignDest) == String("144"),
                  "StaticAny::operator=(const_StaticAny&)-String" );

            AnyType MoveAssignDest;
            TEST( MoveAssignDest.empty(),
                  "StaticAny::operator=(StaticAny&&)-Empty-String" );
            AnyType MoveAssignSource( String("288") );
            MoveAssignDest = std::move( MoveAssignSource );
            TEST( StaticAnyCast<String>(MoveAssignDest) == String("288"),
                  "StaticAny::operator=(StaticAny&&)-Dest-String" );
            TEST( MoveAssignSource.empty(),
                  "StaticAny::operator=(StaticAny&&)-Source-String" );

            AnyType CopyAssignSmallerDest;
            TEST( CopyAssignSmallerDest.empty(),
                  "StaticAny::operator=(const_StaticAny<size_t>&)-Empty-String" );
            SmallerAnyType CopyAssignSmallerSource( char('$') );
            CopyAssignSmallerDest = CopyAssignSmallerSource;
            TEST( StaticAnyCast<char>(CopyAssignSmallerDest) == char('$'),
                  "StaticAny::operator=(const_StaticAny<size_t>&)-String" );

            AnyType MoveAssignSmallerDest;
            TEST( MoveAssignSmallerDest.empty(),
                  "StaticAny::operator=(StaticAny<size_t>&&)-Empty-String" );
            SmallerAnyType MoveAssignSmallerSource( char('@') );
            MoveAssignSmallerDest = std::move( MoveAssignSmallerSource );
            TEST( StaticAnyCast<char>(MoveAssignSmallerDest) == char('@'),
                  "StaticAny::operator=(StaticAny<size_t>&&)-Dest-String" );
            TEST( MoveAssignSmallerSource.empty(),
                  "StaticAny::operator=(StaticAny<size_t>&&)-Source-String" );

            AnyType AnythingAnyAssignDest;
            TEST( AnythingAnyAssignDest.empty(),
                  "StaticAny::operator=(ElementType&&)-Empty-String" );
            AnythingAnyAssignDest = String("8675309");
            TEST( StaticAnyCast<String>(AnythingAnyAssignDest) == String("8675309"),
                  "StaticAny::operator=(ElementType&&)-String" );

            //
            // Utility
            //

            AnyType UtilityAny;
            TEST( AnyType::capacity() == sizeof(String),
                  "StaticAny::capacity()-String" );
            TEST( SmallerAnyType::capacity() == (sizeof(char) * 3),
                  "StaticAny::capacity()-CharString" );

            TEST( AnyType::align() == sizeof(String),
                  "StaticAny::align()-String" );
            TEST( SmallerAnyType::align() == (sizeof(char) * 3),
                  "StaticAny::align()-CharString" );

            TEST( UtilityAny.empty(),
                  "StaticAny::empty()_const-Unassigned-String" );
            TEST( UtilityAny.size() == 0,
                  "StaticAny::size()_const-Unassigned-String" );
            TEST( UtilityAny.get_type() == typeid(void),
                  "StaticAny::get_type()_const-Unassigned-String" );

            UtilityAny.emplace<String>("2304");
            TEST( StaticAnyCast<String>(UtilityAny) == String("2304"),
                  "StaticAny::emplace(ArgTypes&&...)-String" );

            TEST( !UtilityAny.empty(),
                  "StaticAny::empty()_const-Assigned-String" );
            TEST( UtilityAny.size() == sizeof(String),
                  "StaticAny::size()_const-Assigned-String" );
            TEST( UtilityAny.get_type() == typeid(String),
                  "StaticAny::get_type()_const-Assigned-String" );

            UtilityAny = char('a');
            TEST( StaticAnyCast<char>(UtilityAny) == char('a'),
                  "StaticAny::emplace(ArgTypes&&...)-String-Char" );

            TEST( !UtilityAny.empty(),
                  "StaticAny::empty()_const-Assigned-String-Char" );
            TEST( UtilityAny.size() == sizeof(char),
                  "StaticAny::size()_const-Assigned-String-Char" );
            TEST( UtilityAny.get_type() == typeid(char),
                  "StaticAny::get_type()_const-Assigned-String-Char" );

            UtilityAny.clear();
            TEST( UtilityAny.empty(),
                  "StaticAny::clear()-String" );
        }// String

        {// Custom

        }// Custom
    }
    /// @brief Since RunAutomaticTests is implemented so is this.
    /// @return Returns true.
    virtual bool HasAutomaticTests() const
        { return true; }
};//staticanytests

#endif
