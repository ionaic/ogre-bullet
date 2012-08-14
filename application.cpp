#include "application.h"
#include <OgreConfigFile.h>
#include <OgreException.h>
#include <OgreCamera.h>
#include <OgreViewport.h>
#include <OgreSceneManager.h>
#include <OgreRenderWindow.h>
extern Application *MainApplication;
/*-------------------------------------------------------------------------------------
	Constructors and Destructors
-------------------------------------------------------------------------------------*/
Application::Application(void) {
	// flags
	mShutDown = false;
	mUseCameraMan = true;
	mUseStats = true;
	mStatState = STAT_LITTLE;
	// core shit
	mRoot = 0;
	mCameraMan = 0;
	mTrayMan = 0;
	// plugins
	OpenGLRenderer = OGRE_NEW Ogre::GLPlugin();
	CgProgramManager = OGRE_NEW Ogre::CgPlugin();
	// strings
	windowTitle = "Application";
	resourcesFile = "resources.cfg";
}
Application::~Application(void) {
	Ogre::WindowEventUtilities::removeWindowEventListener(mWindow, this);
	windowClosed(mWindow);
	if (mTrayMan) delete mTrayMan;
	if (mCameraMan) delete mCameraMan;
	if (mRoot) delete mRoot;
	OGRE_DELETE OpenGLRenderer;
	OGRE_DELETE CgProgramManager;
}
/*-------------------------------------------------------------------------------------
	Window Callbacks - implements Ogre::WindowEventListener
-------------------------------------------------------------------------------------*/
//Adjust mouse clipping area
void Application::windowResized(Ogre::RenderWindow* rw) {
    unsigned int width, height, depth;
    int left, top;
    rw->getMetrics(width, height, depth, left, top);
 
    const OIS::MouseState &ms = mMouse->getMouseState();
    ms.width = width;
    ms.height = height;
	_windowResized(rw);
}
 
//Unattach OIS before window shutdown (very important under Linux)
void Application::windowClosed(Ogre::RenderWindow* rw) {
    //Only close for window that created OIS (the main window in these demos)
    if( rw == mWindow ) {
        if( mInputManager ) {
            mInputManager->destroyInputObject( mMouse );
            mInputManager->destroyInputObject( mKeyboard );
 
            OIS::InputManager::destroyInputSystem(mInputManager);
            mInputManager = 0;
        }
    }
	_windowClosed(rw);
}
/*-------------------------------------------------------------------------------------
	Frame Callback - implements Ogre::FrameListener
-------------------------------------------------------------------------------------*/
bool Application::frameRenderingQueued(const Ogre::FrameEvent& evt) {
    if(mWindow->isClosed() || mShutDown)
        return false;
 
    //Need to capture/update each device
    mKeyboard->capture();
    mMouse->capture();
	
	if (mUseStats) mTrayMan->frameRenderingQueued(evt);
	
    return _frameRenderingQueued(evt);
}
/*-------------------------------------------------------------------------------------
	Window Initialization
-------------------------------------------------------------------------------------*/
bool Application::go(void) {
	initOgreRoot();
	initResources();
	if (!initWindow()) return false;
	
	// Set default mipmap level (note: some APIs ignore this)
	Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);
	// initialise all resource groups
	Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
	
	// Create the SceneManager, in this case a generic one
	mSceneMgr = mRoot->createSceneManager("DefaultSceneManager");
	
	createCamera();
	
	// Create one viewport, entire window
	mViewport = mWindow->addViewport(mCamera);
	mViewport->setBackgroundColour(Ogre::ColourValue(0,0,0));
	 
	// Alter the camera aspect ratio to match the viewport
	mCamera->setAspectRatio(Ogre::Real(mViewport->getActualWidth()) / Ogre::Real(mViewport->getActualHeight()));
	
	initScene();
	
	if (!initOIS()) return false;
	
	//Set initial mouse clipping size
	windowResized(mWindow);
	 
	//Register as a Window listener
	Ogre::WindowEventUtilities::addWindowEventListener(mWindow, this);
	
	mRoot->addFrameListener(this);
	mTrayMan = new OgreBites::SdkTrayManager("TrayMgr", mWindow, mMouse, this);
	mTrayMan->showFrameStats(OgreBites::TL_BOTTOMLEFT);
	mTrayMan->toggleAdvancedFrameStats();
	//mTrayMan->showLogo(OgreBites::TL_BOTTOMRIGHT);
	mTrayMan->hideCursor();
	
	mRoot->startRendering();
    return true;
}
void Application::initOgreRoot() {
    // construct Ogre::Root
    mRoot = new Ogre::Root("");
	// register plugins
	mRoot->getSingleton().installPlugin(OpenGLRenderer);
	mRoot->getSingleton().installPlugin(CgProgramManager);
}
void Application::initResources() {
	// Load resource paths from config file
	Ogre::ConfigFile cf;
	cf.load(resourcesFile);
	// Go through all sections & settings in the file
	Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();
	
	Ogre::String secName, typeName, archName;
	while (seci.hasMoreElements()) {
		secName = seci.peekNextKey();
		Ogre::ConfigFile::SettingsMultiMap *settings = seci.getNext();
		Ogre::ConfigFile::SettingsMultiMap::iterator i;
		for (i = settings->begin(); i != settings->end(); ++i) {
			typeName = i->first;
			archName = i->second;
			Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
				archName, typeName, secName);
		}
	}
}
bool Application::initWindow() {
	// configure
	// Show the configuration dialog and initialise the system
	if(!(mRoot->restoreConfig() || mRoot->showConfigDialog())) {
		return false;
	}
	
	mWindow = mRoot->initialise(true, windowTitle);
	return true;
}
bool Application::initOIS() {
	Ogre::LogManager::getSingletonPtr()->logMessage("*** Initializing OIS ***");
	OIS::ParamList pl;
	size_t windowHnd = 0;
	std::ostringstream windowHndStr;
	// get the window handle
	mWindow->getCustomAttribute("WINDOW", &windowHnd);
	windowHndStr << windowHnd;
	pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));
	mInputManager = OIS::InputManager::createInputSystem( pl );
	// create the keyboard and mouse interfaces and register this as the callback
	if (!mInputManager) return false;
	mKeyboard = static_cast<OIS::Keyboard*>(mInputManager->createInputObject( OIS::OISKeyboard, true ));
	mMouse = static_cast<OIS::Mouse*>(mInputManager->createInputObject( OIS::OISMouse, true ));
	
	mMouse->setEventCallback(this);
    mKeyboard->setEventCallback(this);
	
	return true;
}
/*                        ,     \    /      ,
                         / \    )\__/(     / \
                        /   \  (_\  /_)   /   \
     __________________/_____\__\@  @/___/_____\_________________
     |                          |\../|                          |
     |                           \VV/                           |
     |                           MAIN                           |
     |                                                          |
     |                     Here be dragons.                     |
     |           Abandon all hope, yee who enter here           |
     |__________________________________________________________|
                   |    /\ /      \\       \ /\    |
                   |  /   V        ))       V   \  |
                   |/     `       //        '     \|
                   `              V                ' 
*/
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#endif
 
#ifdef __cplusplus
extern "C" {
#endif
 
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
    INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT )
#else
    int main(int argc, char *argv[])
#endif
    {
        try {
            MainApplication->go();
        } catch( Ogre::Exception& e ) {
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
            MessageBox( NULL, e.getFullDescription().c_str(), "An exception has occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
            std::cerr << "An exception has occured: " <<
                e.getFullDescription().c_str() << std::endl;
#endif
        }
 
        return 0;
    }
 
#ifdef __cplusplus
}
#endif
