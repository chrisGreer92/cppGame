// Created by Chris Greer on 01/04/2024.
//
#ifndef G52CPP_GAMEOBJECT_H
#define G52CPP_GAMEOBJECT_H


#include "../../DisplayableObject.h"
#include "../../header.h"
#include "../../DrawingSurface.h"
#include "../../SimpleImage.h"
#include "../../ImagePixelMapping.h"
#include "../ZEngine.h"
#include "../ZMaps/MapOffsetFilter.h"
#include "../ZPixels/PixelMapCreator.h"
//#include <utility>
//#include "PixelCollisionUtil.h"

using namespace std;

//class DrawingSurface;

//ABSTRACT GameObject class, needs to be implemented (specifically the image loading)
//Made abstract by setImage() method
class GameObject : public DisplayableObject{
public:
    ~GameObject() {
        delete(m_imageMap);
        m_imageMap = nullptr;
        m_image.reset();
    }
    GameObject(ZEngine *pEngine,
               int startX, int startY,
               string  path,
               double centerOffsetX = 0.5, double centerOffsetY = 0.5);

    //Overloading == operator to check if pixels are colliding
    bool operator==(const GameObject& comp) const;

    //Overloading - operator to calculate the distance to another object
    int operator-(const GameObject& other) const;

    //To ensure this is a virtual class
    void virtDoUpdate(int iCurrentTime) override = 0;

    //Default drawing for game objects
    //Uses rotation and masking by default
    void virtDraw() override;
    //Returns this objects mapOffset filter (if it has one)
    MapOffsetFilter* getMapFilter(){ return m_mapFilter; }

    int getRealX() const { return m_iCurrentScreenX;}
    int getRealY() const { return m_iCurrentScreenY;}
    //If this is an image passing through the filter, then gives virtual position, otherwise real = virtual
    int getVirtX() const;
    //If this is an image passing through the filter, then gives virtual position, otherwise real = virtual
    int getVirtY() const;
    //Based on map coordinates, returns whether any of the object is drawn at that location using our pixel map
    //Takes into account transparency (i.e. not drawn) and the rotation of the object
    //Can set to check whether to use default pixel map (avoids flailing arms causing issues)
    bool isAtLocation(int xVal, int yVal, bool useDefaultMap = false) const;

    //Tells you whether the object is currently on screen
    //Accounts for the HUD at the bottom of the screen
    bool isInScreen() const;

    //Says whether the object is within this (virtual) position on the screen.
    //Overridden so we can (potentially) convert to virtual position first
    virtual bool virtIsPositionWithinObject( int iX, int iY ) override;

    //I would have overriden get center but wasn't possible, Instead these get the exact centre
    int getExactRealCenterX() const{ return static_cast<int>(m_iCurrentScreenX + m_imageCenterX); }
    int getExactRealCenterY() const{ return static_cast<int>(m_iCurrentScreenY + m_imageCenterY); }
    int getExactVirtCenterX() const{ return static_cast<int>(getVirtX() + m_imageCenterX); }
    int getExactVirtCenterY() const{ return static_cast<int>(getVirtY() + m_imageCenterY); }

    //Used to set the rotation via ImagePixelMappingRotateAndColour class
    void setRotation(double rotation);
    //Used to determine initial rotation which we set
    double getInitialRotation() const { return m_initialRotation; }
    //To get the current rotation
    double getRotation() const { return m_rotateAmount; }

    string getType() const { return m_type;}


protected:
    //Different implementations need to set this differently
    //virtual shared_ptr<SimpleImage> setImage() = 0; //So use virtual function
    //Image to render
    shared_ptr<SimpleImage> m_image = nullptr;
    //The path to either the image, or directory of images
    //Must hardcode this in the subclasses classes
    string m_path;
    //Calling classes must also use initialise after running setImage in their constructor
    void initialiseImages();
    //May need a filter to offset in map
    MapOffsetFilter* m_mapFilter = nullptr;
    //Rotation and masking Related
    ImagePixelMappingRotateAndColour* m_imageMap = nullptr;
    //The current pixel map to use for collision detection
    PixelMap* m_pixelMap = nullptr;
    //Our default map for checking barrier collisions
    //Avoids animations suddenly meaning you ARE colliding with something
    PixelMap* m_defaultPixelMap = nullptr;
    //By default all my images point downwards
    //May need to be changed for other images for extension
    double m_initialRotation = (M_PI / 2.0);
    double m_rotateAmount = 0;
    //Image center values, set within subclasses
    double m_imageCenterX = 0;
    double m_imageCenterY = 0;
    double m_centerOffsetX = 0;
    double m_centerOffsetY = 0;
    string m_type; //Determines the type of object, used for saving and loading
};


#endif //G52CPP_GAMEOBJECT_H