//
// Created by Chris Greer on 22/03/2024.
//

#ifndef G52CPP_ZPLAYER_H
#define G52CPP_ZPLAYER_H

#include "../../header.h"
#include "LivingObject.h"
#include "../ZUtility/Animator.h"
#include "../ZPixels/ImageLoader.h"
#include "../ZUtility/InfoStructs.h"

using namespace std;

class ZPlayer : public LivingObject {
public:
    ZPlayer(ZEngine* pEngine, ObjectInfo info);
    ~ZPlayer() {
        m_shootingImages.reset();
        m_idlePistol.reset();
        m_idleRifle.reset();
        m_idlePistolPixelMaps.reset();
        m_idleRiflePixelMaps.reset();
    }
    enum Weapon {w_pistol, w_rifle};
    void fireWeapon();
    void stopFiring();
    void virtDoUpdate(int iCurrentTime) override;
    void virtDraw() override;
    void setMoving(bool mMoving);
    void melee();
    void switchWeapon();
    void beenHit(int critDistance) override;
    void reload(){ m_rifleAmmo+= (rand() % 10) + 10; } //Random amount between 10 - 20
    void heal(){ m_health = min(m_health + 50, 100); } // +50 up to maximum
    void armor(){ m_armour = 100; } // Fully re-armors
    Weapon getWeapon() const {return m_currentWeapon;}
    int getAmmo() const {return m_rifleAmmo;}
    void die() override;


private:
    Weapon m_currentWeapon = w_pistol;
    bool m_firing = false;
    int m_rifleAmmo = 0;
    bool m_rotationBlocked = false;
    int pistolXOffset = 11;
    int pistolYOffset = 35;
    int rifleXOffset = 12;
    int rifleYOffset = 20;
    bool m_melee = false;
    shared_ptr<vector<shared_ptr<SimpleImage>>> m_shootingImages = nullptr;
    shared_ptr<vector<shared_ptr<SimpleImage>>> m_idlePistol = nullptr;
    shared_ptr<vector<shared_ptr<SimpleImage>>> m_idleRifle = nullptr;
    shared_ptr<vector<PixelMap>> m_idlePistolPixelMaps = nullptr;
    shared_ptr<vector<PixelMap>> m_idleRiflePixelMaps = nullptr;

};


#endif //G52CPP_ZPLAYER_H
