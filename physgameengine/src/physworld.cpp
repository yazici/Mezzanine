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
#ifndef _physworld_cpp
#define _physworld_cpp
///////////////////////////////////////////////////////////////////////////////
//The world that integrates everything
//
//This is where all the calls to the the underlying libraries should be from.
//PhysWorld is an abstraction layer to all of the non-ui libraries. The ui
//layers wx and ogre both have their own main loops, so it is almost mandatory
//for other code to interact with those libraries directly.
///////////////////////////////////////////////////////////////////////////////
//Includes
#include "physdatatypes.h"
#include "physevent.h"
#include "physworld.h"
#include "physvector.h"
#include "physcrossplatform.h"
#include "physworldcallbackmanager.h"
#include "physgamesettings.h"
#include "physmisc.h"
#include "physactor.h"
#include "physeventuserinput.h"

#include "SDL.h"
#include "btBulletDynamicsCommon.h"
#include "BulletSoftBody/btSoftRigidDynamicsWorld.h"
#include <Ogre.h>

#include <sstream>
#include <string>
using namespace std;

///////////////////////////////////////////////////////////////////////////////
// Physworld constructor
//this should create the basic objects for stroing and tracking the roots of
//objects in the game
/// @todo TODO Fix the `const`ness of all methods to be as const as allowable
PhysWorld::PhysWorld()
{
	this->Construct(
		new PhysVector3(-100.0,-100.0,-100.0),
		new PhysVector3(100.0, 100.0, 100.0),
		10
		);
}

PhysWorld::PhysWorld(PhysVector3* GeographyLowerBounds_, PhysVector3* GeographyUpperbounds_, unsigned short int  MaxPhysicsProxies_)
{
	this->Construct(GeographyLowerBounds_, GeographyUpperbounds_, MaxPhysicsProxies_);
}

void PhysWorld::Construct(PhysVector3* GeographyLowerBounds_, PhysVector3* GeographyUpperbounds_, unsigned short int  MaxPhysicsProxies_)
{
	PlayerSettings = new Settings();

	//We create our Ogre environment
	this->OgreRoot = new Ogre::Root(GetPluginsDotCFG(),GetSettingsDotCFG(),"Physgame.log");

	//Ogre resource related code
	/// @todo : From here to the logger tests should probably be moved to another file
    this->OgreRoot->addResourceLocation(GetDataDirectory(), "FileSystem", "Robot", false);
    this->OgreResource = Ogre::ResourceGroupManager::getSingletonPtr();

    /// @todo TODO remove Robot test code
    this->OgreResource->declareResource("robot.mesh", "Mesh", "Robot");
    this->OgreResource->declareResource("Examples.material", "Material", "Robot");

    //Perform a Test that only needs to be done once for the SDL/Physuserinputevent system.
    Log("Verifying size of userinput events:");
    Log(sizeof(MetaCode::InputCode));
    Log(sizeof(SDLKey));
    if(sizeof(MetaCode::InputCode) != sizeof(SDLKey))
        {LogAndThrow("User input subsystem Event Sizes  Don't match, userinput subsystem will go down faster than 'that' girl on prom night.");}
    Log("They match, the User Input subsystem won't crash instantly");

    // This Tests the Logger and Logs a few critical Items.
    TestLogger();


    //Callbacks are the main way that a game using the PhysWorld will be able to have their code run at custom times
	this->CallBacks = new PhysWorldCallBackManager(this);

    //Events are the main way for the game using the physworld to  get information about the various subsystems
    this->Events = new PhysEventManager;

	//instantiate the Physics engine and related items
	GeographyLowerBounds = GeographyLowerBounds_;
	GeographyUpperbounds = GeographyUpperbounds_;
	MaxPhysicsProxies = MaxPhysicsProxies_;

	btVector3 worldAabbMin(GeographyLowerBounds->X, GeographyLowerBounds->Y, GeographyLowerBounds->Z);
	btVector3 worldAabbMax(GeographyUpperbounds->X, GeographyUpperbounds->Y, GeographyUpperbounds->Z);

	this->BulletBroadphase = new btAxisSweep3(worldAabbMin, worldAabbMax, MaxPhysicsProxies);
	this->BulletSolver = new btSequentialImpulseConstraintSolver;
	this->BulletCollisionConfiguration = new btDefaultCollisionConfiguration();
	this->BulletDispatcher = new btCollisionDispatcher(BulletCollisionConfiguration);

	this->BulletDynamicsWorld = new btSoftRigidDynamicsWorld(
												BulletDispatcher,
												BulletBroadphase,
												BulletSolver,
												BulletCollisionConfiguration);
}

