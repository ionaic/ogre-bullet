#include "client.h"
#include <OgreEntity.h>
#include <OgreCompositorManager.h>

Application *MainApplication = new Client();

Client::Client() {}
Client::~Client() {}

void Client::initScene() {
	Ogre::Vector3 lightPosition = Ogre::Vector3(0,10,50);
	
	// Set the scene's ambient light
	mSceneMgr->setAmbientLight(Ogre::ColourValue(.1f, .1f, .1f));
	Ogre::Light* pointLight = mSceneMgr->createLight("pointLight");
	pointLight->setType(Ogre::Light::LT_POINT);
	pointLight->setPosition(lightPosition);
	// create a marker where the light is
	Ogre::Entity* ogreHead2 = mSceneMgr->createEntity( "Head2", "ogrehead.mesh" );
	Ogre::SceneNode* headNode2 = mSceneMgr->getRootSceneNode()->createChildSceneNode( "HeadNode2", lightPosition );
	headNode2->attachObject( ogreHead2 );
	headNode2->scale( .5, .5, .5 );
	
	// make a quad
	Ogre::ManualObject* lManualObject = NULL;
	Ogre::String lNameOfTheMesh = "TestQuad";
	{
		// params
		Ogre::String lManualObjectName = "TestQuad";
		bool lDoIWantToUpdateItLater = false;
		// code
		lManualObject = mSceneMgr->createManualObject(lManualObjectName);
		lManualObject->setDynamic(lDoIWantToUpdateItLater);
		lManualObject->begin("BaseWhiteNoLighting", Ogre::RenderOperation::OT_TRIANGLE_LIST);
		{	// Specify Vertices
			float xmin = 0.0f, xmax = 100.0f, zmin = -100.0f, zmax = 0.0f, ymin = -50.0f, ymax = 0.0f;
			
			lManualObject->position(xmin, ymin, zmin);// a vertex
			lManualObject->colour(Ogre::ColourValue::Red);
			lManualObject->textureCoord(0.0,0.0);
			
			lManualObject->position(xmax, ymin, zmin);// a vertex
			lManualObject->colour(Ogre::ColourValue::Green);
			lManualObject->textureCoord(1.0,0.0);
			
			lManualObject->position(xmin, ymin, zmax);// a vertex
			lManualObject->colour(Ogre::ColourValue::Blue);
			lManualObject->textureCoord(0.0,1.0);
			
			lManualObject->position(xmax, ymin, zmax);// a vertex
			lManualObject->colour(Ogre::ColourValue::Blue);
			lManualObject->textureCoord(1.0,1.0);
		}
		{	// specify geometry
			lManualObject->triangle(0,2,1);
			lManualObject->triangle(3,1,2);
		}
		lManualObject->end();
		lManualObject->convertToMesh(lNameOfTheMesh);
	}
	Ogre::Entity* lEntity = mSceneMgr->createEntity(lNameOfTheMesh);
	Ogre::SceneNode* lNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	lNode->attachObject(lEntity);
	Ogre::Entity* ogreHead = mSceneMgr->createEntity("Head", "ogrehead.mesh");
	// Create a SceneNode and attach the Entity to it
	Ogre::SceneNode* headNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("HeadNode",Ogre::Vector3(0,-20,0));
	headNode->attachObject(ogreHead);
	ogreHead->setMaterialName("Hatching");
	
	lEntity->setMaterialName("Hatching");
	// postprocessing effects
	//Ogre::CompositorManager::getSingleton().addCompositor(mViewport,"Sobel" );
	//Ogre::CompositorManager::getSingleton().setCompositorEnabled(mCamera->getViewport(), "Sobel", true);
	//Ogre::CompositorManager::getSingleton().addCompositor(mViewport,"CelShading" );
	//Ogre::CompositorManager::getSingleton().setCompositorEnabled(mCamera->getViewport(), "CelShading", true);
}
