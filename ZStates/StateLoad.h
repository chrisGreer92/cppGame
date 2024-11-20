//
// Created by Chris Greer on 23/04/2024.
//

#ifndef G52CPP_STATELOAD_H
#define G52CPP_STATELOAD_H

#include "../../header.h"
#include "iStateHandler.h"
#include "UIUtil/UISaveLoadUtil.h"

class StateLoad : public iStateHandler {

private:
    int m_saveFile = -1;
    bool loadAttempted = false;
    bool loadFailed = false;

public:
    explicit StateLoad(ZEngine* pEngine) : iStateHandler(pEngine) {}

    void setUpBackgroundBuffer() override {
        SimpleImage pauseImage = ImageManager::loadImage("./resources/SurfaceImages/Background.jpeg");
        pauseImage.renderImage(m_pEngine->getBackgroundSurface(),
                               400,400,0,0,
                               m_pEngine->getWindowWidth(),m_pEngine->getWindowHeight());

        UISaveLoadUtil::drawSaveLoadUI(m_pEngine,m_pEngine->getBackgroundSurface());

    };

    void copyAllBackgroundBuffer() override {
        m_pEngine->getForegroundSurface()->
                copyEntireSurface(m_pEngine->getBackgroundSurface());
        if (m_saveFile != -1){
            UISaveLoadUtil::highlightExistingSave(m_pEngine, m_pEngine->getForegroundSurface(), m_saveFile);
            UISaveLoadUtil::drawLoadButton(m_pEngine,m_pEngine->getForegroundSurface());
        }
        if (loadAttempted && loadFailed){
            m_pEngine->getForegroundSurface()->drawFastString(
                    920, 550, "Loading Failed...", 0xFFFFFF,
                    m_pEngine->getFont("./resources/Fonts/Branda-yolq.ttf", 32));
        }
    };

    void handleMouseDown(int iButton, int iX, int iY) override {

        //Reset our flags
        loadAttempted = false;
        loadFailed = false;

        //If clicked on load button (with something highlighted)
        if ( m_saveFile != -1 && UISaveLoadUtil::loadButtonClicked(iX,iY)){
            loadAttempted = true; //Attempting to load something
            //Loading an existing game based on currently highlighted
            if (SaveLoadUtil::loadGame(m_pEngine,m_saveFile)){//Try to load the relevent info
                //Tell the engine to initialise this level
                m_pEngine->startLevel(m_pEngine->getLevelNumber(), true);
                //m_pEngine->setState(ZEngine::s_running); //Change to the running state
                return;
            } else {
                loadFailed = true;
            }

        }
        //Otherwise check if we clicked something else
        int boxNumber = UISaveLoadUtil::existingSaveClick(iX, iY);
        m_saveFile = boxNumber;

        if (UISaveLoadUtil::saveLoadUIBack(iX,iY)){
            //Go to the state that called this load
            m_pEngine->setState(m_pEngine->getLoadCaller());
        } else {
            m_pEngine->redrawDisplay();
        }

    };
    void handleMouseUp(int iButton, int iX, int iY) override {};

    void postDraw() override {};

    void beforeUpdate() override {};
    void postUpdate() override {};

    void handleKeyDown(int iKeyCode) override {};
    void handleKeyUp(int iKeyCode) override {};
};

#endif //G52CPP_STATELOAD_H
