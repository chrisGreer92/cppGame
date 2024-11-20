//
// Created by Chris Greer on 14/04/2024.
//

#ifndef G52CPP_ZOMBIE_H
#define G52CPP_ZOMBIE_H

#include "../../header.h"
#include "ZEnemy.h"
#include "../ZMovement/AutomatedMovement.h"
#include "../ZUtility/InfoStructs.h"

using namespace std;
//Standard Zombie Class
class Zombie : public ZEnemy {
public:
    Zombie(ZEngine *pEngine, const std::string& imagePrefix, ObjectInfo info,
           double centerX, double centerY, int totalSkins) :
            ZEnemy(pEngine, imagePrefix, info, centerX, centerY){

        m_movement = new AutomatedMovement(this, pEngine, 2.0f, 6.0f);

        //Determine which skin to use
        if (info.typeKey == -1)
            //If it's a random type then choose a random number
            m_type = info.type + to_string(rand() % totalSkins);
        else //Otherwise type is set.
            m_type = info.type + to_string(info.typeKey);

        initialise();

    };

    void beenHit(int critDistance) override {

        //Reduce directly from health
        if (critDistance <= 14){
            m_health -= 100; //Headshot
        } else if (critDistance <= 24){
            m_health -= 50; //Body shot
        } else{
            m_health -= 10; //Just limbs/hand
        }

        if (m_health <= 0){
            die();
        }
    }

};




#endif //G52CPP_ZOMBIE_H