void PhysWorld::TestLogger()
{
    string temp0("0");
    char temp1 = 'a';
    short int temp2 = 2;
    int temp3 = 3;
    long int temp4 = 4;
    unsigned short int temp5 = 5;
    unsigned int temp6 = 6;
    unsigned long int temp7 = 7;
    bool temp8 = true;
    float temp9 = 0.9;
    double temp10 = 9.9999;
    long double temp11 = 11.00011;
    wchar_t temp12 = 'L';
    size_t temp13 = 13;
    PhysReal temp14 = 1.4;
    PhysWhole temp15 = 15;
    PhysString temp16("16 or so");
    PhysVector3 temp17(0,1,7);
    RawEvent temp18;
        temp18.type = SDL_KEYDOWN;
        temp18.key.keysym.sym = SDLK_BACKSPACE;
    MetaCode temp19(temp18);
    //dynamic_cast<PhysEvent*>// Add physevent as something that can be logged.
    /// @todo TODO add each type of event here (logtest) to make it really wasy to log events

    OneLogTest(temp0, "string");
    OneLogTest(temp1, "char");
    OneLogTest(temp2, "short int");
    OneLogTest(temp3, "int");
    OneLogTest(temp4, "long int");
    OneLogTest(temp5, "unsigned short int");
    OneLogTest(temp6, "unsigned int");
    OneLogTest(temp7, "unsigned long int");
    OneLogTest(temp8, "bool");
    OneLogTest(temp9, "float");
    OneLogTest(temp10, "double");
    OneLogTest(temp11, "long double");
    OneLogTest(temp12, "wchar_t");
    OneLogTest(temp13, "size_t");
    OneLogTest(temp14, "PhysReal");
    OneLogTest(temp15, "PhysWhole");
    OneLogTest(temp16, "PhysString");
    OneLogTest(temp17, "PhysVector3");
    OneLogTest(temp18, "RawEvent"); /// @todo TODO Figure out How does this calle= the same streaming function as MetaCode ?!?!?
    OneLogTest(temp19, "MetaCode");
}

template <class T> void PhysWorld::OneLogTest(T Data, string DataType, string Message1, string Message2)
{
    try
    {
        Log(Message1+DataType);
        LogAndThrow(Data);
    } catch (T excepted) {
        Log(Message2+DataType);
        Log(excepted);
    }
}
///////////////////////////////////////////////////////////////////////////////
//tears the world down
PhysWorld::~PhysWorld()
{
	//Destroy the physical world that we loved and cherished
	//dWorldDestroy(this->OdeWorld);
	delete GeographyLowerBounds;
	delete GeographyUpperbounds;
	delete BulletBroadphase;
	delete BulletCollisionConfiguration;
	delete BulletDispatcher;
	delete BulletSolver;
	delete BulletDynamicsWorld;

	//All the pointers Ogre made should get taken care of by OGRE
	delete OgreRoot;

	//clear up our objects
	delete CallBacks;
	delete Events;

	delete PlayerSettings;

	//remove sdl stuff

	SDL_FreeSurface(SDLscreen);
	void SDL_Quit(void);
}

///////////////////////////////////////////////////////////////////////////////
//appends to the gamelog which is managed by Ogre
template <class T> void PhysWorld::Log(T Message)
{
	stringstream temp;
	temp << Message;
	Ogre::LogManager::getSingleton().logMessage(temp.str());
}

template <class T> void PhysWorld::LogAndThrow(T Message)
{
	this->Log(Message);
	throw(Message);
}

///////////////////////////////////////////////////////////////////////////////
//Shows the ogre settings Dialog, and allows it to save settings to ogres
//preset save location
bool PhysWorld::ShowSystemSettingDialog()
{
	try
	{
		return this->OgreRoot->showConfigDialog();
	} catch (exception& e) {
		this->Log("Ogre settings windows from main UI or mandatory setting failure");
		this->Log(e.what());
		return false;
	}
}

