//
// Created by Chris Greer on 21/04/2024.
//

#ifndef G52CPP_ZOMBIEFACTORY_H
#define G52CPP_ZOMBIEFACTORY_H

#include "ZEnemy.h"
#include "Zombie.h"
#include "ZArmoured.h"
#include "ZSpeedy.h"
#include "../ZUtility/InfoStructs.h"

//Used to create zombies based on the type passed to it
//Means we can dynamically load zombies from level data and save/load data
//Allows for extension with various enemy types
class ZombieFactory {

public:
    static ZEnemy* createEnemy(ZEngine *pEngine, const ObjectInfo& info) {
        if (info.type == 'Z') {
            return new Zombie(pEngine, "./resources/Enemies/Z1Walk", info, 0.504, 0.628, 3);
        } else if (info.type == 'A') {
            return new ZArmoured(pEngine, "./resources/Enemies/A0Walk", info, 0.512, 0.624, 2);
        }else if (info.type == 'S') {
                return new ZSpeedy(pEngine, "./resources/Enemies/S0Walk", info, 0.496, 0.456, 1);
        } else {
            return nullptr;
        }
        //Can add other enemies
    }

};

#endif //G52CPP_ZOMBIEFACTORY_H
