#include <Ogre.h>
#include <Overlay/OgreOverlayManager.h>
#include <Overlay/OgreOverlayElement.h>
#include <Overlay/OgreOverlaySystem.h>
#include "MyFrameListener.h"
#include "Mech.h"

class MyApp {
  
private:
  Ogre::SceneManager* _sceneManager;
  Ogre::Root* _root;
  MyFrameListener* _framelistener;
  Ogre::OverlayManager* _overlayManager;
  Mech *mMech;

  
public:
  MyApp();
  virtual ~MyApp();  
  int start();
  void loadResources();
  void createScene();
  void createOverlay();
  virtual bool frameRenderingQueued(const Ogre::FrameEvent &evt);
};