///////////////////////////////////////////////////////////////////////////////
// Start the Game already
void PhysWorld::GameInit()
{
	//try to load the ogre config
	this->LoadOgreSettings();
	this->CreateRenderWindow();

    //Create a the RenderTimer, which will be used to measure the time
    Ogre::Timer RenderTimer;

	bool Callbackbools[] = {true, true, true, true, true, true};

	//Used for tracking times to prevent Infinite render loops in graphically simple games
	//PhysWhole Times[] = {0,0,0,0};

	/// @todo TODO finish test code, there is a bunch of sloppy test code for the robot in the gameinit
	this->OgreResource->initialiseResourceGroup("Robot");
	this->OgreSceneManager->setAmbientLight( Ogre::ColourValue( 1, 1, 1 ) );
	Ogre::Entity *ent1 = this->OgreSceneManager->createEntity( "Robot", "robot.mesh" );
	Ogre::SceneNode *node1 = this->OgreSceneManager->getRootSceneNode()->createChildSceneNode( "RobotNode" );
	node1->attachObject( ent1 );

    PhysWhole FrameDelay = 0;
    PhysWhole FrameTime = 0;

    /*! @page mainloop1 Main Loop Structure and Flow
     The MainLoop is heart of most vidoe games and simulations.

     @section overview1 Main loop Overview
	 The Main loop runs in @ref PhysWorld.GameInit(). By default this Method also starts the render, the physics andthe input systems. It does very little
	 on it's own. It uses the default callback manager (which you can swap out if you want), which is the callback manager point to by PhysWorld::CallBacks . Once started it runs the
	 callbacks present in the callback manager until one of them returns false. It has a pretty specific work flow. Starting with input, then physics, then rendering. There is a callback
	 before and after each of these tasks.
     \n \n The user input task is never started if there is no Pre or Post callback set. It won't crash or throw any weird error condtions, it was specifically designed this way. This
     allows the input buffering to be turned off by simply removing the callbacks. This might not sound like much, but if you simply don't need input this can be a little performance bump.
     It sounds even better when we say it works the same way for physics. If you do not have a pre or post physic callback set physics simply is not run. This allows for easy control over
     the state of motion  in a world. Currently Rendering is performed each iteration of the main loop, regardless of what Callbacks are set. This may be changed at some point in the future,
     but no immediate plans to change it are in the works.
     \n \n One iteration of the main loops is about 1/60 of a second and will render 1 frame and step physics about 1/60 (1/62.5 usually) of a second. Currently there is no way to uncap
     the framerate, we did not see a need for more (we may change this in the future). Situations with low performance should automatically be handled but increasing the size of physics
     steps and automatically not rendering frames when not appropriate. There will be fewer calls on callbacks, but the engine will not inhibit gameplay below the the target framerate.
    \n
     @section callbacks1 1) Input Callbacks and Buffering
     User input is the first task performed each main loop. This was done because in most games and simulations everything depends on the user input.
     @subsection input1 1.A) PreInputCallback
     This can be set using the function in PhysWorldCallBackManager that have "PreInput" in their names. This is a great place to to do begining of loop items, like gathering data for
     computer controlled characters, checking if game goals are met, and other items that needs to be done it iteration of the game loop and don't directly relate to physics or Rendering.
     @subsection input2 1.B) Input Buffering
     During this step Input events are gathered fromt the input Subsystem (which could be any of a number of user input libraries), which are ultimately made by the Operating System and
     processed into PhysEventUserInput objects, and placed in the Default event manager ( check the pointer PhysWorld::Events ).
     @subsection input3 1.C) PostInputCallback
     This callback is your first chance to access the userinput Events and try to use them. This callback can be managed by using methods in PhysWorldCallBackManager that have "PostInput"
     in their names. The functionality of this Callback slightly overlaps with the Prephysics Callbacks due to the timing of using them both. If both are used, one will be called
     immediately after the other.
    \n
     @section callbacks2 2) Physics callbacks and Event Buffering
     This step runs between user input and rendering to allow for change to be immediately visible to the user.
     @subsection physics1 2.A) PrePhysicsCallback
     The funtionality of this callback overlaps a great deal with the PostInputCallBack, but they both still exist to make organization easier when needed. You probably should have figured
     out by now how the Callbackmanager functions are named, the ones for this are ituitively named with "PrePhysics" in their name.
     @subsection physics2 2.B) Physics calculations and Event Buffering
     Physics will be run here, but currently is not fully implemented. But eventually physics event will be generated here. Including a system for checking for certain types of collisions.
     @subsection physics3 2.C) PostPhysicsCallback
     This Callback is you first chance to work with the physics events just generated. This is a good place to check if you want items to adjust hitpoints, explode, die or take some other
     action based on physics.
    \n
     @section callbacks3 3) Rendering
     asdf
     @subsection rendering1 3.A)
     asdf
     @subsection rendering2 3.B)
     asdf
     @subsection rendering3 3.C)
     fdgsdfs
    \n
     @section endingmainloop1 Ending the Main Loop
     ffgdfdfg
	*/
	//As long as all the CallBacks return true the game continues
	while (Callbackbools[0] && Callbackbools[1] && Callbackbools[2] && Callbackbools[3] && Callbackbools[4] && Callbackbools[5])
	{
        //Input buffering Callbacks
        if( this->CallBacks->IsPreInputCallbackSet() || this->CallBacks->IsPostInputCallbackSet() )
        {
     		if( this->CallBacks->IsPreInputCallbackSet() )
                { Callbackbools[0] = this->CallBacks->PreInput(); }
			this->DoMainLoopInputBuffering();
			if( this->CallBacks->IsPostInputCallbackSet() )
                { Callbackbools[1] = this->CallBacks->PostInput(); }
        }
 /// @todo actually document the gameloop
		if( this->CallBacks->IsPrePhysicsCallbackSet() || this->CallBacks->IsPostPhysicsCallbackSet() )
        {
            //this->Events->AddEvent(new PhysEventRenderTime(FrameTime));
            if( this->CallBacks->IsPrePhysicsCallbackSet() )
                { Callbackbools[2] = this->CallBacks->PrePhysics(); }
			this->DoMainLoopPhysics();
            if( this->CallBacks->IsPostPhysicsCallbackSet() )
                { Callbackbools[3] = this->CallBacks->PostPhysics(); }
        }

		if(this->CallBacks->IsPreRenderCallbackSet())
        {
            //this->Events->AddEvent(new PhysEventRenderTime(FrameTime));
			Callbackbools[4] = this->CallBacks->PreRender();
			this->DoMainLoopWindowManagerBuffering();
        }

		//Render the frame and figure the amount of time it took //Limit frame rate to 62.5
		this->DoMainLoopRender();
		FrameTime = RenderTimer.getMilliseconds();
		RenderTimer.reset();
		if(16>FrameTime){               		//use 16666 for microseconds
		    FrameDelay++;
			WaitMilliseconds( FrameDelay );
		}else if(16==FrameTime){
        }else{
            if (0<FrameDelay){
                FrameDelay--;
            }else{
                FrameDelay=0;
            }
        }

		if(this->CallBacks->IsPostRenderCallbackSet())
        {
			this->Events->AddEvent(new PhysEventRenderTime(FrameTime));
			Callbackbools[5] = this->CallBacks->PostRender();
        }

	}//End of main loop

	//Some after loop cleanup
	this->DestroyRenderWindow();
}

