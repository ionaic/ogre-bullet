#include "application.h"
#include <OgreRenderWindow.h>

/*-------------------------------------------------------------------------------------
	Application Initialization Callbacks - Init your application with these
-------------------------------------------------------------------------------------*/
// make your scene
void Application::initScene() {
}
// create a camera
void Application::createCamera() {
	// Create the camera
	mCamera = mSceneMgr->createCamera("PlayerCam");
	
	// Position it at 80 in Z direction
	mCamera->setPosition(Ogre::Vector3(0,0,80));
	// Look back along -Z
	mCamera->lookAt(Ogre::Vector3(0,0,-300));
	mCamera->setNearClipDistance(5);
	
	// create a default camera controller
	if (mUseCameraMan) mCameraMan = new OgreBites::SdkCameraMan(mCamera);
}
/*-------------------------------------------------------------------------------------
	Keyboard Callbacks - implements OIS::KeyListener
-------------------------------------------------------------------------------------*/
bool Application::keyPressed( const OIS::KeyEvent &arg ) {
	// Use the camera manager
	if (mCameraMan && mUseCameraMan) mCameraMan->injectKeyDown(arg);
	// quit if need be
	switch (arg.key) {
		case OIS::KC_ESCAPE:
			mShutDown = true;
			break;
		case OIS::KC_O:
			if (mUseStats) {
				if (mStatState == STAT_LITTLE) {
					mTrayMan->toggleAdvancedFrameStats();
					mStatState = STAT_BIG;
				} else if (mStatState == STAT_BIG) {
					mTrayMan->hideFrameStats();
					mStatState = STAT_NONE;
				} else {
					mTrayMan->showFrameStats(OgreBites::TL_BOTTOMLEFT);
					mTrayMan->toggleAdvancedFrameStats();
					mStatState = STAT_LITTLE;
				}
			}
			break;
		default:
			break;
	};
	return true;
}
bool Application::keyReleased( const OIS::KeyEvent &arg ) {
	// Use the camera manager
	if (mCameraMan && mUseCameraMan) mCameraMan->injectKeyUp(arg);
	return true;
}
/*-------------------------------------------------------------------------------------
	Mouse Callbacks - implements OIS::MouseListener
-------------------------------------------------------------------------------------*/
bool Application::mouseMoved( const OIS::MouseEvent &arg ) {
	// Use the camera manager
	if (mCameraMan && mUseCameraMan) mCameraMan->injectMouseMove(arg);
	return true;
}
bool Application::mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id ) {
	// Use the camera manager
	if (mCameraMan && mUseCameraMan) mCameraMan->injectMouseDown(arg, id);
	return true;
}
bool Application::mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id ) {
	// Use the camera manager
	if (mCameraMan && mUseCameraMan) mCameraMan->injectMouseUp(arg, id);
	return true;
}
/*-------------------------------------------------------------------------------------
	Window Callbacks - proxies Ogre::WindowEventListener
-------------------------------------------------------------------------------------*/
void Application::_windowResized(Ogre::RenderWindow*) {
}
void Application::_windowClosed(Ogre::RenderWindow*) {
}
/*-------------------------------------------------------------------------------------
	Frame Callback - proxies Ogre::FrameListener
-------------------------------------------------------------------------------------*/
bool Application::_frameRenderingQueued(const Ogre::FrameEvent& evt) {
	// Use the camera manager
	if (mCameraMan && mUseCameraMan) mCameraMan->frameRenderingQueued(evt);
	return true;
}
