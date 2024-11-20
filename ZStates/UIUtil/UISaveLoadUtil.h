//
// Created by Chris Greer on 23/04/2024.
//

#ifndef G52CPP_UISAVELOADUTIL_H
#define G52CPP_UISAVELOADUTIL_H

#include "UIUtil.h"
#include "../../ZUtility/SaveLoadUtil.h"

class UISaveLoadUtil : public UIUtil{

protected:
    inline static int m_existingSaveWidth = 500;
    inline static int m_existingSaveHeight = 100;

public:

    //Draw the default UI for both saving and loadiing
    static void drawSaveLoadUI(ZEngine* pEngine, DrawingSurface* surface){

        drawAllExistingSaves(pEngine, surface, 100, 50);
        drawButton(pEngine, surface,900,640,"Go Back");
    }

    //Check whether the back button was clicked on the save/load UI screen(s)
    static bool saveLoadUIBack(int iX, int iY){
        return buttonPress(iX,iY,900,640);
    }

    //Highlight the box that's been selected so it's clear which one you did choose
    static void highlightExistingSave(ZEngine* pEngine, DrawingSurface* highlightSurface, int boxNumber){
        //boxNumber -= 1; //Remove 1 from the number since we DO use 0 as first index here
        vector<string> saveNames = SaveLoadUtil::getSaveNames();
        //Redraw the save box but with highlighting
        drawExistingSave(pEngine, highlightSurface, 100, 50 + boxNumber * m_existingSaveHeight,
                         boxNumber, saveNames.at(boxNumber).c_str(), true);

    }

    //Return the number of save file clicked or -1 if none of them
    static int existingSaveClick(int iX, int iY){
        for (int i = 0; i < SaveLoadUtil::getTotalDocs() ; ++i) {
            if (buttonPress(iX,iY,100, 50 + i * m_existingSaveHeight,
                            m_existingSaveWidth,m_existingSaveHeight)){
                return i;
            }
        }
        return -1; //Not in any of them
    }

    //Draw all 7 save Boxes, loads the SaveFileNames from the directory using the SaveLoadUtil
    static void drawAllExistingSaves(ZEngine* pEngine, DrawingSurface* surface, int startX, int startY){
        vector<string> saveNames = SaveLoadUtil::getSaveNames();
        //Load our save file (use SaveLoadUtil)
        for (int i = 0; i < SaveLoadUtil::getTotalDocs(); ++i) { //Draw boxes for each with the relevent save names (or Empty)
            drawExistingSave(pEngine, surface, startX, startY + i * m_existingSaveHeight, i, saveNames.at(i).c_str(),
                             false);
        }
    }
    //Draw a single SaveBox
    static void drawExistingSave(ZEngine* pEngine, DrawingSurface* surface, int startX, int startY,
                                 int saveNum, const char* name, bool highlighted){
        drawShadowedBox(surface, startX, startY, m_existingSaveWidth, m_existingSaveHeight, highlighted);
        char fullString[16];
        if (saveNum == 0){
            snprintf(fullString, 16, "Autosave Slot");
        } else{
            snprintf(fullString, 16, "Save: %d", saveNum);
        }
        

        surface->drawFastString(
                startX + 10,
                startY + 10,
                fullString, 0xFFFFFF,
                pEngine->getFont("./resources/Fonts/Branda-yolq.ttf", 16));

        surface->drawFastString(
                startX + 25,
                startY + 40,
                name, 0xFFFFFF,
                pEngine->getFont("./resources/Fonts/Branda-yolq.ttf", 32));
    }


    //Just for loading!

    static void drawLoadButton(ZEngine* pEngine, DrawingSurface* surface){
        UIUtil::drawButton(pEngine,surface,900,500,"Load Game");
    }
    static bool loadButtonClicked(int iX, int iY){
        return buttonPress(iX,iY,900,500);
    }
};

#endif //G52CPP_UISAVELOADUTIL_H
