//
// Created by Chris Greer on 22/03/2024.
//

#include "ZPlayer.h"
#include "../ZPixels/ImagePixelRepo.h"
#include "../ZPixels/RayTrace.h"
#include "../ZPixels/PixelCollisionUtil.h"
#include "../ZUtility/InfoStructs.h"

ZPlayer::ZPlayer(ZEngine* pEngine, ObjectInfo info)
        : LivingObject(pEngine,"./resources/Player/Pistol",
                       {pEngine->getWindowWidth()/2,pEngine->getWindowHeight()/2,
                        info.type,info.typeKey,info.health,info.armour},
                       0.54, 0.335 ), m_rifleAmmo(info.ammo){ //Hardcoded here as only one player object


    m_idlePistol = (*ImagePixelRepo::getMultiImages()).at("PistolWalk");
    m_idlePistolPixelMaps = (*ImagePixelRepo::getMultiPixelMaps()).at("PistolWalk");
    //Set up the default starting images/pixel maps
    m_movingImages = m_idlePistol;
    m_image = (*m_movingImages)[0];
    m_movementPixelMaps = m_idlePistolPixelMaps;
    m_pixelMap = &(*m_movementPixelMaps)[0];
    m_defaultPixelMap = &(*m_movementPixelMaps)[0];


    m_idleRifle = (*ImagePixelRepo::getMultiImages()).at("RifleWalk");
    m_idleRiflePixelMaps = (*ImagePixelRepo::getMultiPixelMaps()).at("RifleWalk");

    m_meleeImages = (*ImagePixelRepo::getMultiImages()).at("BatAttack");
    m_meleePixelMaps = (*ImagePixelRepo::getMultiPixelMaps()).at("BatAttack");

    // Add a small offset (equivalent to 4 degrees counter-clockwise)
    //This is due to the weapons being placed slightly off center
    m_initialRotation = (4 * (M_PI / 180.0)) + (M_PI / 2.0);
    initialiseImages();

}

void ZPlayer::beenHit(int critDistance) {
    //For player hits are always melee (For now)
    //Will leave logic it can be altered in the future

    //Determine whether to reduce from armour or health
    int* reduceFrom = (m_armour > 0) ? &m_armour : &m_health;
    //Also determines modifier (armour is harder to damage)
    int modifier = (m_armour > 0) ? 2 : 3;

    *reduceFrom -= 5 * modifier;

    if(m_health < 1){
        die();
    }
}

void ZPlayer::virtDraw() {

    if (!isVisible()) return;
    //Call base class method
    LivingObject::virtDraw();

    //Draw the laser pointer too
    if (!m_melee && !m_rotationBlocked){
        int offset = m_currentWeapon == w_pistol ? pistolXOffset : rifleXOffset;
        int rotationOffset = m_currentWeapon == w_pistol ? pistolYOffset : rifleYOffset;
        RayTrace::playerLineOfSight(this, dynamic_cast<ZEngine *>(m_pEngine), offset, rotationOffset);
    }

}
//Change to account for multiple weapons
void ZPlayer::switchWeapon(){
    if (m_currentWeapon == w_pistol){
        m_movingImages = m_idleRifle;
        m_movementPixelMaps = m_idleRiflePixelMaps;
        m_currentWeapon = w_rifle;
    } else {
        m_movingImages = m_idlePistol;
        m_movementPixelMaps = m_idlePistolPixelMaps;
        m_currentWeapon = w_pistol;
    }
    m_image = (*m_movingImages)[0];
    m_pixelMap = &(*m_movementPixelMaps)[0];
}

void ZPlayer::fireWeapon() {

    if (!isVisible() || m_firing) return;

    m_firing = true;
    m_animationCounter = 1;
    //DEPENDING ON WEAPON
    if (m_currentWeapon == w_pistol){
        m_shootingImages = (*ImagePixelRepo::getMultiImages()).at("PistolShot");
    } else{
        if (m_rifleAmmo == 0) return; //No Ammo to shoot
        m_shootingImages = (*ImagePixelRepo::getMultiImages()).at("RifleShot");
    }
    m_image = (*m_shootingImages)[m_animationCounter];
}

