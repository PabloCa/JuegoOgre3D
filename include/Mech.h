#ifndef __Mech_h_
#define __Mech_h_

#include "Ogre.h"

class Mech
{
public:
    Mech(int id,Ogre::String name, Ogre::String type, Ogre::SceneManager* sceneMgr, Ogre::Real worldSize);
    virtual ~Mech(void);

	const Ogre::Vector3 & getPosition(void);
	Ogre::Vector3 getDirection(void);
	Ogre::Real getSpeed(void);
        Ogre::SceneNode *getMechNode();
        Ogre::SceneNode *getLaserNode();
        Ogre::Entity *getEntity();
	void accelerate(void);
	void decelerate(void);
	void turnLeft(void);
	void turnRight(void);
        void turnUp(void);
	void turnDown(void);	
	const Ogre::String getName(void);
	virtual void animacionCorrer(Ogre::Real time);
	virtual void move(Ogre::Real time);
	virtual void moverX(Ogre::Real time);
	virtual void moverZ(Ogre::Real time,Ogre::Real camara);
	virtual void avanzar(Ogre::Real time);
	virtual void saltar(void);
	virtual void fireLaser();
	void mechDead(void);
	bool isActive(void);
        void setActive(bool value);
        void mechAlive();

        Ogre::BillboardSet *BSFuego;
        Ogre::Billboard *BFuego;
        Ogre::SceneNode *nodoFuego;
        int nShoot=0;

        float salto;
        float direccionMirar;
	float altura;
	bool seguirSaltando;

	int direccionSaltar;

protected:

        int mId;
        bool mActive;
        Ogre::Real mSpeed;
        Ogre::Real mMechDirection;  
        Ogre::Real mTurnIncrementRadians;
        Ogre::Real mWorldSize;
        Ogre::Real mScale;
        Ogre::String mName;
        Ogre::String mType;
	Ogre::SceneManager* mSceneMgr;
	Ogre::Real mLaserSeconds;
	Ogre::Real mCurrentLaserSeconds;
	Ogre::Real mLaserLength;
	Ogre::Real mLaserIncremento;//para subir o bajar el laser

	Ogre::Real mSpeedChange;
	
        
	Ogre::Real mMechLength;
	
	
	
	Ogre::Entity* mEntity;
	Ogre::SceneNode* mMechNode;
	Ogre::AnimationState *mAnimationState; // The current animation state of the object	
	
	Ogre::RaySceneQuery *mRaySceneQuery;
	Ogre::ManualObject* mLaser; 
	Ogre::SceneNode* mLaserNode; 
        //para simular choque de bola
        

	virtual bool isCollision(const Ogre::Vector3& position, const Ogre::Vector3& direction); 
	virtual void checkBounds(const Ogre::Vector3& position);
	void removeLaser(void); 
	bool laserHits(Mech* opponent);

private:	
	

};

#endif // #ifndef __Mech_h_
