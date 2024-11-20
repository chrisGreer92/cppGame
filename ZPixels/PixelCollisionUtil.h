//
//
// Created by Chris Greer on 08/04/2024.
//

#ifndef G52CPP_PIXELCOLLISIONUTIL_H
#define G52CPP_PIXELCOLLISIONUTIL_H

#include "PixelMapCreator.h"
#include "../ZMaps/MapTileManager.h"
#include "../ZEngine.h"
#include "../ZUtility/TileCodes.h"
#include "../ZObjects/ZEnemy.h"
#include "ImageLoader.h"

//Class that handles checking for pixel perfect collision
class PixelCollisionUtil{

public:
    //Checks collision with the player and ALL enemies
    //Used for player melee
    static void checkAllEnemyCollisions(GameObject* player, ZEngine *pEngine){

        //Iterate through the enemy objects to see if any collisions occurred
        int i = 0;
        while(true){
            DisplayableObject* object = pEngine->getDisplayableObject(i++);
            if (!object)  break; //Have reached the end of the array.
            auto* target = dynamic_cast<ZEnemy *>(object);
            if(!target) //First check the cast worked
                continue;
            //Then check if target is still alive, visable and on screen
            if (!target->isVisible() || target->isDead() || !target->isInScreen())
                continue;
            //Then check if they're close enough that they could concievably collide
            if (*player - *target > player->getDrawWidth() || *player - *target > player->getDrawHeight())
                continue;

            //If close enough, check for specific pixel collision
            if (*player == *target){
                target->beenHit(11); //Bats may hit multiple times in one swing (up to 3)
            }
//            if(checkObjectCollision(player, target)){
//            }

        }

    }

    //Checks if two objects' pixel maps are colliding
    static bool checkObjectCollision(const GameObject* checker, const GameObject* target){

        //First get checking object's (virtual) location on the map
        int attX = checker->getVirtX();
        int attY = checker->getVirtY();

        //Now, iterate over the checking object's draw area and check
        //If there are any points where they are both drawn
        for (int x = attX; x < attX + checker->getDrawWidth(); ++x) {
            for (int y = attY; y < attY + checker->getDrawWidth(); ++y) {
                if (checker->isAtLocation(x, y) && target->isAtLocation(x, y)){
                    return true;
                }
            }
        }

        return false;
    }

    //Returns whether a living object is colliding with a tile
    //If checking for player we want this to be pixel perfect
    //For enemies we only care about whether they (would be) on the tile
    static bool checkTileCollision(MapTileManager* tileMap, LivingObject *object,
                                   int adjX = 0, int adjY = 0, bool isPlayer = true){


        //Need to check whether the object is over any drawn point in the tile
        //First get objects (virtual) location on the map plus any adjustment
        int objX = object->getVirtX() + adjX;
        int objY = object->getVirtY() + adjX;

        //Now, iterate over the attackers draw area and check
        //If there are points where both are drawn
        for (int x = objX; x < objX + object->getDrawWidth(); ++x) {
            for (int y = objY; y < objY + object->getDrawWidth(); ++y) {
                if (object->isAtLocation(x - adjX,y - adjY, true)){ //Check at original location
                    //Check tile value for location

                    int mapValue = tileMap->getMapValueForScreenLocation(x, y);
                    if(TileCodes::isCollisionTile(mapValue)){
                        if (!isPlayer) //If it's not the player we only care about being on the tile
                            return true;
                        //For player check if actually drawn there
                        if (tileMap->pixelIsAt(x,y))
                            return true;
                    }
                }
            }
        }

        return false;
    }

    //Check if a specified pixel is set in the map (need to pass it values that account for screen location)
    static bool checkPixel(PixelMap* pixelMap, int x, int y) {

        //First check if it's in bounds
        if ( x < 0 || x >= pixelMap->at(0).size() || y < 0 || y >= pixelMap->size()) {
            return false;
        }

        //Then check if that pixel is set or not
        //return (*pixelMap)[y][x] != 0;
        return pixelMap->at(y)[x];
    }
};

#endif //G52CPP_PIXELCOLLISIONUTIL_H
