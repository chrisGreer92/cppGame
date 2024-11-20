//
// Created by Chris Greer on 01/04/2024.
//

#ifndef G52CPP_ZENEMY_H
#define G52CPP_ZENEMY_H

#include "../../header.h"
#include "LivingObject.h"
#include "ZPlayer.h"
#include "../ZUtility/InfoStructs.h"

class AutomatedMovement;

//Another abstract class for enemies specifically
class ZEnemy : public LivingObject {
public:
    ~ZEnemy();
    ZEnemy(ZEngine *pEngine, const string& directoryPath,
           ObjectInfo info, double centerX, double centerY);
    void virtDoUpdate(int iCurrentTime) override;
    void beenHit(int critDistance) override = 0 ; //Keep it virtual to be implemented by specific enemies
    void updateAStar(AStar* newAstar);
protected:
    void drawStatBar(int stat, int barSize, int yOffset, int backgroundColour, int fillColour);
    void setUpImages();
    void virtDraw() override;
    void die() override;
    void initialise();
    //int m_spottingDistance; //Set per enemy, determines when they see the player
    bool m_spotted = false; //once m_spotted, won't give up trying to get to you
    bool attacking = false; //will complete attack before trying to follow you again
    ZPlayer* m_player = nullptr;
    AutomatedMovement* m_movement = nullptr; //Depends on the type of enemy
};


#endif //G52CPP_ZENEMY_H
