# © Copyright 2010 - 2016 BlackTopp Studios Inc.
# This file is part of The Mezzanine Engine.
#
#    The Mezzanine Engine is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    The Mezzanine Engine is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with The Mezzanine Engine.  If not, see <http://www.gnu.org/licenses/>.
#
#   The original authors have included a copy of the license specified above in the
#   'Docs' folder. See 'gpl.txt'
#
#   We welcome the use of the Mezzanine engine to anyone, including companies who wish to
#   Build professional software and charge for their product.
#
#   However there are some practical restrictions, so if your project involves
#   any of the following you should contact us and we will try to work something
#   out:
#    - DRM or Copy Protection of any kind(except Copyrights)
#    - Software Patents You Do Not Wish to Freely License
#    - Any Kind of Linking to Non-GPL licensed Works
#    - Are Currently In Violation of Another Copyright Holder's GPL License
#    - If You want to change our code and not add a few hundred MB of stuff to
#        your distribution
#
#   These and other limitations could cause serious legal problems if you ignore
#   them, so it is best to simply contact us or the Free Software Foundation, if
#   you have any questions.
#
#   Joseph Toppi - toppij@gmail.com
#   John Blackwood - makoenergy02@gmail.com

cmake_minimum_required (VERSION 2.8)
project (EngineDemo)

message ("${PROJECT_NAME} - Begin")

##############################################################################
#EngineDemo initializations

#add_definitions( ${CommonBuildFlags} )
#target_compile_options( ${PROJECT_NAME} PRIVATE ${CommonBuildFlags} )

set (${PROJECT_NAME}_DATA_DIR "${${PROJECT_NAME}_SOURCE_DIR}/data/")
message ("Base Directory for data input: ${${PROJECT_NAME}_DATA_DIR}")

set (${PROJECT_NAME}_SOURCE_DIR "${${PROJECT_NAME}_SOURCE_DIR}/src/")
message ("Looking for source code in: ${${PROJECT_NAME}_SOURCE_DIR}")

set (${PROJECT_NAME}_MEZZANINE_DIR "${ProjectRootDir}/Mezzanine/")
message ("Base for Mezzanine: ${${PROJECT_NAME}_MEZZANINE_DIR}")

#this next line should work, but I guess CMake applies somekind of scope to variables
#set (${PROJECT_NAME}_MEZZANINE_BIN_DIR "${Mezzanine_BINARY_DIR}")
set (${PROJECT_NAME}_MEZZANINE_BIN_DIR "${Mezzanine_BINARY_DIR}/bin/")
message ("Mezzanine Binaries: ${${PROJECT_NAME}_MEZZANINE_BIN_DIR}")

set (${PROJECT_NAME}_MEZZANINE_BIN_SRC "${${PROJECT_NAME}_MEZZANINE_DIR}src/")
message ("Mezzanine source and headers: ${${PROJECT_NAME}_MEZZANINE_BIN_SRC}")

set (${PROJECT_NAME}_BINARY_DIR "${${PROJECT_NAME}_BINARY_DIR}/bin/")
set (EXECUTABLE_OUTPUT_PATH "${${PROJECT_NAME}_BINARY_DIR}")
message ("Base Directory for Executable binaries and data output: ${${PROJECT_NAME}_BINARY_DIR}")

##############################################################################
#A complete list of our source files and headers
set(${PROJECT_NAME}_sources
    "${${PROJECT_NAME}_SOURCE_DIR}gamebase.h"
    "${${PROJECT_NAME}_SOURCE_DIR}gamebase.cpp"
)

##############################################################################
# Functions to use to copy files at build time

#Description: This function copies items from the Data/lib directory into it's expected location in the output directory During build time. It does not need leading slashes
#PreCondition: This function can only be called after "add_library(${PROJECT_NAME} ...)" is called, otherwise it will fail silently
function(CopyFilesDuringBuild SourceFileInDataTree TargetFileInBinaryTree)
    add_custom_command(
        TARGET ${PROJECT_NAME}
        PRE_LINK
        COMMAND ${CMAKE_COMMAND} -E echo "Preparing File: ${${PROJECT_NAME}_DATA_DIR}${TargetFileInBinaryTree}"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${${PROJECT_NAME}_DATA_DIR}${SourceFileInDataTree}" "$<TARGET_FILE_DIR:${PROJECT_NAME}>/${TargetFileInBinaryTree}"
        VERBATIM
    )
