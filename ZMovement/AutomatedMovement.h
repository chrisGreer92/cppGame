//
// Created by Chris Greer on 04/04/2024.
//

#ifndef G52CPP_AUTOMATEDMOVEMENT_H
#define G52CPP_AUTOMATEDMOVEMENT_H

#include "../../header.h"
#include "../ZEngine.h"
#include "MovementUtil.h"
#include "../ZUtility/MathUtil.h"
#include "AStar.h"
#include "../ZPixels/RayTrace.h"

//Handles enemy movement including the call to our A* Algorithm
class AutomatedMovement : public MovementUtil {

public:
    AutomatedMovement(LivingObject* mover, ZEngine * pEngine, float acceleration, float maxSpeed)
            : MovementUtil(mover, acceleration, maxSpeed), m_pEngine(pEngine), m_aStar(pEngine->getAStar())
    {}

    void updateAStar(AStar* newAstar) {
        m_aStar = newAstar;
    }

    //Virtual so the logic can be changed for more complicated movement
    virtual bool automateMovement(int &pCurrentX, int &pCurrentY){

        //If we're not following a path and haven't been blocked (yet)
        //Just need to check if we have line of sight to move directly
        if (!m_followingPath && !m_blocked) {
            //Check if we have line of sight, if so will move directly
            if (RayTrace::lineOfSightToPlayer(m_pEngine,
                                              m_mover->getExactRealCenterX(),
                                              m_mover->getExactRealCenterY())) {
                //Try to move directly towards the player
                if (moveDirectly(m_mover->getExactRealCenterX(),
                                 m_mover->getExactRealCenterY(),
                                 m_pEngine->getPlayerCoords().x,
                                 m_pEngine->getPlayerCoords().y,
                                 pCurrentX, pCurrentY)){
                    return true; //Have LOS and can move directly
                }
            }
            //If no line of sight or blocked along a direct path then report that we're blocked
            return !(m_blocked = true);

            //If we're not (yet) following a path but we are blocked
            //Just need to work out a new path
        }else if(!m_followingPath && m_blocked){
            m_blocked = false;
            createGoal(); //Just create regular goal and try to move towards it
            m_followingPath = true;
            if (moveTowardsGoal(pCurrentX, pCurrentY)){
                return (m_followingPath = true);
            }else {
                //If we get blocked straight away then likely encountered an awkward corner
                return !(m_blocked = true); //Otherwise we're blocked
            }
        } else if(m_followingPath && !m_blocked){
            //If we're not blocked and have a goal, just try to move towards it
            if (moveTowardsGoal(pCurrentX, pCurrentY)){
                return true;
            } else { //We've either reached our goal or hit an obstacle
                m_followingPath = false; //Will need to work out a new path
                return false; //Stopped moving
            }
        } else {
            //If we're blocked but may have a goal!
            m_blocked = false;
            createGoal(true); //Create a new goal, avoiding blockages
            return moveTowardsGoal(pCurrentX, pCurrentY);
        }

    }

    //Use our A* implementation to work out which tile to aim for
    AStar::aNode* calculateNodeGoal(int currentX, int currentY){

        //Solve the path towards the player using our A* implementation
        AStar::aNode * node = m_aStar->solvePath(
                currentX,currentY,
                m_pEngine->getPlayerCoords().x,
                m_pEngine->getPlayerCoords().y);

        if (!node) return nullptr;
        //Update our current point so it's from the center of the current tile they're on
        currentX = m_aStar->tileToLoc(node->tileX);
        currentY = m_aStar->tileToLoc(node->tileY);

        //Will track the angle to move at
        //Traverse the path to determine a goal to reach before checking again
        //This is determined by if direction needs to change
        double angle = -1;
        while(node->parent){
            //Get the parent node
            AStar::aNode * parent = node->parent;

            //Determine the location it represents
            int parentX = m_aStar->tileToLoc(parent->tileX);
            int parentY = m_aStar->tileToLoc(parent->tileY);

            //Determine the angle towards this point
            double tempAngle = DrawingSurface::getAngle(currentX,currentY,parentX,parentY);

            //If the angle has changed, then exit and use the node we had previously as a local goal
            if (angle != -1 && angle != tempAngle){
                return node;
            }
            angle = tempAngle;
            //Otherwise keep moving along the nodes
            node = node->parent;

            //Check if we can see the player at this next node
            if (RayTrace::lineOfSightToPlayer(dynamic_cast<ZEngine *>(m_mover->getEngine()),
                                              m_aStar->tileToLoc(node->tileX),
                                              m_aStar->tileToLoc(node->tileY))){
                //If so, then we can stop at that point and just move directly
                return node;
            }

        }
        return node;
    }

