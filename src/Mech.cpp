#include "Mech.h"
#include <math.h>  

//-------------------------------------------------------------------------------------
Mech::Mech(int id,Ogre::String name, Ogre::String type, Ogre::SceneManager* sceneMgr, Ogre::Real worldSize ):
        mId(id),
	mActive(true),
	mSpeed(1.0f),
	mMechDirection(Ogre::Math::PI/2),
	mTurnIncrementRadians(Ogre::Math::PI/250),
	mWorldSize(worldSize),
	mScale(1.8f),
	mName(name),
        mType(type),
	mSceneMgr(sceneMgr),
	mLaserSeconds(0.05f),
	mCurrentLaserSeconds(0.0f),
	mLaserLength(300.0f),
        mLaserIncremento(0.0f),
	mSpeedChange(0.5f)
{
	salto =0.0f;
	seguirSaltando=true;
	direccionSaltar=0;
	direccionMirar=0;
	
        if ( type.compare("p")==0 )
           mEntity = mSceneMgr->createEntity(name, "rabbit.mesh");	
        else 
           mEntity = mSceneMgr->createEntity(name, "rabbit.mesh");	

       mEntity->setMaterialName("Conejo");

	Ogre::AxisAlignedBox box = mEntity->getBoundingBox();
        mMechNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
        mMechNode->attachObject(mEntity);
	mMechNode->setScale(mScale, mScale, mScale);
	mMechLength = box.getSize().x;

        mLaser =  mSceneMgr->createManualObject("laser"+name); 
	mLaserNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("laser_node"+name); 
        mLaserNode->translate(0,10,0);

       
	mMechNode->setPosition(0,-box.getCorner(Ogre::AxisAlignedBox::FAR_LEFT_BOTTOM).y*mScale,300);
    altura =-box.getCorner(Ogre::AxisAlignedBox::FAR_LEFT_BOTTOM).y*mScale;
        
	mEntity->setCastShadows(true);
	mAnimationState = mEntity->getAnimationState("Run");
	mAnimationState->setEnabled(true);
        //mAnimationState = mEntity->getAnimationState("run");
	mAnimationState->setLoop(true);


	
}

//-------------------------------------------------------------------------------------
Mech::~Mech(void){
   mSceneMgr->destroyQuery(mRaySceneQuery);
}

void Mech::setActive(bool value) {
   mActive=value;
}


bool Mech::isActive(void) { return mActive; }

Ogre::Real Mech::getSpeed(void) { return mSpeed; }

Ogre::SceneNode* Mech::getMechNode() { return mMechNode; }

Ogre::Entity* Mech::getEntity() { return mEntity; }

Ogre::SceneNode* Mech::getLaserNode() { return mLaserNode; }

const Ogre::Vector3 & Mech::getPosition(void) { return mMechNode->getPosition(); }

Ogre::Vector3 Mech::getDirection(void) {
   return Ogre::Vector3(Ogre::Math::Cos(mMechDirection),0,Ogre::Math::Sin(mMechDirection));
}

void Mech::accelerate(void) {
	
   if (mSpeed<10 && mActive) 
      mSpeed+=mSpeedChange;

}

void Mech::decelerate(void) {
	
   if (mSpeed>0 && mActive) 
      mSpeed-=mSpeedChange;
}

void Mech::turnLeft(void) {
	
   if (mSpeed>0 && mActive) {
      mMechDirection -= mTurnIncrementRadians;
      mMechNode->yaw(Ogre::Radian(mTurnIncrementRadians));
   }

}

void Mech::turnRight(void) {

   if (mSpeed>0 && mActive) {		
      mMechDirection += mTurnIncrementRadians;
      mMechNode->yaw(Ogre::Radian(-mTurnIncrementRadians));
   }

}

void Mech::turnDown(void) {
	
   mLaserIncremento-=0.1;
   

}

void Mech::turnUp(void) {
	
   mLaserIncremento+=0.1;
   
}

void Mech::removeLaser(void) {
    mLaser->clear();
    mLaserNode->detachObject(mLaser);
}

void Mech::fireLaser() {

   if (mCurrentLaserSeconds<=0 && mActive) {	
      Ogre::Vector3 mechHeight = Ogre::Vector3(0, mEntity->getBoundingBox().getSize().y*0.5, 0);
      Ogre::Vector3 mechHeight2 = Ogre::Vector3(0, mEntity->getBoundingBox().getSize().y*mLaserIncremento, 0);
      mLaser->begin("LaserMaterial", Ogre::RenderOperation::OT_LINE_LIST); 
      mLaser->position(mechHeight+getPosition()+getDirection()*mMechLength/2); 
      mLaser->position(mechHeight2+getPosition()+getDirection()*(mLaserLength+mMechLength/2));  
      
      mLaser->end(); 
      mLaserNode->attachObject(mLaser);
      mCurrentLaserSeconds = mLaserSeconds;
	
   }
}

bool Mech::laserHits(Mech* opponent) {
	Ogre::Ray ray(getPosition(), getDirection());
    mRaySceneQuery->setRay(ray);
	Ogre::RaySceneQueryResult &result = mRaySceneQuery->execute();
    Ogre::RaySceneQueryResult::iterator itr;
	for (itr = result.begin(); itr != result.end(); itr++) {
		if (itr->movable->getName().compare(opponent->getName())==0) {
			return itr->distance<(mLaserLength+mMechLength/2);
		}
	}
	return false;
}

