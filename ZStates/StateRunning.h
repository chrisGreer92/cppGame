//
// Created by Chris Greer on 21/04/2024.
//

#ifndef G52CPP_STATERUNNING_H
#define G52CPP_STATERUNNING_H

#include "iStateHandler.h"
#include "StatePaused.h"
#include "SurfaceManager.h"

#include "../ZEngine.h"
#include "../ZObjects/ZPlayer.h"
#include "../ZUtility/Animator.h"
#include "../ZUtility/SaveLoadUtil.h"
#include "../ZUtility/InfoStructs.h"
#include "../ZUtility/TileCodes.h"
#include "../ZPixels/ImagePixelRepo.h"
#include "../ZMovement/MovementUtil.h"

class StateRunning : public iStateHandler, public SurfaceManager {

protected:
    vector<KeyTile> m_keyTiles;
    int m_keysActivated = 0;
    bool levelComplete = false;

public:
    explicit StateRunning(ZEngine* pEngine) : iStateHandler(pEngine) {}
    ~StateRunning() { m_keyTiles.clear(); }

    //Used to handle moving over an unlocking/main/progressing tile
    void handleTile(int mapX, int mapY){

        if(m_keyTiles.empty()) return; //For some reason didn't properly initialise

        int mapValue = m_collisionMap->getMapValueForScreenLocation(mapX,mapY, true);
        if (levelComplete && TileCodes::progressArea(mapValue)){
            m_pEngine->flagProgressLevel();
            return; //Have finished this level so don't do anything else
        }

        //Only care about un-triggered unlock zones
        if (!TileCodes::doorKey(mapValue)&& !TileCodes::mainKey(mapValue)) return;

        //Store our tileX/Y
        int tileX = m_collisionMap->getMapXForScreenX(mapX);
        int tileY = m_collisionMap->getMapXForScreenX(mapY);
        bool normalUnlock = false;
        //Double check (shouldn't be triggered otherwise) that this is in our vector of keys
        for (auto& keyTile : m_keyTiles) {
            if (!keyTile.triggered && keyTile.x == tileX && keyTile.y == tileY) {
                //Redraw this tile to show it was triggered already
                m_collisionMap->setAndRedrawMapValueAt(tileX, tileY, TileCodes::inactiveKey(), m_pEngine , m_srcSurface.get());
                keyTile.triggered = true; //Update that it's been triggered (uneccessary but allows extension)

                if (keyTile.type == 'K') { //if this is a main key then more logic to check/update
                    m_keysActivated++; //Update key count
                    if (m_keysActivated == m_pEngine->getTotalKeys()){
                        levelComplete = true; //Have unlocked all the main keys, progress area is now open
                    } else break; //Don't want to then update the related tiles
                }
                //If normal unlock or have got all the main keys, unlock the relevant tiles based on this tile
                for (const auto& unlockTile : keyTile.unlocksTiles) {
                    m_collisionMap->setAndRedrawMapValueAt(unlockTile.x, unlockTile.y, unlockTile.newMap, m_pEngine, m_srcSurface.get());
                    normalUnlock = true; //We've unlocked some doors
                }
                if (normalUnlock) {
                    //Need engine to update the AStar algo to account for change in tiles...
                    m_pEngine->setAStar(m_collisionMap.get());
                }
                m_pEngine->setKeyTiles(m_keyTiles); //Update the engine, (should really be done with pointers)
                break; //Don't need to check any further
            }
        }

        //Auto-Save the game at this point
        SaveLoadUtil::saveGame(m_pEngine,0,"AutoSave");
    }

    void setUpBackgroundBuffer() override{} //Don't actually need to set anything in background buffer

