//
// Created by Chris Greer on 23/04/2024.
//

#ifndef G52CPP_STATESAVE_H
#define G52CPP_STATESAVE_H

#include "iStateHandler.h"
#include "../ZEngine.h"
//#include "UISaveLoadUtil.h"
#include "UIUtil/UISaveUtil.h"

class StateSave : public iStateHandler {

private:
    int m_saveFile = -1;
    string newSave;
    bool enteringName = false;
    bool m_gameSaved = false;
    bool m_saveSuccessful = false;

public:
    explicit StateSave(ZEngine* pEngine) : iStateHandler(pEngine) {}

    void handleKeyDown(int iKeyCode)override {
        if (!enteringName) return; //Not currently entering in name box

        //Normal char/number Key
        if ((iKeyCode >= SDLK_a && iKeyCode <= SDLK_z) || (iKeyCode >= SDLK_0 && iKeyCode <= SDLK_9)) {
            if (newSave.length() > 10) return; //Too long already
            //Add it to our string
            newSave.push_back(static_cast<char>(iKeyCode));
        }
        switch(iKeyCode){
            case SDLK_RETURN:
                break; //Done writing (for now?)
            case SDLK_BACKSPACE:
                if(!newSave.empty()) newSave.pop_back(); //Delete the last entered char
                break;
            default: //Don't need to do anything
                break;
        }
        //Redraw the display as potentially changed our string
        m_pEngine->redrawDisplay();

    };

    void setUpBackgroundBuffer()override {

        //Just loading the image
        SimpleImage pauseImage = ImageManager::loadImage("./resources/SurfaceImages/Background.jpeg");
        pauseImage.renderImage(m_pEngine->getBackgroundSurface(),
                               400,400,0,0,
                               m_pEngine->getWindowWidth(),m_pEngine->getWindowHeight());

    }

    void copyAllBackgroundBuffer() override{

        m_pEngine->getForegroundSurface()->
                copyEntireSurface(m_pEngine->getBackgroundSurface());

        UISaveLoadUtil::drawSaveLoadUI(m_pEngine,m_pEngine->getForegroundSurface());
        if (m_saveFile != -1){
            UISaveLoadUtil::highlightExistingSave(m_pEngine, m_pEngine->getForegroundSurface(), m_saveFile);
            UISaveUtil::drawSaveInput(m_pEngine, m_pEngine->getForegroundSurface(), newSave, enteringName);
        }
        if (m_gameSaved){ //If we've just saved a game, show dialogue
            //Show our Boxes again (to show new name)
            UISaveLoadUtil::drawSaveLoadUI(m_pEngine,m_pEngine->getForegroundSurface());
            UISaveUtil::drawSaveSuccess(m_pEngine,m_pEngine->getForegroundSurface(),m_saveSuccessful);
        }
    };

    void postDraw()override {};

    void beforeUpdate()override {};
    void postUpdate()override {};

    void handleMouseDown(int iButton, int iX, int iY)override {

        //If we've entered text and clicked "save", save the game!
        if (UISaveUtil::saveButtonClicked(iX,iY) && !newSave.empty()){
            //Flag that we've attempted to save the game (to show dialogue)
            m_gameSaved = true;
            if(SaveLoadUtil::saveGame(m_pEngine, m_saveFile, newSave)){
                //Successful save
                m_saveSuccessful = true;

                //Clear our flags
                m_saveFile = -1;
                newSave.clear();
                enteringName = false;
            } else {
                m_saveSuccessful = false; //Failed to save (hopefully not possible)
            }

        } else { //If we click anywhere else, make sure not to show the dialogue
            m_gameSaved = false;
        }

        //Check if we've clicked to input (only if we already clicked a save box)
        if (UISaveUtil::saveInputClicked(iX,iY) && m_saveFile != -1){
            enteringName = true;
        } else {
            //If not check whether we've clicked on a save box
            int boxNumber = UISaveLoadUtil::existingSaveClick(iX, iY);
            if (boxNumber == 0) return; //Can't select box number 0 as it's auto save
            if (m_saveFile != boxNumber){
                enteringName = false; //Clear flag otherwise as no longer entering
                newSave.clear(); //Clear our current string (if any)
            } //Might have clicked same box again
            m_saveFile = boxNumber; //Update box number
        } //Otherwise clear our flag

        //Check if we clicked back
        if (UISaveLoadUtil::saveLoadUIBack(iX,iY)){
            m_pEngine->setState(ZEngine::s_paused); //Always goes back to paused state
        } else {
            //Redraw our display wherever else we clicked
            m_pEngine->redrawDisplay();
        }




    };


    //No implementation Needed
    void handleMouseUp(int iButton, int iX, int iY)override {};
    void handleKeyUp(int iKeyCode)override {};
};

#endif //G52CPP_STATESAVE_H