void PhysWorld::MoveCamera(PhysVector3 Position, PhysVector3 LookAt)
{
	this->OgreCamera->setPosition(Ogre::Vector3(Position.X,Position.Y,Position.Z));
    this->OgreCamera->lookAt(Ogre::Vector3(LookAt.X,LookAt.Y,LookAt.Z));
}

void PhysWorld::DoMainLoopAllItems()
{
	this->DoMainLoopPhysics();
	this->DoMainLoopWindowManagerBuffering();
	this->DoMainLoopInputBuffering();
	this->DoMainLoopRender();
}

void PhysWorld::DoMainLoopPhysics()
{
	//TODO: Step the physics world here per main loop items
	//this->Events->AddsomeEvents
}

void PhysWorld::DoMainLoopWindowManagerBuffering()
{
    this->PreProcessSDLEvents();
    Log("WM EventCount Pending:");
    Log(SDL_WmEvents.size());
    //TODO: make Physevents for each of the events in SDL_WmEvents(and delete the SDL events)
}

void PhysWorld::DoMainLoopInputBuffering()
{
    this->PreProcessSDLEvents();
    Log("User Input EventCount Pending:");
    Log(SDL_UserInputEvents.size());

    PhysEventUserInput* FromSDLEvent = new PhysEventUserInput();

    /// @todo TODO: make Physevents for each of the events in SDL_WmEvents(and delete the SDL events)
    while( !SDL_UserInputEvents.empty() )
    {
        MetaCode CurrentMetaCode( *(SDL_UserInputEvents.front()) );
        FromSDLEvent->AddCode(CurrentMetaCode);         //This relies on the fact that the constructor copies the data
        SDL_UserInputEvents.pop(); //NEXT!!!
    }

    //temp code
    /// @todo TODO Delete the even temp logging code
    for(unsigned int c=0; c<FromSDLEvent->GetMetaCodeCount(); c++)
    {
        Log(FromSDLEvent->GetMetaCode(c));
    }
    // /tempcode

    if (0 < FromSDLEvent->GetMetaCodeCount())
    {
        this->Events->AddEvent(FromSDLEvent); //Now FromSDL is some else's responsibility
    }else{
        delete FromSDLEvent;
    }
}

