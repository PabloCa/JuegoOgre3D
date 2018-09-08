/*********************************************************************
 * Módulo 2. Curso de Experto en Desarrollo de Videojuegos
 * Autor: Carlos González Morcillo     Carlos.Gonzalez@uclm.es
 *
 * You can redistribute and/or modify this file under the terms of the
 * GNU General Public License ad published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * and later version. See <http://www.gnu.org/licenses/>.
 *
 * This file is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.  
 *********************************************************************/

#include "MyApp.h" 
#include "Mech.h" 
#include <string>
#include <sstream>
#include <iostream>

using namespace std; 

MyApp::MyApp() {
  _sceneManager = NULL;
  _framelistener = NULL;
}

MyApp::~MyApp() {
  delete _root;
  delete _framelistener;
}

int MyApp::start() {
  _root = new Ogre::Root();
  
  if(!_root->restoreConfig()) {
    _root->showConfigDialog();
    _root->saveConfig();
  }
  
  Ogre::RenderWindow* window = _root->initialise(true,"MyApp Example");
  _sceneManager = _root->createSceneManager(Ogre::ST_GENERIC);
  
  Ogre::Camera* cam = _sceneManager->createCamera("MainCamera");
  //cam->setPosition(Ogre::Vector3(0,150,200));
  //cam->lookAt(Ogre::Vector3(0,1,0));


  cam->setPosition(Ogre::Vector3(0,40,200));
  cam->lookAt(Ogre::Vector3(0,40,201));

  cam->setNearClipDistance(5);
  cam->setFarClipDistance(2000);
  
  Ogre::Viewport* viewport = window->addViewport(cam);
  viewport->setBackgroundColour(Ogre::ColourValue(0.6f,0.6f,1.0f));
  double width = viewport->getActualWidth();
  double height = viewport->getActualHeight();
  cam->setAspectRatio(width / height);
  
  Ogre::OverlaySystem* pOverlaySystem = OGRE_NEW Ogre::OverlaySystem();
  _sceneManager->addRenderQueueListener(pOverlaySystem);


  loadResources();
  createScene();
  createOverlay();

  mMech = new Mech(0,"eConejo","p", _sceneManager, 200);

  _framelistener = new MyFrameListener(window, cam, mMech, _overlayManager, _sceneManager);
  _root->addFrameListener(_framelistener);
  
  _root->startRendering();
  return 0;
}

void MyApp::loadResources() {
  Ogre::ConfigFile cf;
  cf.load("resources.cfg");
  
  Ogre::ConfigFile::SectionIterator sI = cf.getSectionIterator();
  Ogre::String sectionstr, typestr, datastr;
  while (sI.hasMoreElements()) {
    sectionstr = sI.peekNextKey();
    Ogre::ConfigFile::SettingsMultiMap *settings = sI.getNext();
    Ogre::ConfigFile::SettingsMultiMap::iterator i;
    for (i = settings->begin(); i != settings->end(); ++i) {
      typestr = i->first;    datastr = i->second;
      Ogre::ResourceGroupManager::getSingleton().addResourceLocation
            (datastr, typestr, sectionstr);	
    }
  }
  Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
}

