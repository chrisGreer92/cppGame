//
// Created by Chris Greer on 14/04/2024.
//

#ifndef G52CPP_ZSpeedy_H
#define G52CPP_ZSpeedy_H

#include "../../header.h"
#include "ZEnemy.h"
#include "../ZMovement/AutomatedMovement.h"
#include "../ZUtility/InfoStructs.h"

using namespace std;
//Standard ZSpeedy Class, much faster zombie
class ZSpeedy : public ZEnemy {
public:
    ZSpeedy(ZEngine *pEngine, const std::string& imagePrefix, ObjectInfo info,
           double centerX, double centerY, int totalSkins) :
            ZEnemy(pEngine, imagePrefix, info, centerX, centerY){

        m_movement = new AutomatedMovement(this, pEngine, 4, 10);
        //m_movement = make_shared<AutomatedMovement>(this, pEngine, 6, 10);

        //Only has 1 skin
        m_type = "S0";

        initialise();

    };

    void beenHit(int critDistance) override {

        //Reduce directly from health
        if (critDistance <= 22){
            m_health -= 100; //Head/Bodyshot
        } else{
            m_health -= 34; //Just limbs/hand
        }

        if (m_health <= 0){
            die();
        }
    }

};




#endif //G52CPP_ZSpeedy_H
