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
#include "MyFrameListener.h"
#include "Mech.h"
#include <string>
#include <sstream>
#include <iostream>

using namespace std; 


MyFrameListener::MyFrameListener(Ogre::RenderWindow* win, 
				 Ogre::Camera* cam, 
				 Mech *mech, 
				 Ogre::OverlayManager *om, 
				 Ogre::SceneManager *sm) {
  OIS::ParamList param;
  size_t windowHandle;  std::ostringstream wHandleStr;

  _camera = cam;
  mMech=mech;
  _overlayManager = om;
  _sceneManager = sm; 
  _win = win;
  turnoGround=false;
  limiteCambioGround=1000.0f;
  contador=0;
  fenceAvanceAcum=0;
  contadorFences=0;
  vidas=3;
  faseBoss=0;
  vidaBoss=0;

  _win->getCustomAttribute("WINDOW", &windowHandle);
  wHandleStr << windowHandle;
  param.insert(std::make_pair("WINDOW", wHandleStr.str()));
  
  #if defined OIS_WIN32_PLATFORM
  param.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_FOREGROUND" )));
  param.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_NONEXCLUSIVE")));
  param.insert(std::make_pair(std::string("w32_keyboard"), std::string("DISCL_FOREGROUND")));
  param.insert(std::make_pair(std::string("w32_keyboard"), std::string("DISCL_NONEXCLUSIVE")));
  #elif defined OIS_LINUX_PLATFORM
  param.insert(std::make_pair(std::string("x11_mouse_grab"), std::string("false")));
  param.insert(std::make_pair(std::string("x11_mouse_hide"), std::string("false")));
  param.insert(std::make_pair(std::string("x11_keyboard_grab"), std::string("false")));
  param.insert(std::make_pair(std::string("XAutoRepeatOn"), std::string("true")));
  #endif

  for(int i=0;i<10;i++){
      _animState[i] = NULL;
      _aleteo[i] = NULL;
      _dip[i] = NULL;
    }



  _inputManager = OIS::InputManager::createInputSystem(param);
  //para eventos de teclado
  _keyboard = static_cast<OIS::Keyboard*>
    (_inputManager->createInputObject(OIS::OISKeyboard, false));
 
  //para eventos de raton
  _mouse = static_cast<OIS::Mouse*>
    (_inputManager->createInputObject(OIS::OISMouse, false));
  _mouse->getMouseState().width = _win->getWidth();
  _mouse->getMouseState().height = _win->getHeight();

  //inicializamos objetos
  _raySceneQuery = _sceneManager->createRayQuery(Ogre::Ray());
  _selectedNode = NULL;
  ostringstream os;

  
  for(int i=0;i<10;i++){
    
    os << 'f' << i;
    _animState[i] =_sceneManager->getEntity(os.str())->getAnimationState("idle");
    _animState[i]->setEnabled(true);
    _animState[i]->setLoop(true);
    os.str("");

    os << 'a' << i;
    _aleteo[i] =_sceneManager->getEntity(os.str())->getAnimationState("flap");
    _aleteo[i]->setEnabled(true);
    _aleteo[i]->setLoop(true);
    os.str("");

    os << 'd' << i;
    _dip[i] =_sceneManager->getEntity(os.str())->getAnimationState("ArmatureAction");
    _dip[i]->setEnabled(true);
    _dip[i]->setLoop(true);
    os.str("");
    
  }
  
  



}

MyFrameListener::~MyFrameListener() {
  _inputManager->destroyInputObject(_keyboard);
  OIS::InputManager::destroyInputSystem(_inputManager);
  //destruimos los nuevo objetos
  _inputManager->destroyInputObject(_mouse);
  _sceneManager->destroyQuery(_raySceneQuery);
}

//añadimos un metodo que crea un nuevo objeto ray en funcion de la posicion del puntero
Ogre::Ray MyFrameListener::setRayQuery(int posx, int posy) {
  Ogre::Ray rayMouse = _camera->getCameraToViewportRay
    (posx/float(_win->getWidth()), posy/float(_win->getHeight()));
  _raySceneQuery->setRay(rayMouse);
  _raySceneQuery->setSortByDistance(true);
  
  return (rayMouse);
}


