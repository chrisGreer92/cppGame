//
// Created by Chris Greer on 23/04/2024.
//

#ifndef G52CPP_STATEMENU_H
#define G52CPP_STATEMENU_H

#include "iStateHandler.h"
#include "../../ImageManager.h"
#include "../ZEngine.h"
#include "UIUtil/UIUtil.h"

class StateMenu : public iStateHandler{

public:
    explicit StateMenu(ZEngine* pEngine) : iStateHandler(pEngine) {}

    void setUpBackgroundBuffer() override {
        SimpleImage logoImage = ImageManager::loadImage("./resources/SurfaceImages/LOGOFullScreen.png");
        logoImage.renderImage(m_pEngine->getBackgroundSurface(),
                               0,0,0,0,
                               m_pEngine->getWindowWidth(),m_pEngine->getWindowHeight());

        //Draw our buttons
        UIUtil::drawButton(m_pEngine, m_pEngine->getBackgroundSurface(), newX, newY, "New Game");
        UIUtil::drawButton(m_pEngine,m_pEngine->getBackgroundSurface(),loadX,loadY,"Load Game");
        UIUtil::drawButton(m_pEngine,m_pEngine->getBackgroundSurface(),exitX, exitY,"Exit Game");
    }

    void handleKeyDown(int iKeyCode) override {}

    void handleMouseDown(int iButton, int iX, int iY) override {

        if (UIUtil::buttonPress(iX, iY, newX, newY)){
            m_pEngine->startLevel("LevelOne"); //Start a new game from level one
            //m_pEngine->setState(ZEngine::s_running); //Change to the running state
        } else if(UIUtil::buttonPress(iX,iY,loadX,loadY)){
            m_pEngine->setLoadCaller(ZEngine::s_menu); //Tell the engine it was here before going to load
            m_pEngine->setState(ZEngine::s_loadGame); //Change to load state
        } else if(UIUtil::buttonPress(iX,iY,exitX,exitY)) {
            m_pEngine->setExitWithCode(0); //Exit the game
        }
    }

    //Below don't need any implementation
    void postDraw() override {}

    void beforeUpdate() override {}
    void postUpdate() override {}


    void handleMouseUp(int iButton, int iX, int iY) override {}
    void handleKeyUp(int iKeyCode) override {}

private:
    int newX = 325;
    int newY = 502;
    int loadX = 715;
    int loadY = 502;
    int exitX = 520;
    int exitY = 650;
};

#endif //G52CPP_STATEMENU_H