endfunction(CopyFilesDuringBuild)

#Description: This function copies items from the output directory in Mezzanine into the location of the Catch executable. 
#PreCondition: This function can only be called after "add_library(${PROJECT_NAME} ...)" is called, otherwise it will fail silently
function(CopyLibraryFromMezzanineDuringBuild TargetName)
    add_custom_command(
        TARGET ${PROJECT_NAME}
        PRE_LINK
        COMMAND ${CMAKE_COMMAND} -E echo "Preparing File: $<TARGET_FILE:${TargetName}>"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "$<TARGET_FILE:${TargetName}>" "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
        VERBATIM
    )
endfunction(CopyLibraryFromMezzanineDuringBuild)

#Description: This function copies license files in Mezzanine into the Catch License folder. 
#PreCondition: This function can only be called after "add_library(${PROJECT_NAME} ...)" is called, otherwise it will fail silently
function(CopyLicenseFromMezzanineDuringBuild TargetPath DestName)
    add_custom_command(
        TARGET ${PROJECT_NAME}
        PRE_LINK
        COMMAND ${CMAKE_COMMAND} -E echo "Preparing File: ${TargetPath}"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${${PROJECT_NAME}_MEZZANINE_BIN_DIR}${TargetPath}" "$<TARGET_FILE_DIR:${PROJECT_NAME}>/data/common/${DestName}"
        VERBATIM
    )
endfunction(CopyLicenseFromMezzanineDuringBuild)


##############################################################################
#define the executable and say it requires Mezzanine
include_directories( ${${PROJECT_NAME}_SOURCE_DIR} ${${PROJECT_NAME}_MEZZANINE_BIN_SRC} ${ProjectIncludeDir} )
link_directories( ${${PROJECT_NAME}_MEZZANINE_BIN_DIR} )

add_executable(${PROJECT_NAME} ${${PROJECT_NAME}_sources} )
add_dependencies( ${PROJECT_NAME} Mezzanine )
target_link_libraries( ${PROJECT_NAME} ${MezzanineLinkName} ${OgreLinkName} )

target_compile_options( ${PROJECT_NAME} PRIVATE ${CommonBuildFlags} )

##############################################################################
# File copies during all builds
message ("${PROJECT_NAME} - Begin File Copy Process")
CopyFilesDuringBuild( common/r2skin.jpg data/common/r2skin.jpg )
CopyFilesDuringBuild( common/flare.png data/common/flare.png )
CopyFilesDuringBuild( common/wood-texture.jpg data/common/wood-texture.jpg )
CopyFilesDuringBuild( common/Sphere_Metal.psd data/common/Sphere_Metal.psd )
CopyFilesDuringBuild( common/EngineDemo_Menu.png data/common/EngineDemo_Menu.png )
CopyFilesDuringBuild( common/Plane_UVW_Text.tga data/common/Plane_UVW_Text.tga )
CopyFilesDuringBuild( common/Plane.mesh data/common/Plane.mesh )
CopyFilesDuringBuild( common/robot.mesh data/common/robot.mesh )
CopyFilesDuringBuild( common/Sphere_Metal.mesh data/common/Sphere_Metal.mesh )
CopyFilesDuringBuild( common/Sphere_Wood.mesh data/common/Sphere_Wood.mesh )
CopyFilesDuringBuild( common/EngineDemo_Menu.mta data/common/EngineDemo_Menu.mta )
CopyFilesDuringBuild( common/robot.skeleton data/common/robot.skeleton )
CopyFilesDuringBuild( common/Sphere_Wood.material data/common/Sphere_Wood.material )
CopyFilesDuringBuild( common/Examples.material data/common/Examples.material )
CopyFilesDuringBuild( common/Plane.material data/common/Plane.material )
CopyFilesDuringBuild( common/Sphere_Metal.material data/common/Sphere_Metal.material )
CopyFilesDuringBuild( common/RobotDecomp3.bullet data/common/RobotDecomp3.bullet )
CopyFilesDuringBuild( common/Examples.particle data/common/Examples.particle )
CopyFilesDuringBuild( common/test.xml data/common/test.xml )
CopyFilesDuringBuild( common/test.zip data/common/test.zip )
CopyFilesDuringBuild( common/music/cAudioTheme1.ogg data/common/music/cAudioTheme1.ogg )
CopyFilesDuringBuild( common/music/cAudioTheme2.ogg data/common/music/cAudioTheme2.ogg )
CopyFilesDuringBuild( common/sounds/welcomefun-1.ogg data/common/sounds/welcomefun-1.ogg )
CopyFilesDuringBuild( common/sounds/welcomefun-1-edit.ogg data/common/sounds/welcomefun-1-edit.ogg )

