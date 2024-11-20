//
// Created by Chris Greer on 02/04/2024.
//

#ifndef G52CPP_PICKUPOBJECT_H
#define G52CPP_PICKUPOBJECT_H

#include "GameObject.h"
#include "../ZUtility/InfoStructs.h"
#include "../../ImageManager.h"
#include "../ZPixels/ImagePixelRepo.h"

//Non-living/moving objects in the game, only have a single image
class PickupObject : public GameObject {

public:
    ~PickupObject() {
        m_image.reset();
        //delete(m_pixelMap);//No one else will
    }
    PickupObject(ZEngine *pEngine,
                 const string& directoryPath,
                 ObjectInfo info,
                 double centerX = 0.5, double centerY = 0.5 )
                     : GameObject(pEngine,info.x,info.y,directoryPath,centerX,centerY)
                     ,m_player(pEngine->getPlayer()){

        m_type = info.type; //Load what type we are <- This is bad OO practice but very small differences
        m_mapFilter = pEngine->getMapFilter().get();
        m_image = make_shared<SimpleImage>(ImageManager::loadImage(m_path, true));
        m_pixelMap = ((*ImagePixelRepo::getSinglePixelMaps()).at(string(1,info.type))).get();
        //static map<string, shared_ptr<PixelMap>>* getSinglePixelMaps(){return m_singlePixelMaps.get();};
        initialiseImages();

    }

    //When updating it just
    void virtDoUpdate(int iCurrentTime) override{
        if (!isVisible() || !isInScreen()) return; //Don't need to worry about

        //Check if they might be colliding
        if (*m_player - *this > getDrawWidth() + 40 || *m_player - *this > getDrawHeight() + 40)
            return;

        //If close enough, check for specific pixel collision
        if (*m_player == *this){
            switch(m_type[0]){
                case 'B':
                    m_player->armor();
                    break;
                case 'R':
                    m_player->reload();
                    break;
                case 'H':
                    m_player->heal();
                    break;
            }
            //Ask the engine to delete us
            dynamic_cast<ZEngine*>(m_pEngine)->addToDelete(this);
        }
    }

    

private:
    ZPlayer* m_player = nullptr;
};

#endif //G52CPP_PICKUPOBJECT_H