void MyApp::createScene() {

  //primero creamos el plano para el suelo
  Ogre::Plane plane1(Ogre::Vector3::UNIT_Y, 0);
  Ogre::MeshManager::getSingleton().createPlane("plane",
  Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane1,
  2000,2000,1,1,true,1,200,200,Ogre::Vector3::UNIT_Z);


  Ogre::SceneNode* node2 = _sceneManager->createSceneNode("ground1");
  Ogre::Entity* groundEnt = _sceneManager->createEntity("planeEnt", "plane");
  groundEnt->setMaterialName("grass");
  node2->attachObject(groundEnt);
  node2->setPosition(0,0,2000);
  _sceneManager->getRootSceneNode()->addChild(node2);

  Ogre::SceneNode* node = _sceneManager->createSceneNode("ground2");
  groundEnt = _sceneManager->createEntity("planeEnt2", "plane");
  groundEnt->setMaterialName("grass");
  node->attachObject(groundEnt);
  node->setPosition(0,0,0);
  _sceneManager->getRootSceneNode()->addChild(node);

  _sceneManager->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_ADDITIVE); 
  Ogre::Light* light = _sceneManager->createLight("Light1");
  light->setType(Ogre::Light::LT_DIRECTIONAL);
  light->setDirection(Ogre::Vector3(0,-1,1));
  node->attachObject(light);

  
  ostringstream os;

  for(int i=0;i<10;i++){  
  
    os << 'f' << i;
    groundEnt = _sceneManager->createEntity(os.str(), "imp_flower.mesh");
    groundEnt->setMaterialName("imp_flower");
    groundEnt->setCastShadows(false);
    node2= _sceneManager->createSceneNode(os.str());
    node2->attachObject(groundEnt);
    _sceneManager->getRootSceneNode()->addChild(node2);
    node2->yaw(Ogre::Degree(180));
    node2->setPosition(i*10,0,0);
    node2->setScale(Ogre::Vector3(60.0f,60.0f,60.0f));
    os.str("");


    os << 'a' << i;
    groundEnt = _sceneManager->createEntity(os.str(), "flying_bird.mesh");
    groundEnt->setMaterialName("Aguila");
    groundEnt->setCastShadows(false);
    node2= _sceneManager->createSceneNode(os.str());
    node2->attachObject(groundEnt);
    _sceneManager->getRootSceneNode()->addChild(node2);
    node2->setPosition(i*10,10,0);
    node2->setScale(Ogre::Vector3(70.0f,70.0f,70.0f));
    os.str("");

    os << 'd' << i;
    groundEnt = _sceneManager->createEntity(os.str(), "dipvira.mesh");
    groundEnt->setMaterialName("dipvira");
    groundEnt->setCastShadows(false);
    node2= _sceneManager->createSceneNode(os.str());
    node2->attachObject(groundEnt);
    _sceneManager->getRootSceneNode()->addChild(node2);
    node2->yaw(Ogre::Degree(180));
    node2->setPosition(i*10,10,0);
    node2->setScale(Ogre::Vector3(8.0f,8.0f,8.0f));
    os.str("");
  }

  _sceneManager->getSceneNode("d9")->setScale(Ogre::Vector3(20.0f,15.0f,20.0f));

  groundEnt = _sceneManager->createEntity("flecha", "arrow_b.mesh");
  groundEnt->setMaterialName("flecha");
  //groundEnt->setCastShadows(false);
  node2= _sceneManager->createSceneNode("flecha");
  node2->attachObject(groundEnt);
  node2->setScale(Ogre::Vector3(40.0f,40.0f,40.0f));
  node2->setPosition(0,20,20000);
  node2->roll(Ogre::Degree(90));
  _sceneManager->getRootSceneNode()->addChild(node2);


  //las fences, z derecha, x izquierda(creo)
  for(int i=0;i<30;i++){  

    os << 'z' << i;
    groundEnt = _sceneManager->createEntity(os.str(), "fence_section_a.mesh");
    groundEnt->setMaterialName("fence");
    groundEnt->setCastShadows(false);
    node2= _sceneManager->createSceneNode(os.str());
    node2->attachObject(groundEnt);
    node2->setScale(Ogre::Vector3(80.0f,10.0f,10.0f));
    node2->setPosition(-60,0,i*160+400);
    node2->yaw(Ogre::Degree(90));
    _sceneManager->getRootSceneNode()->addChild(node2);
    os.str("");

    os << 'x' << i;
    groundEnt = _sceneManager->createEntity(os.str(), "fence_section_a.mesh");
    groundEnt->setMaterialName("fence");
    groundEnt->setCastShadows(false);
    node2= _sceneManager->createSceneNode(os.str());
    node2->attachObject(groundEnt);
    node2->setScale(Ogre::Vector3(80.0f,10.0f,10.0f));
    node2->setPosition(60,0,i*160+400);
    node2->yaw(Ogre::Degree(90));
    _sceneManager->getRootSceneNode()->addChild(node2);
    os.str("");

  }


  _sceneManager->getSceneNode("d0")->setPosition(-36,0,2500);
  _sceneManager->getSceneNode("d1")->setPosition(-12,0,2500);
  _sceneManager->getSceneNode("d2")->setPosition(12,0,2500);
  _sceneManager->getSceneNode("d3")->setPosition(36,0,2500);

 
}

void MyApp::createOverlay() {

  



  _overlayManager = Ogre::OverlayManager::getSingletonPtr();
  Ogre::Overlay *overlay = _overlayManager->getByName("Info");
  overlay->show();
  Ogre::OverlayElement *oe;
  oe = _overlayManager->getOverlayElement("vidas");
  oe->setCaption(Ogre::StringConverter::toString(3));
  oe = _overlayManager->getOverlayElement("flechas");
  oe->setCaption(Ogre::StringConverter::toString(0)); 
 
}

bool MyApp::frameRenderingQueued(const Ogre::FrameEvent &arg) {
 

   
   return true;

   

}




