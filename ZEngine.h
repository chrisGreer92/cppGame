//
// Created by Chris Greer on 22/03/2024.
//

#ifndef G52CPP_ZENGINE_H
#define G52CPP_ZENGINE_H

#include "../header.h"
#include "../BaseEngine.h"
#include "ZMaps/MapTileManager.h"
#include "../ExampleFilterPointClasses.h"
#include "ZMaps/MapOffsetFilter.h"
#include <memory>
#include <utility>
#include "./ZUtility/InfoStructs.h"

//class ZCharacter;
//Forward declarations to avoid circular dependency
class ZPlayer;
class MovementUtil;
class LivingObject;
class GameObject;
class AStar;
class iStateHandler;
class LevelRunner;

using namespace std;

class ZEngine : public BaseEngine {

public:
    ~ZEngine();

    //Initialisation and dynamic handling of objects
public:
    int virtInitialise() override;
    int virtInitialiseObjects() override;
    void startLevel(string levelNumber, bool fromSave = false);
    void addToDelete(GameObject* object) { m_objectsToDelete.push_back(object); } //Add to the queue of objects to delete
    void addToBeAdded(GameObject* object) { m_objectsToAdd.push_back(object); } //Add to the queue of objects to delete
    //Handles the adding/deleting of objects
    void virtMainLoopPostUpdate() override;

private:
    vector<GameObject*> m_objectsToDelete;
    vector<GameObject*> m_objectsToAdd;
    void removeObject(GameObject* object); //Remove an object from the game

//State Logic
public:
    enum State {s_menu,  s_running, s_paused, s_saveGame, s_loadGame, s_gameOver, s_gameComplete};
    void setState(State newState);
    string getLevelNumber(){return m_currentLevelNumber;}
    void setLevelNumber(string levelNumber){ m_currentLevelNumber = std::move(levelNumber);}
    void flagProgressLevel() {m_progressLevel = true;}
    void flagGameOver() { m_gameOver = true; }
    //Get/set the state we're in before going to load state
    State getLoadCaller() const { return m_lastLoadCall;}
    void setLoadCaller(State currentState) { m_lastLoadCall = currentState;}
    //Get/Set the keys used in this level so we know when we've got them all
    int getTotalKeys() const { return m_totalKeys;}
    void setTotalKeys(int keys) { m_totalKeys = keys;}
    //Methods completely handled By State
    void virtSetupBackgroundBuffer() override;
    void copyAllBackgroundBuffer() override;
    void virtPostDraw() override;
    void virtMainLoopDoBeforeUpdate() override;
    void virtMouseUp(int iButton, int iX, int iY) override;
    void virtMouseDown(int iButton, int iX, int iY) override;
    void virtKeyDown(int iKeyCode) override;
    void virtKeyUp(int iKeyCode) override;

private:
    State m_lastLoadCall; //Tracks which state you were in before going to load
    shared_ptr<iStateHandler> m_currentState = nullptr;;
    shared_ptr<LevelRunner> m_currentLevel = nullptr;;
    string m_currentLevelNumber = "LevelOne";
    bool m_progressLevel = false;
    bool m_gameOver = false;
    int m_totalKeys = 0;

//SAVE/LOAD Utility
public:
    void setObjectInfo(const vector<ObjectInfo> &zCoords){ m_livingCoordinates = zCoords; }
    void setBloodCoords(const vector<ObjectInfo> &bloodCoords){ m_bloodCoordinates = bloodCoords; }
    vector<ObjectInfo>& getBloodCoords() { return m_bloodCoordinates;}
    void setKeyTiles(const vector<KeyTile> &keyTiles){ m_keyTiles = keyTiles; }
    vector<KeyTile>& getKeyTiles( ) {return m_keyTiles; }
    ObjectInfo getPlayerCoords( ) {return {playerX, playerY}; }
private:
    vector<ObjectInfo> m_livingCoordinates; //Used for initialising the zombie coordinates
    vector<ObjectInfo> m_bloodCoordinates; //Used for storing dead item coordinates
    vector<KeyTile> m_keyTiles;

//Logic for aiming at enemies
public:
    //Struct to pass information of which object and the critical distance
    struct InSights {
        LivingObject *object;
        int critDistance; //Determines how damaging the hit is
    };
    void setInSights(InSights inSights){ m_inSight = inSights;}
    InSights getInSights() const { return m_inSight;}
private:
    InSights m_inSight;


//General Game Information
public:
    ZPlayer* getPlayer() const {return m_player;}
    AStar* getAStar() const { return m_aStar.get(); }
    void setAStar(MapTileManager* newMap);
    int getTileSize() const { return m_tileSize; }
    int getTilesX() const { return srcTilesX; }
    int getTilesY() const { return srcTilesY; }
    bool updatePlayerMovement(); //Updates movement based on the movement utility
    shared_ptr<MovementUtil> getPlayerMovementUtil() const { return playerMovement ; }
    shared_ptr<MapOffsetFilter> getMapFilter() const;
    shared_ptr<MapTileManager> getCollisionMap() const;
    DrawingSurface* getEffectsSurface() const;
    DrawingSurface* getSrcSurface() const;
    //DrawingSurface* getBackgroundSurface() { return m_pBackgroundSurface; }

private:
    ZPlayer* m_player = nullptr;
    shared_ptr<AStar> m_aStar = nullptr; //Used to create/delete our actual a_Star
    //shared_ptr<AStar> m_pfixedAStar = nullptr; //Held by objects so they can retrieve new aStar
    shared_ptr<MovementUtil> playerMovement = nullptr;
    shared_ptr<MapOffsetFilter> m_mapFilter = nullptr;
    int srcTilesX = 100;
    int srcTilesY = 100;
    int m_tileSize = 64;
    //Also used for general offset of the map
    int playerX = 0;
    int playerY = 0;






};


#endif //G52CPP_ZENGINE_H
