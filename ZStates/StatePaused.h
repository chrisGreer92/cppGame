//
// Created by Chris Greer on 21/04/2024.
//

#ifndef G52CPP_STATEPAUSED_H
#define G52CPP_STATEPAUSED_H

#include "iStateHandler.h"
#include "../ZUtility/SaveLoadUtil.h"
#include "../ZEngine.h"
#include "StateRunning.h"
#include "UIUtil/UIUtil.h"

class StatePaused : public iStateHandler {

public:
    explicit StatePaused(ZEngine* pEngine) : iStateHandler(pEngine) {}


    void setUpBackgroundBuffer() override {

        SimpleImage pauseImage = ImageManager::loadImage("./resources/SurfaceImages/Background.jpeg");
        pauseImage.renderImage(m_pEngine->getBackgroundSurface(),
                                 400,400,0,0,
                                 m_pEngine->getWindowWidth(),m_pEngine->getWindowHeight());

        m_pEngine->getBackgroundSurface()->
                drawFastString(
                m_pEngine->getWindowWidth()/4 + m_pEngine->getWindowWidth()/6,
                m_pEngine->getWindowHeight()/4 + m_pEngine->getWindowHeight()/7,
                "PAUSED", 0xFFFFFF,
                m_pEngine->getFont("./resources/Fonts/Branda-yolq.ttf", 64));


        m_pEngine->getBackgroundSurface()->drawFastString(
                470,
                m_pEngine->getWindowHeight()/3 + m_pEngine->getWindowHeight()/7,
                "Hit Space/Esc To Continue", 0xFFFFFF,
                m_pEngine->getFont("./resources/Fonts/Branda-yolq.ttf", 32));

        //Draw our UI buttons
        UIUtil::drawButton(m_pEngine,m_pEngine->getBackgroundSurface(),saveX,saveY,"Save Game");
        UIUtil::drawButton(m_pEngine,m_pEngine->getBackgroundSurface(),loadX,loadY,"Load Game");
        UIUtil::drawButton(m_pEngine,m_pEngine->getBackgroundSurface(),menuX, menuY,"Main Menu");

    }


    void handleMouseDown(int iButton, int iX, int iY) override{

        if (UIUtil::buttonPress(iX,iY,saveX,saveY)){
            m_pEngine->setState(ZEngine::s_saveGame); //Change to save state
        } else if(UIUtil::buttonPress(iX,iY,loadX,loadY)){
            m_pEngine->setLoadCaller(ZEngine::s_paused); //Tell the engine it was here before going to load
            m_pEngine->setState(ZEngine::s_loadGame); //Change to load state
        } else if(UIUtil::buttonPress(iX,iY,menuX,menuY)){
            m_pEngine->setState(ZEngine::s_menu); //Change to Intro (menu) state
        }

    };
    void handleKeyDown(int iKeyCode) override {
        if (iKeyCode == SDLK_SPACE || iKeyCode == SDLK_ESCAPE)
            m_pEngine->setState(ZEngine::s_running);
    };

    //No Implementation needed
    void postDraw() override{};
    void beforeUpdate() override{};
    void postUpdate() override {};
    void handleMouseUp(int iButton, int iX, int iY) override{};
    void handleKeyUp(int iKeyCode) override {};

private:
    int saveX = 325;
    int saveY = 502;
    int loadX = 715;
    int loadY = 502;
    int menuX = 520;
    int menuY = 650;


};


#endif //G52CPP_STATEPAUSED_H
