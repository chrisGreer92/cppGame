//
// Created by Chris Greer on 13/04/2024.
//

#ifndef G52CPP_MATHUTIL_H
#define G52CPP_MATHUTIL_H

#include "../../header.h"

class MathUtil{
public:
    //Template classes that handle various values either floats, doubles or ints (without needing to static cast)
    template<typename T>
    static T distanceBetween(T x1, T y1, T x2, T y2) {

        //Work out the changes on x and y
        T deltaX = x2 - x1;
        T deltaY = y2 - y1;

        //Using pythag return the hypotenuse (i.e. direct distance between)
        return hypotenuse(deltaX,deltaY);
    }
    template<typename T>
    static T hypotenuse(T s1, T s2) {
        //Using pythag return the hypotenuse
        return static_cast<T>(sqrt((s1 * s1) + (s2 * s2)));
    }

    template<typename T>
    //Convert radians to degrees
    static T radToDeg(T radians) {
        return radians * static_cast<T>(180.0) / static_cast<T>(M_PI);
    }

    template<typename T>
    //Convert degrees to radians
    static T degToRad(T degrees) {
        return degrees * static_cast<T>(M_PI) / static_cast<T>(180.0);
    }

    static float fDistanceBetween(int x1, int y1, int x2, int y2){
        int deltaX = x2 - x1;
        int deltaY = y2 - y1;

        //Using pythag return the distance
        return static_cast<float>(sqrt((deltaX * deltaX) + (deltaY * deltaY)));
    }

    //Calculate rotation to point towards a certain point
    static double rotation(int fromX, int fromY, int toX, int toY,  double initialRotation){
        //Check x/y distances between
        int deltaX = toX - fromX;
        int deltaY = toY - fromY;

        //Work out the rotation in radians based on delta values
        double radians = atan2(deltaY, deltaX);
        //This is same as the Drawing Surface version
        //double radians = DrawingSurface::getAngle(fromX,fromY,toX,toY);

        return rotation(radians, initialRotation);
    }
    //Calculate rotation adjusting for initial rotation and the rotation needing to be inverted
    static double rotation(double radians, double initialRotation){

        //Adjust for initial direction (usually downwards)
        radians -= initialRotation;

        //Invert the rotation (goes in opposite direction otherwise)
        radians = 2 * M_PI - radians;

        //Keep rotation within one circle
        if (radians < 0)
            radians += 2 * M_PI;

        return radians;
    }
};

#endif //G52CPP_MATHUTIL_H
