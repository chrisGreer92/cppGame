//
// Created by Chris Greer on 21/04/2024.
//

#ifndef G52CPP_LEVELRUNNER_H
#define G52CPP_LEVELRUNNER_H

#include "StateRunning.h"

class LevelRunner : public StateRunning{
public:
    explicit LevelRunner(ZEngine* pEngine, const string& levelNumber, bool fromSave) : StateRunning(pEngine){
        //Create our map filter
        m_mapFilter = make_shared<MapOffsetFilter>();
        //Set up our surfaces (doesn't draw anything on them yet)
        setUpSurfaces(pEngine);
        //Then initialise these surfaces with the relevent data
        initialiseNewLevel(pEngine, levelNumber, fromSave);

    }

   void initialiseNewLevel(ZEngine* pEngine, const string& levelNumber, bool fromSave) {

       m_keysActivated = 0;

       //Fill our surfaces based on this levels maps
       loadLevelMaps(pEngine, levelNumber);

       //If loading from a save, don't need to load level's game objects
       if (!fromSave) {
           //Load Our Player/Enemies/Objects Map for given level
           MapLoader::loadObjectTileMap(pEngine, "./resources/TileMaps/" + levelNumber + "/ObjectMap.txt");
       }

       m_keyTiles = m_pEngine->getKeyTiles(); // This will either be set on loading or on the above mapLoader
       if (fromSave)
           intialiseTiles(); //If from save then initialise them based on the save data

       //Create our AStar Map based on these surfaces/Tiles
       pEngine->setAStar(m_collisionMap.get());

    }

    //Initialises already triggered tiles based on the tileLeys we've loaded.
    void intialiseTiles(){
        for (auto& keyTile : m_keyTiles) {
            if (keyTile.triggered) {
                //Redraw this tile to show it was triggered already
                m_collisionMap->setAndRedrawMapValueAt(keyTile.x, keyTile.y, TileCodes::inactiveKey(), m_pEngine , m_srcSurface.get());
                if (keyTile.type == 'K') { //if this is a main key then more logic to check/update
                    m_keysActivated++; //Update key count
                    if (m_keysActivated == m_pEngine->getTotalKeys()){
                        levelComplete = true; //Have unlocked all the main keys, progress area is now open
                    } else continue; //Don't want to then update the related tiles
                }
                //If normal unlock or have got all the main keys, unlock the relevant tiles based on this tile
                for (const auto& unlockTile : keyTile.unlocksTiles) {
                    m_collisionMap->setAndRedrawMapValueAt(unlockTile.x, unlockTile.y, unlockTile.newMap, m_pEngine, m_srcSurface.get());
                }
            }
        }
    }
};

#endif //G52CPP_LEVELRUNNER_H
