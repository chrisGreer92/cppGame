//
// Created by Chris Greer on 26/04/2024.
//

#ifndef G52CPP_STATICOBJECTFACTORY_H
#define G52CPP_STATICOBJECTFACTORY_H

#include "PickupObject.h"
#include "../ZEngine.h"
#include "../ZUtility/InfoStructs.h"

class StaticObjectFactory{
public:

    static PickupObject* createObject(ZEngine *pEngine, const ObjectInfo& info) {
        if (info.type == 'H') {
            return new PickupObject(pEngine, "./resources/Pickups/Health.png", info);
        } else if (info.type == 'B') {
            return new PickupObject(pEngine, "./resources/Pickups/Armor.png", info);
        } else if(info.type == 'R'){
            return new PickupObject(pEngine, "./resources/Pickups/Ammo.png", info);
        } else {
            return nullptr;
        }
        //Can add other enemies
    }
};

#endif //G52CPP_STATICOBJECTFACTORY_H
