//
// Created by Chris Greer on 24/04/2024.
//

#ifndef G52CPP_UISAVEUTIL_H
#define G52CPP_UISAVEUTIL_H

#include "UISaveLoadUtil.h"

//Class for handling the UI of just the Save screen UI
class UISaveUtil : public UISaveLoadUtil{

private:
    inline static int m_saveInputWidth = 300;
    inline static int m_saveInputHeight = 300;

public:
    //Handles the save input box, strings etc
    static void drawSaveInput(ZEngine* pEngine, DrawingSurface* surface, const string& input, bool clicked){
        int startX = 800; //General location of our box etc
        //Info string
        surface->drawFastString(startX, 80, "Enter Save Name", 0xFFFFFF,
                                pEngine->getFont("./resources/Fonts/Branda-yolq.ttf", 16));

        //How many characters so far so we know where to draw our cursor
        int characters = static_cast<int>(input.size());
        //Draw the box itself
        surface->drawRectangle(startX, 100, 1200, 200, 0x1A1A1A); //Shadow
        //If we have input, write it
        surface->drawFastString(
                startX + 20, 120, input.c_str(), 0xFFFFFF,
                pEngine->getFont("./resources/Fonts/Branda-yolq.ttf", 64));

        if (clicked && input.empty()){
            //Draw our cursor
            int cursorX = startX + 20 + characters*32; // <- Doesn't work great for certain characters (m specifically)
            surface->drawLine(cursorX, 110, cursorX, 190, 0x30D5C8);
        }

        if (characters == 0) return; //Nothing entered
        //Draw our button if there is some text
        drawButton(pEngine,pEngine->getForegroundSurface(),950,250,"Save Game");
    }
    //Check if you've clicked into the input box
    static bool saveInputClicked(int iX, int iY){
        return buttonPress(iX,iY, 800,100,m_saveInputWidth,m_saveInputHeight);
    }
    //Check you've clicked the "Save" Button
    static bool saveButtonClicked(int iX, int iY){
        return buttonPress(iX,iY, 950,250,m_buttonWidth,m_buttonHeight);
    }

    static void drawSaveSuccess(ZEngine* pEngine, DrawingSurface* surface, bool success){
        string dialogue;
        if (success){
            dialogue = "Game Saved Successfully!";
        } else {
            dialogue = "Saving Failed";
        }
        surface->drawFastString(
                800, 200, dialogue.c_str(), 0xFFFFFF,
                pEngine->getFont("./resources/Fonts/Branda-yolq.ttf", 32));
    }

};

#endif //G52CPP_UISAVEUTIL_H
