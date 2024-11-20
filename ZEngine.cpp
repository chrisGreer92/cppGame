//
// Created by Chris Greer on 22/03/2024.
//

#include "../header.h"
#include "ZEngine.h"
#include "ZObjects/ZPlayer.h"
#include "ZMaps/MapLoader.h"
#include "ZPixels/ImagePixelRepo.h"
#include "ZMovement/MovementUtil.h"
#include "ZObjects/ZombieFactory.h"
#include "ZObjects/StaticObjectFactory.h"
#include <memory>
#include "./ZStates/StateMenu.h"
#include "./ZStates/StatePaused.h"
#include "./ZStates/StateSave.h"
#include "./ZStates/StateLoad.h"
#include "./ZStates/LevelRunner.h"
#include "./ZStates/StateGameOver.h"
#include "./ZStates/StateGameComplete.h"


using namespace std;

ZEngine::~ZEngine() {
    drawableObjectsChanged();
    destroyOldObjects(true);
    playerMovement.reset();
    m_currentState.reset();
    m_currentLevel.reset();
    m_objectsToDelete.clear();
    m_objectsToAdd.clear();
    m_bloodCoordinates.clear();
    m_livingCoordinates.clear();
    m_keyTiles.clear();
    m_aStar.reset();
    m_mapFilter.reset();
    ImagePixelRepo::deleteRepo();
}


int ZEngine::virtInitialise() {

    //initialise our image/pixel repo
    ImagePixelRepo::initialise();

    //Initialise the starting state (Menu)
    m_currentState = make_shared<StateMenu>(this);

    //Do Set up our background buffer for current state in advance
    m_pBackgroundSurface->mySDLLockSurface();
    virtSetupBackgroundBuffer();
    m_pBackgroundSurface->mySDLUnlockSurface();

    return 0;
    //Don't need to set up objects (yet), this is done depending on the new/load
}

//Create a level based on the string level number, initialise objects and auto save
void ZEngine::startLevel(string levelNumber, bool fromSave){

    pause();
    //Either create a new level runner or tell the old one to re-initialse
    if (!m_currentLevel)
        m_currentLevel = make_shared<LevelRunner>(this, levelNumber, fromSave);
    else
        m_currentLevel->initialiseNewLevel(this, levelNumber, fromSave);

    //Need to update current state since objects depend on this to initialse themselves
    m_currentState = m_currentLevel;
    virtInitialiseObjects(); //Initialise our objects based on this level
    
    m_currentLevelNumber.clear();
    m_currentLevelNumber = levelNumber; //Update our level Number/Name for use with saving
    //Show our objects
    setAllObjectsVisible(true);
    SaveLoadUtil::saveGame(this, 0, "AutoSave"); //Create an autosave file for the new level
    unpause(); //Never actually paused it...
}

void ZEngine::setAStar(MapTileManager* newMap) { 

    //Create our AStar unless it already exists
    if(!m_aStar ) m_aStar = std::make_shared<AStar>(this, newMap);

    //If just updating, then just re-set values to use the new tileManager
    else m_aStar->resetMap(newMap);
}

//Called by the various levels and after loading
int ZEngine::virtInitialiseObjects() {

    drawableObjectsChanged();
    destroyOldObjects(true);

    //Create our array based on how many objects are in this level
    createObjectArray(static_cast<int>(m_livingCoordinates.size()));


    int i = 1;
    for(const auto& info : m_livingCoordinates){

        if (info.type == 'P'){ //This will always be the first in the save file
            playerX = info.x; //Determine starting location
            playerY = info.y;
            m_player = new ZPlayer(this, info); //Needed so we can pass it to other objects
            //May be re-initialising player
            playerMovement = make_shared<MovementUtil>(m_player, 2.0f, 7.0f, true);
            //Also set the player as first object, it's more immersive if you're 'swarmed' by enemies
            storeObjectInArray(0, m_player); //Will always be 0
        } else if (info.type == 'Z' || info.type == 'A' || info.type == 'S') { //Our enemies
            //Initialise the zombie
            auto *zombie = ZombieFactory::createEnemy(this, info);
            storeObjectInArray(i++, zombie);
        } else { //Our Pickup Objects
            auto *object = StaticObjectFactory::createObject(this, info);
            storeObjectInArray(i++, object);
        }
    }
    m_livingCoordinates.clear(); //Clear zombie coords (not really needed)
    //Then paint any blood from dead objects
    for(const auto& blood : m_bloodCoordinates){
        Animator::paintBlood(this, blood.x, blood.y);
    }
    //Need to update this to total objects

    setAllObjectsVisible(false); // <- Don't show by default
    return 0;

}

