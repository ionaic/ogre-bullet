#ifndef __MAF_OGRE_H__
#define __MAF_OGRE_H__

#include <OgreRoot.h>
#include <OgreWindowEventUtilities.h>
// plugins
#include <RenderSystems/GL/OgreGLPlugin.h>
#include <Plugins/CgProgramManager/OgreCgPlugin.h>
// OIS
#include <OISEvents.h>
#include <OISInputManager.h>
#include <OISKeyboard.h>
#include <OISMouse.h>
// default camera manager
#include <SdkCameraMan.h>
#include <SdkTrays.h>

class Application: public Ogre::WindowEventListener, public Ogre::FrameListener, public OIS::KeyListener, public OIS::MouseListener, OgreBites::SdkTrayListener {
public:
	Application(void);
	virtual ~Application(void);
	bool go(void);
protected:
	// Flags
	bool mShutDown, mUseCameraMan, mUseStats;
	enum {
		STAT_LITTLE,
		STAT_BIG,
		STAT_NONE
	} mStatState;
	// Core Objects
	Ogre::Root* mRoot;
	Ogre::RenderWindow* mWindow;
	Ogre::SceneManager* mSceneMgr;
	Ogre::Camera* mCamera;
	Ogre::Viewport *mViewport;
	// Names to possibly overwrite
	Ogre::String windowTitle;
	Ogre::String resourcesFile;
	// OIS Input devices
	OIS::InputManager* mInputManager;
	OIS::Mouse*    mMouse;
	OIS::Keyboard* mKeyboard;
	// default camera controller
    OgreBites::SdkCameraMan* mCameraMan;
	OgreBites::SdkTrayManager* mTrayMan;

	virtual void initScene();
	virtual void createCamera();
	
	// Ogre::WindowEventListener Proxies
	virtual void _windowResized(Ogre::RenderWindow*);
	virtual void _windowClosed(Ogre::RenderWindow*);
	// Ogre::FrameListener Proxies
	virtual bool _frameRenderingQueued(const Ogre::FrameEvent&);
	
    // OIS::KeyListener
    virtual bool keyPressed( const OIS::KeyEvent &);
    virtual bool keyReleased( const OIS::KeyEvent &);
    // OIS::MouseListener
    virtual bool mouseMoved( const OIS::MouseEvent &);
    virtual bool mousePressed( const OIS::MouseEvent &, OIS::MouseButtonID);
    virtual bool mouseReleased( const OIS::MouseEvent &, OIS::MouseButtonID);
private:
	// plugins
	Ogre::Plugin *OpenGLRenderer;
	Ogre::Plugin *CgProgramManager;
	// helpers
	void initOgreRoot();
	void initResources();
	bool initWindow();
	bool initOIS();
	// Ogre::WindowEventListener
	virtual void windowResized(Ogre::RenderWindow* rw);
	virtual void windowClosed(Ogre::RenderWindow* rw);
	// Ogre::FrameListener
	virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt);
};

#endif
