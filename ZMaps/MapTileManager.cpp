//
// Created by Chris Greer on 26/03/2024.
//

#include "MapTileManager.h"
#include "../ZPixels/ImagePixelRepo.h"
#include "../ZPixels/PixelCollisionUtil.h"
//#include "ZEngine.h"

MapTileManager::MapTileManager(ZEngine* pEngine, int tileWidth, int tileHeight)
        : m_pEngine(pEngine),TileManager(tileHeight, tileWidth){

    //Load our tiles image and pixel map
    m_tilesImage = (*ImagePixelRepo::getSingleImages()).at("Tiles");
    m_pixelMap = (*ImagePixelRepo::getSinglePixelMaps()).at("Tiles");
}

void MapTileManager::virtDrawTileAt(BaseEngine *pEngine, DrawingSurface *pSurface,
                                    int iMapX, int iMapY,
                                    int iStartPositionScreenX,
                                    int iStartPositionScreenY) const {

    int mapValue = getMapValue(iMapX,iMapY);
    int xOffset = offsetX(mapValue);
    int yOffset = offsetY(mapValue);

    m_tilesImage->renderImageWithMaskAndTransparency(pSurface,
                                                    xOffset, yOffset,
                                                    iStartPositionScreenX, iStartPositionScreenY,
                                                    m_iTileWidth, m_iTileHeight,
                                                    m_tilesImage->getPixelColour(0, 0), 100);



    //Paint some random details
    vector<shared_ptr<SimpleImage>>* images = (*ImagePixelRepo::getMultiImages()).at("GrassDetails").get();
    if (mapValue == 1){ //For grass, randomly add details
        paintRandomDetail(images,10,pSurface,iStartPositionScreenX,iStartPositionScreenY);
    }// Any other details?

    //Random Blood
    images = (*ImagePixelRepo::getMultiImages()).at("BloodDetails").get();
    if ((mapValue >= 1 && mapValue <= 3) || (mapValue >= 5 && mapValue <= 14) ||
    (mapValue >= 27 && mapValue <= 29) || (mapValue >= 42 && mapValue <= 44))
        paintRandomDetail(images, 7, pSurface, iStartPositionScreenX, iStartPositionScreenY);

    //Random Cracks
    images = (*ImagePixelRepo::getMultiImages()).at("CracksDetails").get();
    if ((mapValue >= 2 && mapValue <= 3) || (mapValue >= 5 && mapValue <= 14) ||
        (mapValue >= 27 && mapValue <= 29) || (mapValue >= 42 && mapValue <= 44))
        paintRandomDetail(images, 4, pSurface, iStartPositionScreenX, iStartPositionScreenY);


}

void MapTileManager::paintRandomDetail(vector<shared_ptr<SimpleImage>>* images, int probability,
                                       DrawingSurface *pSurface,
                                       int xStart,
                                       int yStart) {

    if(rand() % 100 > probability) return;

    SimpleImage* detail = nullptr;
    if (!images->empty()){
        int random = rand() % images->size();
            detail = (*images)[random].get();
    
        //If we'll have details, draw them
        if (detail->getTheData() != nullptr)
            detail->renderImageWithMaskAndTransparency(pSurface,
                                                      0, 0,
                                                      xStart,
                                                      yStart,
                                                      detail->getWidth(), detail->getHeight(),
                                                      0, 100);
        }
}


int MapTileManager::getMapValueForScreenLocation(int xLoc, int yLoc, bool realValues){

    if (!realValues){ //If these are virtual valyes, first convert via the screen
        xLoc = m_pEngine->getMapFilter()->filterConvertVirtualToRealXPosition(xLoc);
        yLoc = m_pEngine->getMapFilter()->filterConvertVirtualToRealYPosition(yLoc);
    }
    return getMapValue(getMapXForScreenX(xLoc),getMapYForScreenY(yLoc));
}

bool MapTileManager::pixelIsAt(int xVal, int yVal, bool realValues) {
    //Which tile
    int mapValue = getMapValueForScreenLocation(xVal, yVal, realValues);

    //Get the specific location in our image that this screen location relates to
    int imageX = offsetX(mapValue) + getXLocationInTile(xVal);
    int imageY = offsetY(mapValue) + getYLocationInTile(yVal);

    //Now we just check if there's a pixel at this specific location in our pixel map
    return PixelCollisionUtil::checkPixel(m_pixelMap.get(),imageX,imageY);
}

//Tells you where (which pixel) in the tile you are (X axis)
int MapTileManager::getXLocationInTile(int virtualX ) {
    int realX = m_pEngine->getMapFilter()->filterConvertVirtualToRealXPosition(virtualX);
    return (realX - m_iBaseScreenX) % getTileWidth();
}
//Tells you where (which pixel) in the tile you are (Y axis)
int MapTileManager::getYLocationInTile(int virtualY ) {
    int realY = m_pEngine->getMapFilter()->filterConvertVirtualToRealYPosition(virtualY);
    return (realY - m_iBaseScreenY) % getTileHeight();
}