bool MyFrameListener::frameStarted(const Ogre::FrameEvent& evt) {
  Ogre::Vector3 vt(0,0,0);     
  Ogre::Real deltaT = evt.timeSinceLastFrame;

  _keyboard->capture();

   
  
  _mouse->capture();   // Captura eventos
  

  Ogre::Real velocidad=200.0f;
  ostringstream os;
  Ogre::OverlayElement *oe;

  for(int i=0;i<10;i++){
    _animState[i]->addTime(deltaT);  
    _aleteo[i]->addTime(deltaT);  
    _dip[i]->addTime(deltaT);
  }


  if(_keyboard->isKeyDown(OIS::KC_ESCAPE)) return false;

  //_sceneManager->getSceneNode("f0")->setPosition(mMech->getPosition()+(2.0f*Ogre::Vector3::UNIT_Y));
  _sceneManager->getSceneNode("flecha")->translate(Ogre::Vector3::UNIT_Z*deltaT*velocidad*2);



  
  
  if (faseBoss!=5)
  { 
    //moviento la entidad
    mMech->avanzar(deltaT*velocidad);
    mMech->animacionCorrer(deltaT);


    //moviendo la camara
    const Ogre::Quaternion &orientacion = _camera->getDerivedOrientation();
    Ogre::Vector3 xyzTranslation( velocidad*Ogre::Vector3::UNIT_Z*deltaT); //Move left, down, forward
    _camera->setPosition( _camera->getPosition() + orientacion * xyzTranslation*-1.0f );

      velocidad=75.0f;

    if (_keyboard->isKeyDown(OIS::KC_LEFT) ||_keyboard->isKeyDown(OIS::KC_A)) {
      mMech->moverX(deltaT*velocidad);

    }
    if (_keyboard->isKeyDown(OIS::KC_RIGHT) || _keyboard->isKeyDown(OIS::KC_D)) {
      mMech->moverX(-deltaT*velocidad);
    }
    if (_keyboard->isKeyDown(OIS::KC_UP) || _keyboard->isKeyDown(OIS::KC_W)) {
      mMech->moverZ(deltaT*velocidad,_camera->getPosition().z);
    }
    if (_keyboard->isKeyDown(OIS::KC_DOWN) || _keyboard->isKeyDown(OIS::KC_S)) {
      mMech->moverZ(-deltaT*velocidad,_camera->getPosition().z);
    }


    if (_keyboard->isKeyDown(OIS::KC_SPACE)) {
      mMech->saltar();
    } 

    


    //moviendo las fences
    fenceAvanceAcum=fenceAvanceAcum+deltaT*velocidad;
    while(fenceAvanceAcum>65){
      os << 'z' << contadorFences;
      _sceneManager->getSceneNode(os.str())->translate(0.0f, 0.0f, 4800.0f);
      os.str("");
      os << 'x' << contadorFences;
      _sceneManager->getSceneNode(os.str())->translate(0.0f, 0.0f, 4800.0f);
      os.str("");
      contadorFences++;
      if(contadorFences==30)contadorFences=0;
      fenceAvanceAcum=fenceAvanceAcum-65;

    }
  }



 


  //si se tocan
  bool perdioVida=false;

  for(int i=0;i<10;i++){ 

    //flor
    os << 'f' << i;
    if(mMech->getPosition().positionEquals(_sceneManager->getSceneNode(os.str())->getPosition()+(Ogre::Vector3::UNIT_Y*23.9), 24.f)){
      _sceneManager->getSceneNode(os.str())->setPosition(mMech->getPosition().x,0,mMech->getPosition().z-500);
      contador=0;
      perdioVida=true;
    } 
    os.str("");


    //aguila
    os << 'a' << i;
    if (mMech->getPosition().y>35)
    {
      if(mMech->getPosition().positionEquals(_sceneManager->getSceneNode(os.str())->getPosition(), 35.f)){
        _sceneManager->getSceneNode(os.str())->setPosition(mMech->getPosition().x,40,mMech->getPosition().z-500);
        contador=0;
        perdioVida=true;
      } 

    }
    os.str("");

    //dipvira
    os << 'd' << i;
    if(mMech->getPosition().positionEquals(_sceneManager->getSceneNode(os.str())->getPosition()+(Ogre::Vector3::UNIT_Y*2), 10.f)){
      _sceneManager->getSceneNode(os.str())->setPosition(mMech->getPosition().x,0,mMech->getPosition().z-500);
      contador=0;
      perdioVida=true;
    } 
    os.str("");
  }


  //flecha
  if(mMech->getPosition().positionEquals(_sceneManager->getSceneNode("flecha")->getPosition()+(Ogre::Vector3::UNIT_Z*35), 6.5f)){
      _sceneManager->getSceneNode("flecha")->setPosition(mMech->getPosition().x,0,mMech->getPosition().z+20000);
      contador=0;
      perdioVida=true;
    } 

  //daño boss
  if(_sceneManager->getSceneNode("d9")->getPosition().positionEquals(_sceneManager->getSceneNode("flecha")->getPosition()+(Ogre::Vector3::UNIT_Z*35), 15.f)){
    _sceneManager->getSceneNode("flecha")->setPosition(mMech->getPosition().x,0,mMech->getPosition().z+20000);
    if(faseBoss==3){
      faseBoss=4;
      vidaBoss--;
      oe = _overlayManager->getOverlayElement("vidaBoss");
      oe->setCaption(Ogre::StringConverter::toString(vidaBoss));
      if(vidaBoss==0){
        oe = _overlayManager->getOverlayElement("terminar");
        oe->setCaption("¡Ganaste!");
        faseBoss=5;
      }
    }
  } 

  

  if (perdioVida)
  {
    vidas--;
    oe = _overlayManager->getOverlayElement("vidas");
    oe->setCaption(Ogre::StringConverter::toString(vidas));
    if(vidas==0){
      oe = _overlayManager->getOverlayElement("terminar");
      oe->setCaption("¡Perdiste!");
      faseBoss=5;
    }

  }


  //flecha
  
  oe = _overlayManager->getOverlayElement("flechas");
  oe->setCaption(Ogre::StringConverter::toString(10-contador/20));

  if(contador==200){
    contador=0;
    _sceneManager->getSceneNode("flecha")->setPosition(mMech->getPosition().x,mMech->getPosition().y+6,mMech->getPosition().z-100);
  }else if(faseBoss!=5){
    contador++;
  }

  if(mMech->getPosition().z-100>=limiteCambioGround){
    
    limiteCambioGround=limiteCambioGround+2000;
    
    if(turnoGround){
      _sceneManager->getSceneNode("ground1")->setPosition(0,0,limiteCambioGround+1000);
    }else{
      _sceneManager->getSceneNode("ground2")->setPosition(0,0,limiteCambioGround+1000);
    }
    turnoGround=!turnoGround;
    
  }

  //posicionamiento de los enemigos



  if (mMech->getPosition().z<1750 && mMech->getPosition().z>1700)
  {
    _sceneManager->getSceneNode("a0")->setPosition(-50,40,3500);
    _sceneManager->getSceneNode("a1")->setPosition(-17,40,3500);
    _sceneManager->getSceneNode("a2")->setPosition(17,40,3500);
    _sceneManager->getSceneNode("a3")->setPosition(50,40,3500);
  }

  if (mMech->getPosition().z<2750 && mMech->getPosition().z>2700)
  {
    _sceneManager->getSceneNode("f0")->setPosition(0,0,4500);
    _sceneManager->getSceneNode("f1")->setPosition(25,0,4700);
    _sceneManager->getSceneNode("f2")->setPosition(-25,0,4900);
  }

  if (mMech->getPosition().z<3750 && mMech->getPosition().z>3700)
  {
    _sceneManager->getSceneNode("f3")->setPosition(0,0,5900);
    _sceneManager->getSceneNode("a4")->setPosition(-35,40,5900);
    _sceneManager->getSceneNode("d4")->setPosition(35,0,5900);
  }

    if (mMech->getPosition().z<5000 && mMech->getPosition().z>4950)
  {
    _sceneManager->getSceneNode("f0")->setPosition(30,0,6800);
    _sceneManager->getSceneNode("f1")->setPosition(-30,0,6800);
    _sceneManager->getSceneNode("d1")->setPosition(0,0,6850);
    _sceneManager->getSceneNode("a0")->setPosition(0,40,7000);
    _sceneManager->getSceneNode("d0")->setPosition(-25,0,7050);
    _sceneManager->getSceneNode("d2")->setPosition(0,0,7050);
  }

  //boss
  if (mMech->getPosition().z<7450 && mMech->getPosition().z>7400)
  {
    faseBoss=1;
    vidaBoss=3;
    oe = _overlayManager->getOverlayElement("vidaBoss");
    oe->setCaption(Ogre::StringConverter::toString(vidaBoss));

  }
  if (faseBoss>0 && faseBoss!=5)
  {    
    _dip[9]->addTime(deltaT*5);

    if (contador==190){//si se esta por disparar que cambie de fase

      if(faseBoss==2){
        faseBoss++;
      }
      
      if(faseBoss==1){

        _sceneManager->getSceneNode("d0")->setPosition(-36,0,_sceneManager->getSceneNode("d9")->getPosition().z-50);
        _sceneManager->getSceneNode("d1")->setPosition(-12,0,_sceneManager->getSceneNode("d9")->getPosition().z-50);
        _sceneManager->getSceneNode("d2")->setPosition(12,0,_sceneManager->getSceneNode("d9")->getPosition().z-50);
        _sceneManager->getSceneNode("d3")->setPosition(36,0,_sceneManager->getSceneNode("d9")->getPosition().z-50);

        faseBoss++;
      }
    } 

    if(contador==60 && (faseBoss==3  || faseBoss==4)){ //que termine la fase de cargar y regrese a la 1
      faseBoss=1;
      _sceneManager->getEntity("d9")->setVisible(true);
    }
    
    if(faseBoss==3){//cargar en fase 3
      _sceneManager->getSceneNode("d9")->translate(0,0,-15); 
    }else{
      if(faseBoss==4){//parpadear en fase 4
        if(contador%10<5)_sceneManager->getEntity("d9")->setVisible(false);
        else _sceneManager->getEntity("d9")->setVisible(true);
        _sceneManager->getSceneNode("d9")->translate(0,0,deltaT*velocidad*3);
      }else //imitar movimientos en el resto de las fases
        _sceneManager->getSceneNode("d9")->setPosition(mMech->getPosition().x,0,mMech->getPosition().z+500);
    }


  }
  return true;
}
