//
// Created by Chris Greer on 01/04/2024.
//
#ifndef G52CPP_LIVINGOBJECT_H
#define G52CPP_LIVINGOBJECT_H

#include "../../header.h"
#include "GameObject.h"
#include "../ZEngine.h"
#include "../ZUtility/InfoStructs.h"
#include "../ZPixels/ImageLoader.h"
#include "../ZMaps/MapTileManager.h"
#include "../ZUtility/Animator.h"

using namespace std;

//Intermediate class between game objects and subclasses
//Just adds the fact it's tracked whether it's moving
//And that they have images that move (animate)

class LivingObject : public GameObject{
public:
    ~LivingObject() {
        m_movingImages.reset();
        m_meleePixelMaps.reset();
        m_movementPixelMaps.reset();
        m_meleeImages.reset();
        m_deathImages.reset();
    }
    LivingObject(ZEngine *pEngine,
                 const string& directoryPath,
                 ObjectInfo info,
                 double centerX, double centerY)
        : GameObject(pEngine,info.x,info.y,directoryPath,centerX,centerY),
        m_collisionMap(pEngine->getCollisionMap().get()), m_health(info.health),m_armour(info.armour){

        //m_movingImages.clear();
        //m_image = LivingObject::setImage(); //Set image based on the first of the path
        //initialiseImages(); //Then initialise

    }
    MapTileManager* getCollisionMap(){ return m_collisionMap; }
    int getHalfBuffer() const { return m_halfBuffer; }
    bool isDead() const {return m_dead;};
    virtual void beenHit(int critDistance) = 0;
    virtual void die() = 0;
    int getHealth() const {return m_health;};
    int getArmour() const {return m_armour;};

protected:

    //Animates and updates pixel maps of a living object
    bool animateAndUpdatePixelMaps(const vector<shared_ptr<SimpleImage>>& images, vector<PixelMap>& pixelMaps,
                                   int currentTime, int timeLag){
        if (Animator::animate(m_image,images,currentTime,m_animationCounter,m_imagesLastUpdated,timeLag)){
            m_pixelMap = &(pixelMaps[m_animationCounter]); //Update our pixel maps
            return true;
        }
        return false;
    }

    //Used for animation
    int m_animationCounter = 0;
    int m_imagesLastUpdated = 0;
    int m_actionLastUpdated = 0;
    bool m_moving = false;
    // Used for collision detection
    int m_halfBuffer = 17;
    //General information
    bool m_dead = false;
    int m_health = 100;
    int m_armour = 0;

    MapTileManager* m_collisionMap;
    shared_ptr<vector<shared_ptr<SimpleImage>>> m_movingImages = nullptr;
    shared_ptr<vector<PixelMap>> m_movementPixelMaps = nullptr;
    shared_ptr<vector<PixelMap>> m_meleePixelMaps = nullptr;
    shared_ptr<vector<shared_ptr<SimpleImage>>> m_meleeImages = nullptr;
    shared_ptr<vector<shared_ptr<SimpleImage>>> m_deathImages = nullptr;
};


#endif //G52CPP_LIVINGOBJECT_H

