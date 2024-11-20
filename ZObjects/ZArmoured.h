//
// Created by Chris Greer on 14/04/2024.
//

#ifndef G52CPP_ZArmoured_H
#define G52CPP_ZArmoured_H

#include "../../header.h"
#include "ZEnemy.h"
#include "../ZMovement/AutomatedMovement.h"
#include "../ZUtility/InfoStructs.h"

using namespace std;
//Standard ZArmoured Class, zombie with (visible) armour
class ZArmoured : public ZEnemy {
public:
    ZArmoured(ZEngine *pEngine, const std::string& imagePrefix, ObjectInfo info,
           double centerX, double centerY, int totalSkins) :
            ZEnemy(pEngine, imagePrefix, info, centerX, centerY){

        //Slower movement
        //m_movement = make_shared<AutomatedMovement>(this, pEngine, 3, 7);
        m_movement = new AutomatedMovement(this, pEngine, 2, 5);

        //Determine which skin to use
        if (info.typeKey == -1)
            //If it's a random type then choose a random number
            m_type = info.type + to_string(rand() % totalSkins);
        else //Otherwise type is set.
            m_type = info.type + to_string(info.typeKey);
        //Set up the images and pixel maps depending on it's type


        initialise();

    };

    void beenHit(int critDistance) override {

        m_spotted = true; //If they hadn't spotted before, they have now...
        //Determine whether to reduce from armour or health
        int* reduceFrom = (m_armour > 0) ? &m_armour : &m_health;
        //Also determines modifier (armour is harder to damage)
        int modifier = (m_armour > 0) ? 2 : 10;

        if (critDistance <= 14){
            *reduceFrom -= 10 * modifier; //Headshot
        } else if (critDistance <= 24){
            *reduceFrom -= 5 * modifier; //Body shot
        } else{
            *reduceFrom -= 1 * modifier; //Just limbs/hand
        }

        if (m_health <= 0){
            die();
        }
    }

};




#endif //G52CPP_ZArmoured_H
