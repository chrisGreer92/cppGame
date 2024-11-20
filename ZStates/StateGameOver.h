//
// Created by Chris Greer on 21/04/2024.
//

#ifndef G52CPP_StateGameOver_H
#define G52CPP_StateGameOver_H

#include "iStateHandler.h"
#include "../ZUtility/SaveLoadUtil.h"
#include "../ZEngine.h"
#include "StateRunning.h"
#include "UIUtil/UIUtil.h"

class StateGameOver : public iStateHandler {

public:
    explicit StateGameOver(ZEngine* pEngine) : iStateHandler(pEngine) {}


    void setUpBackgroundBuffer() override {


        SimpleImage gameOver = ImageManager::loadImage("./resources/SurfaceImages/GameOver.jpeg");
        gameOver.renderImage(m_pEngine->getBackgroundSurface(),
                             0,0,0,0,
                             m_pEngine->getWindowWidth(),m_pEngine->getWindowHeight());
        //Draw our UI buttons
        UIUtil::drawButton(m_pEngine, m_pEngine->getBackgroundSurface(), newX, newY, "New Game");
        UIUtil::drawButton(m_pEngine,m_pEngine->getBackgroundSurface(),loadX,loadY,"Load Game");
        UIUtil::drawButton(m_pEngine,m_pEngine->getBackgroundSurface(),menuX, menuY,"Main Menu");

    }


    void handleMouseDown(int iButton, int iX, int iY) override{

        if (UIUtil::buttonPress(iX, iY, newX, newY)){
            m_pEngine->startLevel("LevelOne",false); //Change to save state
        } else if(UIUtil::buttonPress(iX,iY,loadX,loadY)){
            m_pEngine->setLoadCaller(ZEngine::s_gameOver); //Tell the engine it was here before going to load
            m_pEngine->setState(ZEngine::s_loadGame); //Change to load state
        } else if(UIUtil::buttonPress(iX,iY,menuX,menuY)){
            m_pEngine->setState(ZEngine::s_menu); //Change to Intro (menu) state
        }

    };

    //No Implementation needed
    void handleKeyDown(int iKeyCode) override {};
    void postDraw() override{};
    void beforeUpdate() override{};
    void postUpdate() override {};
    void handleMouseUp(int iButton, int iX, int iY) override{};
    void handleKeyUp(int iKeyCode) override {};

private:
    int newX = 325;
    int newY = 502;
    int loadX = 715;
    int loadY = 502;
    int menuX = 520;
    int menuY = 650;


};


#endif //G52CPP_StateGameOver_H