void Mech::mechDead(void)
{
	if (mActive) {
		mActive=false;
		mMechNode->translate(0.0f, mEntity->getBoundingBox().getHalfSize().y*mScale, 0.0f);
		mMechNode->pitch(Ogre::Radian(Ogre::Math::PI));
		mSpeed=0;
	}
}

void Mech::mechAlive() {
    Ogre::AxisAlignedBox box = mEntity->getBoundingBox();
    mMechNode->setPosition(0,-box.getCorner(Ogre::AxisAlignedBox::FAR_LEFT_BOTTOM).y*mScale,130);

}

const Ogre::String Mech::getName(void) 
{
	return mName;
}

void Mech::move(Ogre::Real time) 
{	
	if (mCurrentLaserSeconds>0) {
		mCurrentLaserSeconds-=time;
		if (mCurrentLaserSeconds<=0) {
			removeLaser();
		}
	}
	if (!mActive) {
		return;
	}
	if (mSpeed>0) {
		mAnimationState->setEnabled(true);
		mAnimationState->addTime(time * mSpeed/5);
		Ogre::Vector3 position = mMechNode->getPosition();
		if (!isCollision(position, getDirection())) {
			mMechNode->translate(getDirection() * mSpeed * time * 2.5);
			checkBounds(position);
		}
	} else {
		mAnimationState->setEnabled(false);
	}
}

void Mech::moverX(Ogre::Real tiempo) {	
	if(tiempo>0){
		if(mMechNode->getPosition().x<40){
			mMechNode->translate(Ogre::Vector3::UNIT_X*tiempo);
			direccionMirar=20;
		}
	}else{
		if(mMechNode->getPosition().x>-40){
			mMechNode->translate(Ogre::Vector3::UNIT_X*tiempo);
			direccionMirar=-20;
		}
	}
	
	
}

void Mech::moverZ(Ogre::Real tiempo, Ogre::Real camara) {	
	
	if(tiempo>0){
		if(mMechNode->getPosition().z-camara<160.0)mMechNode->translate(Ogre::Vector3::UNIT_Z*tiempo);
	}else{
		if(mMechNode->getPosition().z-camara>100.0)mMechNode->translate(Ogre::Vector3::UNIT_Z*tiempo);
	}

}

void Mech::animacionCorrer(Ogre::Real tiempo) 
{	
	
	mAnimationState->addTime(tiempo*2);


}
void Mech::avanzar(Ogre::Real tiempo) 
{	
	mMechNode->lookAt(Ogre::Vector3(mMechNode->getPosition().x+20,mMechNode->getPosition().y,mMechNode->getPosition().z-direccionMirar), Ogre::Node::TS_WORLD, Ogre::Vector3::UNIT_X);
	mMechNode->translate(Ogre::Vector3::UNIT_Z*tiempo);
	
	if (direccionSaltar==1)
	{
		if (mMechNode->getPosition().y>40)
		{
			direccionSaltar=-1;
		}else{
			mMechNode->translate(Ogre::Vector3::UNIT_Y*2.7f);
		}
	}
	if (direccionSaltar==-1)
	{
		if (mMechNode->getPosition().y<altura+2)
		{
			direccionSaltar=0;
		}else{
			mMechNode->translate(Ogre::Vector3::UNIT_Y*-2.7f);
		}
	}
	direccionMirar=direccionMirar/1.4;

	

}

void Mech::saltar(void) 
{	
	if(direccionSaltar==0)direccionSaltar=1;
	
}



bool Mech::isCollision(const Ogre::Vector3& position, const Ogre::Vector3& direction) 
{
	Ogre::Ray ray(position, direction);
    mRaySceneQuery->setRay(ray);
	Ogre::RaySceneQueryResult &result = mRaySceneQuery->execute();
    Ogre::RaySceneQueryResult::iterator itr;
	for (itr = result.begin(); itr != result.end(); itr++) {
		if (itr->movable->getName().compare(mName)!=0 && itr->distance<mMechLength/2) {
				return true;
		}
	}
	return false;
}

void Mech::checkBounds(const Ogre::Vector3& position)
{
	Ogre::Vector3 newPosition(position.x, position.y, position.z);
	bool changed = false;
	if (position.x>mWorldSize/2){
		newPosition.x = mWorldSize/2;
		changed = true;
	} else if (position.x<-mWorldSize/2) {
		newPosition.x = -mWorldSize/2;
		changed = true;
	}

	if (position.z>mWorldSize/2){
		newPosition.z = mWorldSize/2;
		changed = true;
	} else if (position.z<-mWorldSize/2) {
		newPosition.z = -mWorldSize/2;
		changed = true;
	}

	if (changed) {
		mMechNode->setPosition(newPosition);
		mAnimationState->setEnabled(false);
		mMechNode->yaw(Ogre::Radian(Ogre::Math::PI));
		mMechDirection = fmod(mMechDirection+Ogre::Math::PI, 2*Ogre::Math::PI);
		mSpeed = 0;
	}
}