//Set the state to change to (once this loop is done)
void ZEngine::setState(State newState){

    if (newState == s_running){ //Only one where we want visible objects
        //Just pick up where we left off in current level
        m_currentState = m_currentLevel;
        //Show our objects
        setAllObjectsVisible(true);
        unpause();
    } else {
        //Otherwise we always pause then set a different state
        setAllObjectsVisible(false);
        pause();
    }
    switch(newState){
        case s_paused:
            getPlayerMovementUtil()->stopMoving();
            m_currentState = make_shared<StatePaused>(this);
            break;
        case s_loadGame:
            m_currentState = make_shared<StateLoad>(this);
            break;
        case s_saveGame:
            m_currentState = make_shared<StateSave>(this);
            break;
        case s_menu:
            m_currentState = make_shared<StateMenu>(this);
            break;
        case s_gameOver:
            m_currentState = make_shared<StateGameOver>(this);
            break;
        case s_gameComplete:
            m_currentState = make_shared<StateGameComplete>(this);
            break;
        default:
            break;
    }
    //Re-set up background
    lockAndSetupBackground();
    //Force a re-draw
    redrawDisplay();
}

void ZEngine::virtMainLoopPostUpdate() {

    //Call on our current state in case they need to do anything
    m_currentState->postUpdate();

    //Check if we're progressing or it's game over
    if (m_progressLevel){
        m_progressLevel = false; //Make sure to unflag now
        m_bloodCoordinates.clear(); //Clear any saved blood coords
        if (m_currentLevelNumber == "LevelOne"){
            startLevel("LevelTwo");
        } else if (m_currentLevelNumber == "LevelTwo"){
            startLevel("LevelThree");
        } else if (m_currentLevelNumber == "LevelThree") {
            setState(s_gameComplete);
        }
    }
    if (m_gameOver){ //If game over then show relevent screen
        m_gameOver = false;
        drawableObjectsChanged();
        destroyOldObjects(true); //Delete all the current objects
        setState(s_gameOver);//Move to game over state
        return; //Exit as don't want to delete anything else
    }

    //Delete any objects that need deleting
    //This can (and potentially should) be done whichever state you're in
    if (!m_objectsToDelete.empty()){

        for(const auto& obj : m_objectsToDelete){
            //First add them to our blood vector to keep track of their positions
            m_bloodCoordinates.push_back(
                    {obj->getExactRealCenterX(),obj->getExactRealCenterY(),'D',0});//May change the type setting later to paint specific blood
            //Then remove them from the objects array
            removeObject(obj);
            delete(obj);
        }
        m_objectsToDelete.clear(); //Clear since we've just handled them
    }

    //Add any new objects
    if (!m_objectsToAdd.empty()) {
        for(const auto& obj : m_objectsToAdd) {
            appendObjectToArray(obj);
        }
        m_objectsToAdd.clear();
    }

}

//Method for removing objects from our objects array
void ZEngine::removeObject(GameObject *object) {
    moveToLast(object); //Put it in the last position
    removeDisplayableObject(object); //Then remove it (deletes it too)
}

bool ZEngine::updatePlayerMovement(){
    if (!playerMovement) return false;
    return playerMovement->updateMovement(playerX, playerY);
}

shared_ptr<MapTileManager> ZEngine::getCollisionMap() const {
    return m_currentLevel->getCollisionMap();
}

DrawingSurface* ZEngine::getEffectsSurface() const {
    return m_currentLevel->getEffectsSurface();
}

DrawingSurface* ZEngine::getSrcSurface() const {
    return m_currentLevel->getSrcSurface();
}

shared_ptr<MapOffsetFilter> ZEngine::getMapFilter() const {
    return m_currentLevel->getOffsetFilter();
}

//The below methods are handled by our state pattern so will depend on which state is set during runtime
void ZEngine::virtSetupBackgroundBuffer() {
    m_currentState->setUpBackgroundBuffer();
}

void ZEngine::copyAllBackgroundBuffer() {
    m_currentState->copyAllBackgroundBuffer();
}
void ZEngine::virtPostDraw() {
    m_currentState->postDraw();
}

//Do things first IF we're updating
void ZEngine::virtMainLoopDoBeforeUpdate() {
    m_currentState->beforeUpdate();
}

void ZEngine::virtMouseDown(int iButton, int iX, int iY) {
    m_currentState->handleMouseDown(iButton,iX,iY);
}

void ZEngine::virtMouseUp(int iButton, int iX, int iY) {
    m_currentState->handleMouseUp(iButton,iX,iY);
}

void ZEngine::virtKeyUp(int iKeyCode) {
    m_currentState->handleKeyUp(iKeyCode);
}

void ZEngine::virtKeyDown(int iKeyCode) {

    if (iKeyCode == SDL_WINDOWEVENT_CLOSE) {
        setExitWithCode(0);
    }
    m_currentState->handleKeyDown(iKeyCode);
    //ALL states need too exit on windows exit
   
}


