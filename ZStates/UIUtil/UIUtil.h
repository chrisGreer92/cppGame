//
// Created by Chris Greer on 23/04/2024.
//

#ifndef G52CPP_UIUTIL_H
#define G52CPP_UIUTIL_H

#include "../../../header.h"
#include "../../ZEngine.h"

class UIUtil{
//General class for helping simplify UI building (UI is not my strong point...)
protected:
    inline static int m_buttonWidth = 260;
    inline static int m_buttonHeight = 100;
public:

    //Check if we've clicked within a button
    static bool buttonPress(int iX, int iY, int buttonX, int buttonY){
        return (iX > buttonX && iX < buttonX + m_buttonWidth && iY > buttonY && iY < buttonY + m_buttonHeight);
    }

    static bool buttonPress(int iX, int iY, int buttonX, int buttonY, int buttonW, int buttonH){
        return (iX > buttonX && iX < buttonX + buttonW && iY > buttonY && iY < buttonY + buttonH);
    }

    static void drawButton(ZEngine* pEngine, DrawingSurface* surface, int startX, int startY, const char* msg){

        //Default hardcoded button size
        drawShadowedBox(surface, startX,startY,m_buttonWidth,m_buttonHeight, false);
        //Label
        surface->drawFastString(
                startX + 15,
                startY + 28,
                msg, 0xFFFFFF,
                pEngine->getFont("./resources/Fonts/Branda-yolq.ttf", 48));
    }

    static void drawShadowedBox(DrawingSurface* surface, int startX, int startY,
                                int width, int height, bool highlighted = false){
        int endX = startX + width; //Default button size
        int endY = startY + height;

        surface->drawRectangle(startX, startY, endX + 6, endY + 6,0x1A1A1A); //Shadow
        if (highlighted)
            surface->drawRectangle(startX + -2, startY + -2, endX + 2,endY + 2,0x30D5C8); //Highlighted Border
        else
            surface->drawRectangle(startX + -2, startY + -2, endX + 2,endY + 2,0x2F2F2F); //Border

        surface->drawRectangle(startX, startY, endX, endY, 0x460000); //Red Button
    }



};

#endif //G52CPP_UIUTIL_H