    //Work out a new goal to move towards
    void createGoal(bool avoidBlockage = false){

        int currentX = m_mover->getExactRealCenterX();
        int currentY = m_mover->getExactRealCenterY();

        AStar::aNode* nodeGoal = calculateNodeGoal(currentX, currentY);

        if (!nodeGoal) return;

        //The node we have represents our goal tile to move towards before checking path again
        //Work out the x and y distances to the goal
        int deltaX = m_aStar->tileToLoc(nodeGoal->tileX) - currentX;
        int deltaY = m_aStar->tileToLoc(nodeGoal->tileY) - currentY;

        // Make sure we only go in one direction
        //If we're trying to avoid a blockage, go in the non-dominating direction
        if (abs(deltaX) <= abs(deltaY)) {
            deltaX = avoidBlockage ? deltaX : 0;
            deltaY = avoidBlockage ? 0 : deltaY;
        } else {
            deltaX = avoidBlockage ? 0 : deltaX;
            deltaY = avoidBlockage ? deltaY : 0;
        }

        //Create a buffer of one tile (this ensures you completely clear any corners
        int bufferSize = m_pEngine->getTileSize()/4;
        int bufferX = deltaX > 0 ? bufferSize : -bufferSize;
        int bufferY = deltaY > 0 ? bufferSize : -bufferSize;

        //For the chosen direction, set the goal, for the other direction our current position is the goal
        m_localGoalX = (deltaX == 0) ? currentX : m_aStar->tileToLoc(nodeGoal->tileX) + bufferX;
        m_localGoalY = (deltaY == 0) ? currentY :m_aStar->tileToLoc(nodeGoal->tileY) + bufferY;
    }

    //Move towards our determined goal
    bool moveTowardsGoal(int &pCurrentX, int &pCurrentY){

        //Store our current location
        int currentX = m_mover->getExactRealCenterX();
        int currentY = m_mover->getExactRealCenterY();

        int deltaX = m_localGoalX - currentX;
        int deltaY = m_localGoalY - currentY;

        //If still moving towards original goal, don't try to solve again
        if ((abs(deltaX) > m_pEngine->getTileSize()/8 ||
             abs(deltaY) > m_pEngine->getTileSize()/8) && //Buffer (close enough to goal)
            m_localGoalX != -1 && m_localGoalY != -1) {

            //Adjust so we only move in one direction (avoids clipping)
            if (abs(deltaX) <= abs(deltaY)) {
                deltaX = 0;
            } else {
                deltaY = 0;
            }

            return moveDirectly(deltaX, deltaY, pCurrentX, pCurrentY);
        }
        return false; //Reached our goal
    }

    //Moves directly towards one point based on starting point
    bool moveDirectly(int xFrom, int yFrom, int xTo, int yTo, int &pCurrentX, int &pCurrentY ){

        int deltaX = xTo - xFrom;
        int deltaY = yTo - yFrom;

        return moveDirectly(deltaX, deltaY, pCurrentX, pCurrentY);

    }

    //Moves directly towards a point based on delta values
    bool moveDirectly(int deltaX, int deltaY, int &xLocation, int &yLocation){

        double angle = atan2(deltaY,deltaX);

        //Using degrees makes it easier to visualise the cone
        //Gives between -180 and 180, downwards is 0 degrees...
        double degrees = angle * (180.0 / M_PI);
        degrees = fmod(degrees + 180.0, 360.0) - 180.0; //Keep within range
        //Based on the angle determine directions
        setConeDirection(degrees);

        //Try to move in set direction(s)
        if (updateMovement(xLocation, yLocation)) {
            //If we are moving then rotate in that that direction
            double angle = atan2(deltaY, deltaX);

            double originalRotation = m_mover->getRotation();
            //Set rotation towards the direction
            m_mover->setRotation(MathUtil::rotation(angle, m_mover->getInitialRotation()));
            //Make sure we're not now colliding with a tile
            if (PixelCollisionUtil::checkTileCollision(m_mover->getCollisionMap(), m_mover))
                m_mover->setRotation(originalRotation);
            //If so change our rotation back (will still move at that angle though for now)
            return true;
           
        } else return false;

    }

private:

    void setConeDirection(double degrees){
        //Try to move in that direction within overlapping cones
        //Makes movement look more natural than almost always moving at 45 degrees
        int halfCone = 75;
        m_right =   (degrees > 0 - halfCone && degrees < 0 + halfCone);
        m_left =    (degrees > 180 - halfCone || degrees < -180 + halfCone); //Special case since it flips from +ve to -ve
        m_down =    (degrees > 90 - halfCone && degrees < 90 + halfCone);
        m_up =      (degrees > -90 - halfCone && degrees < -90 + halfCone);
    }
private:
    ZEngine* m_pEngine;
    AStar* m_aStar;
    int m_localGoalX = -1;
    int m_localGoalY = -1;
    bool m_followingPath = false;
    bool m_blocked = false;

};


#endif //G52CPP_AUTOMATEDMOVEMENT_H