    void copyAllBackgroundBuffer() override{

        //First update our maps/object offset so located correctly relative to player
        updateOffset();

        MapOffsetFilter* mapFilter = m_mapFilter.get();
        int offsetX = mapFilter->getXOffset();
        int offsetY = mapFilter->getYOffset();

        //Update our waves animation background
        Animator::animate(m_waveSurface, 
            m_backgroundWaves, m_pEngine->getModifiedTime(), m_wavesCounter, m_wavesLastUpdated, 80);
        m_pEngine->getForegroundSurface()->copyEntireSurface(m_waveSurface.get());

        //IF updated, are we doing this too frequently otherwise??
        //Although it is constantly updated...
        //Draw the shifted background to show the movement
        m_pEngine->getForegroundSurface()->copyRectangleFrom(m_srcSurface.get(),
                                                  0,0,
                                                  m_pEngine->getWindowWidth(),m_pEngine->getWindowHeight(),
                                                  offsetX, offsetY);
        //Do the same from our effects surface
        m_pEngine->getForegroundSurface()->copyRectangleFrom(m_effectsSurface.get(),
                                                  0,0,
                                                  m_pEngine->getWindowWidth(),m_pEngine->getWindowHeight(),
                                                  offsetX, offsetY);
    }
    void postDraw() override{

        //Draw our hud on top of everything (inc objects)
        m_pEngine->getForegroundSurface()->
        copyRectangleFrom(m_hudSurface.get(),0,700,
                           m_pEngine->getWindowWidth(),100,
                           0,0);
        //Now draw on top of that our actual values
        drawHudInfo(m_pEngine,m_pEngine->getForegroundSurface());

        if (m_pEngine->isKeyPressed(SDLK_m)){
            shared_ptr<SimpleImage> image = (*ImagePixelRepo::getSingleImages()).at(m_pEngine->getLevelNumber());
            image->renderImage(m_pEngine->getForegroundSurface(),0,0,300,139,image->getWidth(),image->getHeight());
        }
    }

    void beforeUpdate() override {
        if (!m_pEngine->getPlayer() || !m_pEngine->getMapFilter()) return ;

        int ticks = m_pEngine->getModifiedTime() - m_lastUpdated;

        //If we updated too recently don't need to do anything for this update
        if (ticks < 20) return;
        else m_lastUpdated = m_pEngine->getModifiedTime();
        //cout << ticks << endl;

        //If we're moving at all then do some updating
        if(m_pEngine->updatePlayerMovement()){
            //Make sure player is animated
            m_pEngine->getPlayer()->setMoving(true);
            //Update our filter with the offset based on the players location, minus the exact center
            updateOffset();
            //Check if on a key tile
            handleTile(m_pEngine->getPlayerCoords().x,m_pEngine->getPlayerCoords().y);
            //m_pEngine->updateOffset();
        } else {
            //Stopped moving so don't animate
            m_pEngine->getPlayer()->setMoving(false);
        }
    }

    void postUpdate() override {
        //Redraw the display
        m_pEngine->redrawDisplay();
    }

    void handleMouseDown(int iButton, int iX, int iY) override{
        //Left click fires weapon
        if (iButton == SDL_BUTTON_LEFT && m_pEngine->getPlayer())
            m_pEngine->getPlayer()->fireWeapon();
    }

    void handleMouseUp(int iButton, int iX, int iY) override{
        //Tell player to stop firing (automatic weapons only)
        if (iButton == SDL_BUTTON_LEFT && m_pEngine->getPlayer())
            m_pEngine->getPlayer()->stopFiring();
    }

    void handleKeyDown(int iKeyCode) override{

        if (iKeyCode == SDLK_SPACE || iKeyCode == SDLK_ESCAPE ){
            //Go to Paused state
            m_pEngine->setState(ZEngine::s_paused);
            return;
        }

        if (iKeyCode == SDLK_i) {
            //Move to the next level
            m_pEngine->flagProgressLevel();
            return;
        }

        //Otherwise handle any relevant key presses
        shared_ptr<MovementUtil> movement =  m_pEngine->getPlayerMovementUtil();
        switch ( iKeyCode )
        {
            case SDLK_q:
                m_pEngine->getPlayer()->switchWeapon();
                break;
            case SDLK_LSHIFT:
                movement->running(true);
                break;
            case SDLK_w:
                movement->movingUp(true);
                break;
            case SDLK_a:
                movement->movingLeft(true);
                break;
            case SDLK_s:
                movement->movingDown(true);
                break;
            case SDLK_d:
                movement->movingRight(true);
                break;
            case SDLK_f:
                m_pEngine->getPlayer()->melee();
                break;
            default:
                break;
        }
    }

    void handleKeyUp(int iKeyCode) override{

        shared_ptr<MovementUtil> movement =  m_pEngine->getPlayerMovementUtil();

        switch ( iKeyCode ) {
            case SDLK_LSHIFT:
                movement->running(false);
                break;
            case SDLK_w:
                movement->movingUp(false);
                break;
            case SDLK_a:
                movement->movingLeft(false);
                break;
            case SDLK_s:
                movement->movingDown(false);
                break;
            case SDLK_d:
                movement->movingRight(false);
                break;
            default:
                break;
        }
    }

private:
    void updateOffset() {
        m_mapFilter->setOffset(m_pEngine->getPlayerCoords().x - m_pEngine->getPlayer()->getExactRealCenterX(),
            m_pEngine->getPlayerCoords().y - m_pEngine->getPlayer()->getExactRealCenterY());

    }
    long m_lastUpdated = 0;
};

#endif //G52CPP_STATERUNNING_H
