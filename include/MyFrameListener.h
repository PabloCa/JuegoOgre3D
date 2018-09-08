#include <Ogre.h>
#include <Overlay/OgreOverlayManager.h>
#include <Overlay/OgreOverlayElement.h>
#include <OIS/OIS.h>
#include "Mech.h" 

class MyFrameListener : public Ogre::FrameListener {
private:
  OIS::InputManager* _inputManager;
  OIS::Keyboard* _keyboard;
  
  Ogre::Camera* _camera;
  Ogre::RenderWindow* _win;
 

  Mech* mMech;

  Ogre::OverlayManager* _overlayManager;
  Ogre::SceneManager* _sceneManager;
  Ogre::AnimationState *_animState[10];
  Ogre::AnimationState *_aleteo[10];
  Ogre::AnimationState *_dip[10];

  int contador;
  int contadorFences;
  int vidas;
  int faseBoss;
  int vidaBoss;
  bool turnoGround;
  float limiteCambioGround;
  float fenceAvanceAcum;
  //=========================================================================
  // Recursos necesarios para seleccionar objetos en ogre
  //=========================================================================
  OIS::Mouse* _mouse;
  //Objto para guardar el objeto que se ha seleccionado
  Ogre::SceneNode *_selectedNode;
  //Objeto para seleccion de entidades mediante el mecanismo de consulta
  Ogre::RaySceneQuery *_raySceneQuery;
  //Cabecera del metodo que modifica objeto en funcion de la posicion del mouse
  Ogre::Ray setRayQuery(int posx, int posy);

public:
  MyFrameListener(Ogre::RenderWindow* win, Ogre::Camera* cam, 
		  Mech* mech, Ogre::OverlayManager* om, 
		  Ogre::SceneManager* sm);
  ~MyFrameListener();
  bool frameStarted(const Ogre::FrameEvent& evt);  
};
