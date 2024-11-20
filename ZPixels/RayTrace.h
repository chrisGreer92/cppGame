//
// Created by Chris Greer on 05/04/2024.
//

#ifndef G52CPP_RAYTRACE_H
#define G52CPP_RAYTRACE_H

#include "../../header.h"
#include "../ZObjects/GameObject.h"
#include "../ZEngine.h"
#include "../ZMaps/MapOffsetFilter.h"
#include "PixelMapCreator.h"
#include "../ZUtility/MathUtil.h"
#include "../ZObjects/ZEnemy.h"
#include "../ZUtility/TileCodes.h"

//Class to handle the ray tracing logic in the game
//Includes the line of sight of the player (towards any enemies/obstacles)
//And line of sight from any point to the player
class RayTrace {

public:

    static void playerLineOfSight(GameObject *player, ZEngine* pEngine, int xOffset, int yOffset ){

        //First declare that player is not going to hit anything yet
        //Will get set (again) IF an enemy is in the firing range.
        pEngine->setInSights({nullptr,0});

        auto mouseX = static_cast<float>(pEngine->getCurrentMouseX());
        auto mouseY = static_cast<float>(pEngine->getCurrentMouseY());

        auto objX = static_cast<float>(player->getExactRealCenterX());
        auto objY = static_cast<float>(player->getExactRealCenterY());
        auto rotation = static_cast<float>(player->getRotation());

        float mouseToCenterDistance = MathUtil::distanceBetween(mouseX,mouseY,
                                                                objX,objY);
        //Work out the distance to the offset point, will also be our radius
        auto offsetDistance = static_cast<float>(sqrt(xOffset * xOffset + yOffset * yOffset));

        //If mouse is between offset (plus buffer) and the player then don't do anything
        if (mouseToCenterDistance <= offsetDistance + 54) return;

        //Work out the angle between the line that goes to xOffset and to the edge of our circle
        float angleBetween = acos(static_cast<float>(xOffset)/offsetDistance);

        //Work out the rotation from the center based on rotate amount and the offset
        float referencePointRotation = -rotation - angleBetween;

        //Make sure we're within 1 circle
        referencePointRotation = static_cast<float>(fmod(referencePointRotation + M_PI, 2 * M_PI) - M_PI);
        if (referencePointRotation < 0)
            referencePointRotation += 2 * M_PI;
        referencePointRotation -= M_PI;

        //Work out the coordinates around the offset circle away from the center
        float referencePointOffsetX = offsetDistance * cos(referencePointRotation);
        float referencePointOffsetY = offsetDistance * sin(referencePointRotation);

        //With the relative location, work out the exact location based on the center of the object
        float offsetX = objX + referencePointOffsetX;
        float offsetY = objY + referencePointOffsetY;

        auto radAngle = static_cast<float>(DrawingSurface::getAngle(
                                        offsetX, offsetY,
                                        mouseX, mouseY));

        float length = playerLosLength(pEngine, offsetX, offsetY, radAngle);

        //Work out where our line goes to
        float lineX = length * cos(radAngle);
        float lineY = length * sin(radAngle);

        pEngine->getForegroundSurface()->drawLine(offsetX, offsetY,
                                                  (offsetX + lineX), (offsetY + lineY),
                                                  0x30D5C8);
    }

    static float playerLosLength(ZEngine* pEngine, float pointX, float pointY, float angle){

        float directionX = cos(angle);
        float directionY = sin(angle);
        //Quite a large step size, can reduce this to improve accuracy but slows performance
        float stepSize = 7;
        float length = 0;
        MapTileManager* collisionMap = pEngine->getCollisionMap().get();

        //Move along the line and return length once you hit an obstacle OR reach the edge
        while(true){
            length += stepSize;
            int xVal = static_cast<int>(pointX + (length * directionX));
            int yVal = static_cast<int>(pointY + (length * directionY));

            if (xVal < 0 || yVal < 0 || xVal > pEngine->getWindowWidth() || yVal > pEngine->getWindowHeight())
                return length; //We've hit the edge

            //Check for each non-player object (will have to change once non-living objects exist)
            //Actually range will be correct... Still check
            int i = 1;
            while(true){
                DisplayableObject* object = pEngine->getDisplayableObject(i++);
                if (!object)  break; //Have reached the end of the array.
                auto* enemy = dynamic_cast<ZEnemy *>(object);
                if (!enemy) continue ;//If dynamic cast didn't work

                //Check if Object is visable, on screen and alive
                if (!enemy->isVisible() || enemy->isDead() || !enemy->isInScreen())
                    continue;
                //Check if it's in the drawing area of the object at all
                if (enemy->virtIsPositionWithinObject(xVal, yVal)){
                    //If so, check the pixel map to see if we're actually hitting the drawn object
                    if(enemy->isAtLocation(xVal, yVal)){
                        //Determine how close that location is to the center of the player
                        int distance = MathUtil::distanceBetween(xVal, yVal,
                                                                 enemy->getExactVirtCenterX(),
                                                                 enemy->getExactVirtCenterY());
                        //Finally, tell the engine that the player is aiming at something
                        pEngine->setInSights({enemy , distance});
                        return length;
                    }
                }
            }

            //Lastly check if colliding with any obstacles
            int mapValue = collisionMap->getMapValueForScreenLocation(xVal, yVal);

            //Potentially colliding with a barrier, (not including edge barriers)
            if (TileCodes::isLosBlockingTile(mapValue)){
                //Now check if that specific pixel is filled at that location
                if (collisionMap->pixelIsAt(xVal,yVal))
                    return length;
                //Otherwise doesn't count
            }

        }
    }


    //Can use enemies center (head) to check whether they have a clear line of sight to the player
    //Will check if ANY blocking tiles in the way, even if only partially (As want to move around them)
    //Work also out if any point in the map has line of sight to the player
    //This is used to determine line of sight along a shortest path to check whether
    //It's necessary to keep iterating through the path or if you can just go directly towards player at that point
    static bool lineOfSightToPlayer(ZEngine* pEngine, int fromX, int fromY){

        int playerX = pEngine->getPlayerCoords().x;
        int playerY = pEngine->getPlayerCoords().y;

        int targetDistance = MathUtil::distanceBetween(fromX, fromY, playerX, playerY);

        //Get the angle between them
        auto radAngle = static_cast<float>(DrawingSurface::getAngle(
                fromX, fromY,
                playerX, playerY));

        //Split directions into x and y parts
        float directionX = cos(radAngle);
        float directionY = sin(radAngle);

        //Can use a much larger step size since we only care about the tiles we pass over
        //(not whether we hit the exact first point of the player closest)
        int stepSize = 44;
        int length = 0;

        MapTileManager* collisionMap = pEngine->getCollisionMap().get();
        //Move along the line and return true if we can reach the player without hitting an obstacle
        while(length < targetDistance) {
            length += stepSize;
            int xVal = fromX + static_cast<int>(directionX * static_cast<float>(length));
            int yVal = fromY + static_cast<int>(directionY * static_cast<float>(length));

            //Check if direct path to player crosses any obstacle tiles
            int mapValue = collisionMap->getMapValueForScreenLocation(xVal, yVal, true);
            if (TileCodes::isLosBlockingTile(mapValue)) return false;
            //Don't need to check if specific pixel is filled at that location
        }
        //If gone all the way to the player without obstacle, then have line of sight
        return true;
    }


};

#endif //G52CPP_RAYTRACE_H
