//
// Created by Chris Greer on 01/04/2024.
//

#ifndef G52CPP_MOVEMENTUTIL_H
#define G52CPP_MOVEMENTUTIL_H


//Moving objects can contain this object
//It helps them work out if they're moving and how much by

#include "../../header.h"
#include "../ZObjects/LivingObject.h"
#include "../ZMaps/MapTileManager.h"
#include "../ZUtility/MathUtil.h"
#include "../ZPixels/PixelCollisionUtil.h"

class MovementUtil {
private:
    //Need to divide speeds by below so that we can accellerate but keep things to lower pixel speeds
    int m_speedAdjustment = 2;

public:
    MovementUtil(LivingObject* object, float acceleration, float maxSpeed, bool isPlayer = false)
    : acceleration(acceleration), m_maxSpeedWalking(maxSpeed), m_isPlayer(isPlayer),
      m_mover(object), m_collisionMap(object->getCollisionMap())
    {}
    ~MovementUtil()= default;

    //Needed as pausing means key up isn't recognised
    void stopMoving(){ m_up = false;m_down = false;m_left = false;m_right = false; }
    void movingUp(bool isMovingUp){ m_up = isMovingUp;}
    void movingDown(bool isMovingDown){ m_down = isMovingDown;}
    void movingRight(bool isMovingRight){ m_right = isMovingRight;}
    void movingLeft(bool isMovingLeft){ m_left = isMovingLeft;}
    void running(bool isRunning){ m_running = isRunning;}

    bool updateMovement(int &locationX, int &locationY){

        if (m_mover == nullptr) return false;

        if (!m_mover->isVisible()) return false;

        if (!m_left && !m_right && !m_up && !m_down) return false;

        //If running, set max speed to higher
        maxSpeed = m_running ? m_maxSpeedWalking + 5 : m_maxSpeedWalking;

        updateVelocities(); //Update velocities based on current directions

        if(!calculateSpeed()) //Check if moving and normalise velocities
            return false; //No Speed

        return tryToMove(locationX, locationY); //Try to move and return result

    }

protected:
    void updateVelocities(){
        //Work out whether the player is moving, if so determine which directions
        //If moving in that direction, first set opposite to zero
        //Then, add to velocity in that direction up to the maximum speed
        if (m_up)
            velocityY = (velocityY > 0) ? 0 : max(velocityY - acceleration, -maxSpeed);
        if (m_down)
            velocityY = (velocityY < 0) ? 0 : min(velocityY + acceleration, maxSpeed);
        if (m_right)
            velocityX = (velocityX < 0) ? 0 : min(velocityX + acceleration, maxSpeed);
        if (m_left)
            velocityX = (velocityX > 0) ? 0 : max(velocityX - acceleration, -maxSpeed);

        //Did try adding deceleration but it felt LESS natural
        //So in the end can come to hard stops (also above when changing direction)
        if(!m_up && !m_down)
            velocityY = 0;
        if(!m_right && !m_left)
            velocityX = 0;

    }


    bool calculateSpeed() {

        float speed = MathUtil::hypotenuse(velocityX,velocityY);

        //If we're moving at all then do some updating
        if(speed > 0) {

            //Limit the maximum speed
            if (speed > maxSpeed) {
                velocityX *= maxSpeed / speed;
                velocityY *= maxSpeed / speed;
            }

            return true;
        }
        return false;
    }


    bool tryToMove(int &locationX, int &locationY){

        //Adjust for high velocity
        int adjustmentX = static_cast<int>(velocityX) / m_speedAdjustment;
        int adjustmentY = static_cast<int>(velocityY) / m_speedAdjustment;

        //If we're not really moving, don't need to check anything.
        if (adjustmentX == 0 && adjustmentY == 0) return false;
        //Check if you'd be colliding
        if(PixelCollisionUtil::checkTileCollision(m_collisionMap, m_mover,
                                                  adjustmentX, adjustmentY, m_isPlayer)){
            //Would collide!
            return false;
        }
        //Otherwise move!
        locationX += adjustmentX;
        locationY += adjustmentY;

        return true;
    }

protected:
    MapTileManager* m_collisionMap = nullptr;
    LivingObject* m_mover = nullptr;
    bool m_isPlayer = false;
    bool m_running = false;
    bool m_left = false;
    bool m_right = false;
    bool m_up = false;
    bool m_down = false;
private: //(only for working out the movement itself)
    float velocityX = 0;
    float velocityY = 0;
    float maxSpeed = 0;
    float m_maxSpeedWalking =  0;
    float acceleration = 0;
};

#endif //G52CPP_MOVEMENTUTIL_H