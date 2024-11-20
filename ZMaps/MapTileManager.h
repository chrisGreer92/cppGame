//
// Created by Chris Greer on 26/03/2024.
//

#ifndef G52CPP_MAPTILEMANAGER_H
#define G52CPP_MAPTILEMANAGER_H

#include "../../header.h"
#include "../ZPixels/PixelMapCreator.h"
#include "../../TileManager.h"
#include "../../ImagePixelMapping.h"
#include "../../BaseEngine.h"

class ZEngine;

class MapTileManager : public TileManager {
public:
    MapTileManager(ZEngine* pEngine, int tileWidth, int tileHeight);
//
//    ~MapTileManager() override{
//        //TileManager::~TileManager();
//        return;
//    }

    //Overiden version, uses our tiles image and draws from that depending on map value
    void virtDrawTileAt(BaseEngine *pEngine, DrawingSurface *pSurface,
                        int iMapX, int iMapY,
                        int iStartPositionScreenX, int iStartPositionScreenY) const override;

    //Directly get the map/tile value for either a real or virtual screen location
    int getMapValueForScreenLocation(int xLoc, int yLoc, bool realValues = false);

    //Tells you where the tile is in our image (X axis)
    int offsetX(int mapValue) const { return (mapValue % 10) * m_iTileWidth; }
    //Tells you where the tile is in our image (Y axis)
    int offsetY(int mapValue) const { return (mapValue / 10) * m_iTileHeight;}
    //Tells you whether there's filled pixels at that exact location
    bool pixelIsAt(int xVal, int yVal, bool realValues = false);
    //Tells you where (which pixel) in the tile you are (X axis)
    int getXLocationInTile(int virtualX );
    //Tells you where (which pixel) in the tile you are (Y axis)
    int getYLocationInTile(int virtualY );

private:
    ZEngine* m_pEngine;
    shared_ptr<SimpleImage> m_tilesImage;
    shared_ptr<PixelMap> m_pixelMap;

    //Used to paint random details on chosen tiles given a specific probability (percent)
    static void paintRandomDetail(vector<shared_ptr<SimpleImage>>* images, int probability,
                                  DrawingSurface *pSurface,
                                  int xStart, int yStart) ;
};


#endif //G52CPP_MAPTILEMANAGER_H
