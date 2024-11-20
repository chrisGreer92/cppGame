//
// Created by Chris Greer on 01/04/2024.
//

#include "GameObject.h"
#include "../ZPixels/PixelCollisionUtil.h"

GameObject::GameObject(ZEngine *pEngine, int startX, int startY, std::string  path,
                       double centerOffsetX, double centerOffsetY)
        : DisplayableObject(startX, startY, pEngine,
        0, 0,false ),
          m_path(std::move(path)),
          m_centerOffsetX(centerOffsetX), m_centerOffsetY(centerOffsetY){

    //UNFIX it from the screen... (don't necessarily want everything on the screen)
    m_iCurrentScreenX = startX;
    m_iCurrentScreenY = startY;
}

//Overloading == operator to check if two objects are colliding
bool GameObject::operator==(const GameObject &comp) const {
    return PixelCollisionUtil::checkObjectCollision(this,&comp);
}

//Overloading - operator to calculate the distance to another object
int GameObject::operator-(const GameObject &other) const {
    int deltaX = this->getVirtX() - other.getVirtX();
    int deltaY = this->getVirtY() - other.getVirtY();

    return static_cast<int>(sqrt(deltaX*deltaX + deltaY*deltaY));
}

//Initialises all the image data based on the image that's been set
void GameObject::initialiseImages(){

    setSize(m_image->getWidth(),m_image->getHeight());

    //Create an image map (will always use these since we want to mask pngs
    m_imageMap = new ImagePixelMappingRotateAndColour();
    m_imageMap->setTransparencyColour(0); //Always blank background pngs

    //Set up the centers of the images
    m_imageCenterX = m_centerOffsetX*m_image->getWidth();
    m_imageCenterY = m_centerOffsetY*m_image->getHeight();
    m_imageMap->setRotationCentre(static_cast<int>(m_imageCenterX), static_cast<int>(m_imageCenterY));

    //Account for the image position on the map
    m_iCurrentScreenX -= (int)m_imageCenterX;
    m_iCurrentScreenY -= (int)m_imageCenterY;
}

//Default drawing for game objects
//Uses rotation and masking by default
void GameObject::virtDraw() {

    //If set to not visible or not actually on the screen, don't need to do anything
    if (!isVisible() || !isInScreen()) return;

    //Get location (called method takes account of filtering)
    int drawX = getVirtX();
    int drawY = getVirtY();

    //Then render the image
    m_image->renderImageApplyingMapping(m_pEngine, m_pEngine->getForegroundSurface(),
                                       drawX, drawY,
                                       m_image->getWidth(), m_image->getHeight(),
                                       m_imageMap);


}

//Based on map coordinates, returns whether any of the object is drawn at that location
//Takes into account transparency (i.e. not drawn) and the rotation of the object
bool GameObject::isAtLocation(int xVal, int yVal, bool useDefaultMap) const {

    //First convert the location if we're checking an object that's shifted
    if (m_mapFilter != nullptr){
        xVal = m_mapFilter->filterConvertVirtualToRealXPosition(xVal);
        yVal = m_mapFilter->filterConvertVirtualToRealYPosition(yVal);
    }

    //Set the location within the image draw area (i.e. top left is 0)
    double pixelX = xVal - getRealX();
    double pixelY = yVal - getRealY();

    //If location is outside the image, then can ignore it completely
    if (pixelX < 0 || pixelX > m_image->getWidth() ||
        pixelY < 0 || pixelY > m_image->getHeight())
        return false;

    //Account for rotation
    m_imageMap->mapCoordinates(pixelX,pixelY,*m_image);

    //Check whether that pixel is matching
    return PixelCollisionUtil::checkPixel(useDefaultMap ? m_defaultPixelMap : m_pixelMap,
                                          static_cast<int>(pixelX),
                                          static_cast<int>(pixelY));



}

//Says whether the object is within this (virtual) position on the screen.
//Overridden so we can (potentially) convert to virtual position first
bool GameObject::virtIsPositionWithinObject( int iX, int iY ) {
    if (m_mapFilter != nullptr){
        iX = m_mapFilter->filterConvertVirtualToRealXPosition(iX);
        iY = m_mapFilter->filterConvertVirtualToRealYPosition(iY);
    }
    return DisplayableObject::virtIsPositionWithinObject(iX,iY);
}

//Sets the rotation of the image (Self explanatory)
void GameObject::setRotation(double rotation){
    m_rotateAmount = rotation;
    m_imageMap->setRotation(m_rotateAmount);
}

//If this is an image passing through the filter, then gives virtual position, otherwise real = virtual
int GameObject::getVirtX() const {
    return (m_mapFilter == nullptr) ? m_iCurrentScreenX
        : m_mapFilter->filterConvertRealToVirtualXPosition(m_iCurrentScreenX);
}

//If this is an image passing through the filter, then gives virtual position, otherwise real = virtual
int GameObject::getVirtY() const {
    return (m_mapFilter == nullptr) ? m_iCurrentScreenY
        : m_mapFilter->filterConvertRealToVirtualYPosition(m_iCurrentScreenY);
}
//Self Explanatory, says whether the image us currently on screen
//Accounts for the HUD at the bottom of the screen (hardcoded as won't change)
bool GameObject::isInScreen() const{
    if (getVirtX() > -m_iDrawWidth && getVirtX() < m_pEngine->getWindowWidth() + m_iDrawWidth &&
        getVirtY() > -m_iDrawHeight && getVirtY() < (m_pEngine->getWindowHeight() + m_iDrawHeight) - 100)
            return true;
    return false;
}