CopyLicenseFromMezzanineDuringBuild( data/common/BulletZLibLicense.txt BulletZLibLicense.txt )
CopyLicenseFromMezzanineDuringBuild( data/common/gpl-2.0.txt gpl-2.0.txt )
CopyLicenseFromMezzanineDuringBuild( data/common/gpl-3.0.txt gpl-3.0.txt)
CopyLicenseFromMezzanineDuringBuild( data/common/lgpl-2.1.txt lgpl-2.1.tx)
CopyLicenseFromMezzanineDuringBuild( data/common/lgpl-3.0.txt lgpl-3.0.tx)
CopyLicenseFromMezzanineDuringBuild( data/common/Licenses.txt Licenses.txt)
CopyLicenseFromMezzanineDuringBuild( data/common/Lua-MIT.txt Lua-MIT.txt)
CopyLicenseFromMezzanineDuringBuild( data/common/Ogre-MITLicense.txt Ogre-MITLicense.txt )
CopyLicenseFromMezzanineDuringBuild( data/common/PugiXML-MITLicense.txt PugiXML-MITLicense.txt )
CopyLicenseFromMezzanineDuringBuild( data/common/ReneNyffenegger-MIT.txt ReneNyffenegger-MIT.txt )
CopyLicenseFromMezzanineDuringBuild( data/common/SDL-MITLicense.txt SDL-MITLicense.txt )
CopyLicenseFromMezzanineDuringBuild( data/common/XiphCopyrightNotice.txt XiphCopyrightNotice.txt )
CopyLicenseFromMezzanineDuringBuild( data/common/zlib-libpngLicense.txt zlib-libpngLicense.txt )
CopyLicenseFromMezzanineDuringBuild( data/common/zlib-zliblicense.txt zlib-zliblicense.txt )

CopyLibraryFromMezzanineDuringBuild( "Mezzanine" )

##############################################################################
#Copy System Dependant files during build time

# Windows File Copy Specific Items
if(${WIN32})
    message ("${PROJECT_NAME} - Begin Windows32/64 Specific File Copy Process")
    message ("${PROJECT_NAME} - End Windows32/64 Specific File Copy Process")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -mwindows" )
endif(${WIN32})

# Linux File Copy Specific Items
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    message ("${PROJECT_NAME} - Begin Linux Specific File Copy Process")
    set(GLXIcon data/linux/GLX_icon.png )
    add_custom_command(
        TARGET ${PROJECT_NAME}
        PRE_LINK
        COMMAND ${CMAKE_COMMAND} -E echo "Preparing File: ${GLXIcon}"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${${PROJECT_NAME}_MEZZANINE_BIN_DIR}${GLXIcon}" "$<TARGET_FILE_DIR:${PROJECT_NAME}>/${DestName}"
        VERBATIM
    )
    message ("${PROJECT_NAME} - End Linux Specific File Copy Process")
endif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")

# Mac File Copy Specific Items
if(${APPLE})
    message ("${PROJECT_NAME} - Begin Apple Specific File Copy Process")
    message ("${PROJECT_NAME} - End Apple Specific File Copy Process")
endif(${APPLE})

message ("${PROJECT_NAME} - End Copy Process")

message ("${PROJECT_NAME} - End")