void PhysWorld::DoMainLoopRender()
{
	RenderPhysWorld(this);
}
///////////////////////////////////////////////////////////////////////////////
// Private Functions
///////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//Loads the Ogre Configuration
void PhysWorld::LoadOgreSettings()
{
	//Try loading from the default location
	if (!this->OgreRoot->restoreConfig())
    {
    	//if we can't do that then lets make new settings
    	if (!this->ShowSystemSettingDialog())
    	{
    		this->LogAndThrow("Error 1: Could not setup Ogre.");
		}
    }

}

//Seriously read the Function Name
void PhysWorld::CreateRenderWindow()
{
	//Get what is needed for SDL started
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		this->Log("Error 2: Unable to init SDL, SDL Error Follows:");
		this->LogAndThrow(SDL_GetError());
	}

	//Setup the SDL render window
	this->SDLscreen = SDL_SetVideoMode(PlayerSettings->getRenderHeight(), PlayerSettings->getRenderWidth(), 0, SDL_OPENGL);
    SDL_WM_SetCaption("Catch!", NULL);

	//Start Ogre Without a native render window
	this->OgreGameWindow = this->OgreRoot->initialise(false, "physgame");

	//Configure Ogre to render to the SDL window
	Ogre::NameValuePairList *misc;
	misc=(Ogre::NameValuePairList*) GetSDLOgreBinder();
	(*misc)["title"] = Ogre::String("Catch!");
	this->OgreGameWindow = this->OgreRoot->createRenderWindow("physgame", PlayerSettings->getRenderHeight(), PlayerSettings->getRenderWidth(), PlayerSettings->getFullscreen(), misc);
    //Added following lines to attempt to make the render window visible
    //this->OgreGameWindow->setVisible(true);
	//this->OgreGameWindow->setActive(true);
    //this->OgreGameWindow->setAutoUpdated(true);

	//prepare a scenemanager
	this->OgreSceneManager = this->OgreRoot->createSceneManager(Ogre::ST_GENERIC,"SceneManager");

	//setup a default camera
	this->OgreCamera = this->OgreSceneManager->createCamera("Camera");
	this->OgreCamera->setNearClipDistance(5.0f);
    this->OgreCamera->setFarClipDistance(5000.0f);

	this->MoveCamera(PhysVector3(0.0f,0.0f,500.0f), PhysVector3(0.0f,0.0f,0.0f));

	//viewport connects camera and render window
	this->OgreViewport = this->OgreGameWindow->addViewport(OgreCamera);

	//setting the aspect ratio must be done after we setup the viewport
	this->OgreCamera->setAspectRatio( Ogre::Real(OgreViewport->getActualWidth()) / Ogre::Real(OgreViewport->getActualHeight()) );
}

void PhysWorld::DestroyRenderWindow()
{
    this->OgreGameWindow->destroy();
}

///////////////////////////////////////////////////////////////////////////////
// Deals with SDL

//This function will get all the events from SDL and Sort them into one of two Queues
void PhysWorld::PreProcessSDLEvents()
{
    RawEvent temp;
    RawEvent* FromSDL=&temp;
	while(SDL_PollEvent(FromSDL))
	{
	    RawEvent* ScopeHolder = new RawEvent;
	    *ScopeHolder = temp;
        switch(FromSDL->type)
        {
            case SDL_ACTIVEEVENT:   //when the window gains focus
            case SDL_VIDEORESIZE:   //when the screen is resized
            case SDL_VIDEOEXPOSE:   //when the windows goes from being hidden to being shown
            case SDL_QUIT:          //when SDL closes
            case SDL_SYSWMEVENT:
                SDL_WmEvents.push(ScopeHolder);
                break;
            case SDL_KEYDOWN:
            case SDL_KEYUP:
            case SDL_MOUSEMOTION:
            case SDL_MOUSEBUTTONDOWN:
            case SDL_MOUSEBUTTONUP:
            case SDL_JOYAXISMOTION:
            case SDL_JOYBUTTONDOWN:
            case SDL_JOYBUTTONUP:
            case SDL_JOYBALLMOTION:
            case SDL_JOYHATMOTION:
                SDL_UserInputEvents.push(ScopeHolder);
                break;
            case SDL_USEREVENT://Never thrown by SDL, but could be added by a user
            default:
                throw ("Unknown SDL Event Inserted");
                break;
        }
	}
}

///////////////////////////////////////////////////////////////////////////////
// Bullet Related Public Members
///////////////////////////////////////

void PhysWorld::AddActor(ActorBase* ActorToAdd)
{
    ActorToAdd->AddObjectToWorld(this, this->BulletDynamicsWorld);
}

#endif
