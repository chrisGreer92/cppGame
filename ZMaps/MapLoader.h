//
// Created by Chris Greer on 01/04/2024.
//

#ifndef G52CPP_MAPLOADER_H
#define G52CPP_MAPLOADER_H


#include <fstream>
#include "../../DrawingSurface.h"
#include "../../header.h"
#include "MapTileManager.h"
#include "../ZPixels/ImagePixelRepo.h"
#include "../ZEngine.h"
#include "../ZUtility/InfoStructs.h"
#include "../ZUtility/TileCodes.h"

using namespace std;

class MapLoader {

public:

    //Loads the map of locations of objects and stores them for use elsewhere
    static void loadObjectTileMap(ZEngine *pEngine, const string &mapPath){

        vector<ObjectInfo> objectInfo;
        vector<KeyTile> keyTiles = vector<KeyTile>();
        ifstream mapDoc;
        //std::basic_fstream<char> mapDoc;
        mapDoc.open(mapPath);
        int mapX,mapY;
        int tileSize = pEngine->getTileSize();

        //First add our player coords from top of the mapDoc
        string line;
        getline(mapDoc, line);
        if(!line.empty()) {
            if (sscanf(line.c_str(), "%d %d", &mapX, &mapY) == 2) {
                objectInfo.push_back({mapX * tileSize, mapY * tileSize, 'P', -1, 60, 0});
            }
        }

        int k = 0;
        //Scan the Main Unlocking Tiles
        while (mapDoc.peek() == 'K') { //Pulls in the tiles and their related info
            KeyTile tile(0,0);
            mapDoc >> tile;
            keyTiles.push_back(tile);
            k++;
        }
        pEngine->setTotalKeys(k); //Tell the engine how many keys this level

        //Then scan the tiles and their relevant keys
        while (mapDoc.peek() == 'T') { //Pulls in the tiles and their related info
            KeyTile tile(0,0);
            mapDoc >> tile;
            keyTiles.push_back(tile);

        }

        //Then get all the enemies/objects depending on their position
        for (int y = 0; y < pEngine->getTilesY(); ++y) {
            for (int x = 0; x < pEngine->getTilesX(); ++x) {

                int mapValue = getMapValue(mapDoc);
                int xCoord = 0;
                int yCoord = 0;
                if (mapValue != 0){ // Something to add, work out coords
                    xCoord = x*tileSize + (tileSize/2);
                    yCoord = y*tileSize + (tileSize/2);
                }
                if (TileCodes::zombie(mapValue)){ //Zombie
                    objectInfo.push_back({xCoord, yCoord,
                                          'Z', -1, 100, 0});//Initialise type randomly
                } else if(TileCodes::armouredZombie(mapValue)) { //Armoured Zombie
                    objectInfo.push_back({xCoord, yCoord,
                                          'A', -1, 100, 100});
                } else if(TileCodes::speedy(mapValue)){ //Armoured Zombie
                        objectInfo.push_back({xCoord, yCoord,
                                              'S', -1, 100, 0}); //Speedy zombie
                } else if (TileCodes::armorItem(mapValue)){ //Body Armour
                    objectInfo.push_back({xCoord, yCoord,'B'});
                } else if (TileCodes::ammoItem(mapValue)){ //Ammo (Reload)
                    objectInfo.push_back({xCoord, yCoord,'R'});
                } else if (TileCodes::healthItem(mapValue)){ //Health
                    objectInfo.push_back({xCoord, yCoord,'H'});
                }
            }
        }

        mapDoc.close();

        pEngine->setObjectInfo(objectInfo);
        pEngine->setKeyTiles(keyTiles);
        objectInfo.clear();
        keyTiles.clear();

    }
    //Loads map without needing to give it a specific map tile manager reference
    static void loadTileMap(ZEngine *pEngine, DrawingSurface *pSurface, int mapWidth, int mapHeight, const string &mapPath) {

        auto* newMap = new MapTileManager(pEngine,pEngine->getTileSize(),pEngine->getTileSize());
        newMap->setMapSize(pEngine->getTilesX(), pEngine->getTilesY());

        loadTileMap(pEngine, pSurface, newMap, mapPath);

        delete(newMap);

    }

    //Loads map using specific map tile manager reference
    static void loadTileMap(ZEngine *pEngine, DrawingSurface *pSurface, MapTileManager *map, const string &mapPath){

        ifstream mapDoc;
        mapDoc.open(mapPath);
        int mapValue;

        if (!mapDoc.is_open()){
            cerr << "Maps Not Found" << endl;
            exit(-1);
        }

        for (int y = 0; y < pEngine->getTilesY(); ++y) {
            for (int x = 0; x < pEngine->getTilesX(); ++x) {
                mapValue = getMapValue(mapDoc);
                map->setMapValue(x, y, mapValue);
            }
        }
        map->drawAllTiles(pEngine, pSurface);

        mapDoc.close();
    }

private:

    static int getMapValue(ifstream &mapDoc){
        char mapChar;
        int mapOne,mapTwo;
        int mapValue;
        mapDoc.get(mapChar);
        mapOne = static_cast<int>(mapChar - '0');

        mapDoc.get(mapChar);
        mapTwo = static_cast<int>(mapChar - '0');

        //Check if it's a 3 digit number
        if (isdigit(mapDoc.peek())) {
            //If so get the third digit
            mapDoc.get(mapChar);
            int mapThree = static_cast<int>(mapChar - '0');
            //Combine all three to the mapValue
            mapValue = 100 * mapOne + 10 * mapTwo + mapThree;
        } else {
            //Otherwise just combine the original two
            mapValue = 10 * mapOne + mapTwo;
        }
        mapDoc.ignore();//Ignore the comma/new line characters
        return mapValue;
    }


};


#endif //G52CPP_MAPLOADER_H