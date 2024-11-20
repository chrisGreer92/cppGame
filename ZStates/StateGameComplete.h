//
// Created by Chris Greer on 21/04/2024.
//

#ifndef G52CPP_StateGameComplete_H
#define G52CPP_StateGameComplete_H

#include "iStateHandler.h"
#include "../ZUtility/SaveLoadUtil.h"
#include "../ZEngine.h"
#include "StateRunning.h"
#include "UIUtil/UIUtil.h"

class StateGameComplete : public iStateHandler {

public:
    explicit StateGameComplete(ZEngine* pEngine) : iStateHandler(pEngine) {}


    void setUpBackgroundBuffer() override {

        SimpleImage logoImage = ImageManager::loadImage("./resources/SurfaceImages/LOGOFullScreen.png");
        logoImage.renderImage(m_pEngine->getBackgroundSurface(),
                              0,0,0,0,
                              m_pEngine->getWindowWidth(),m_pEngine->getWindowHeight());
       m_pEngine->getBackgroundSurface()->
                drawFastString(385, 480,
                "GAME COMPLETED!", 0xFFFFFF,
                m_pEngine->getFont("./resources/Fonts/Branda-yolq.ttf", 64));


       m_pEngine->getBackgroundSurface()->
                drawFastString(480, 557,
                "Thank you for playing", 0xFFFFFF,
                m_pEngine->getFont("./resources/Fonts/Branda-yolq.ttf", 32));



        UIUtil::drawButton(m_pEngine,m_pEngine->getBackgroundSurface(),menuX, menuY,"Main Menu");

    }


    void handleMouseDown(int iButton, int iX, int iY) override{

        if(UIUtil::buttonPress(iX,iY,menuX,menuY)){
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
    int menuX = 520;
    int menuY = 650;


};


#endif //G52CPP_StateGameComplete_H