void ZPlayer::stopFiring(){
    if (m_currentWeapon != w_pistol){ //Pistol will always fire completely
        m_firing = false;
        m_image = (*m_movingImages)[0]; //Other weapons need to reset
    }
}

void ZPlayer::melee(){
    if (!isVisible() || m_melee) return;

    m_melee = true;
    m_animationCounter = 1;
    //DEPENDING ON WEAPON
    m_image = (*m_meleeImages)[m_animationCounter];
    m_pixelMap = &(*m_meleePixelMaps)[m_animationCounter];
}

void ZPlayer::virtDoUpdate(int iCurrentTime) {

    if (!isVisible()) return;

    //Don't move every time we update
    int lag = iCurrentTime - m_actionLastUpdated;

    if (m_firing){ //Firing takes priority (even if firing AND moving)
        //If we've looped all the way round (or interrupted), stop shooting
        if (m_animationCounter == 0 && m_currentWeapon == w_pistol) {
            m_firing = false; //Stop firing (pistol is once per click)
        }else if (m_currentWeapon == w_rifle && m_rifleAmmo == 0){
            stopFiring();
        } else{
            //Only damage enemy once per loop for pistol
            //But every 3 animations for rifle
            if (Animator::animate(m_image,*m_shootingImages,iCurrentTime,m_animationCounter,m_imagesLastUpdated,40) &&
                                    ((m_animationCounter == 0 && m_currentWeapon == w_pistol) ||
                                    ((m_animationCounter % 3 == 2 ) && m_currentWeapon == w_rifle))){
                //REDUCE AMMO
                if (m_currentWeapon == w_rifle) m_rifleAmmo--;

                //Find out if someone is in the sights
                ZEngine::InSights enemy = dynamic_cast<ZEngine*>(m_pEngine)->getInSights();
                if (enemy.object != nullptr){
                    enemy.object->beenHit(enemy.critDistance); //If so they've been hit (depending on crit distance)
                }
            }
        }
    } else if (m_melee) { //then Melee
        //If we've looped all the way round (or interrupted), stop attacking
        if (m_animationCounter == 0){
            m_melee = false;
            m_image = (*m_movingImages)[m_animationCounter]; //Change back to regular images and pixel maps
            m_pixelMap = &(*m_movementPixelMaps)[m_animationCounter];
        }
        else{
            if (animateAndUpdatePixelMaps(*m_meleeImages, *m_meleePixelMaps, iCurrentTime, 40) &&
                (m_animationCounter == 4 || m_animationCounter == 5 || m_animationCounter == 6)){
                //Check for any collision
                PixelCollisionUtil::checkAllEnemyCollisions(this, dynamic_cast<ZEngine *>(m_pEngine));
            }
        }

    } else if (m_moving){
        animateAndUpdatePixelMaps(*m_movingImages, *m_movementPixelMaps, iCurrentTime, 200);
    }

    //Update rotation and redraw
    double previousRotation = m_rotateAmount;
    m_rotateAmount = MathUtil::rotation(getExactRealCenterX(),
                                            getExactRealCenterY(),
                                            getEngine()->getCurrentMouseX(),
                                            getEngine()->getCurrentMouseY(),
                                            m_initialRotation);
    //Rotate
    setRotation(m_rotateAmount);
    //Check if we're now colliding
    if(PixelCollisionUtil::checkTileCollision(m_collisionMap,this)){
        m_rotateAmount = previousRotation;
        setRotation(m_rotateAmount);
        m_rotationBlocked = true;
    } else {
        m_rotationBlocked = false;
    }
    //If so rotate back...
}

void ZPlayer::setMoving(bool mMoving) { m_moving = mMoving; }

void ZPlayer::die() {
    dynamic_cast<ZEngine*>(m_pEngine)->flagGameOver();
}




