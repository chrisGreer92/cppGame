//
// Created by Chris Greer on 21/04/2024.
//

#ifndef G52CPP_ISTATEHANDLER_H
#define G52CPP_ISTATEHANDLER_H

//Interface for methods that the different states will implement in order to handle things differently depending on state
//#include "ZEngine.h"
//#include "MovementUtil.h"

class ZEngine;
class MovementUtil;

class iStateHandler{

public:
    explicit iStateHandler(ZEngine* pEngine) : m_pEngine(pEngine){}
    virtual ~iStateHandler() = default;

    virtual void setUpBackgroundBuffer() = 0;
    //Some states just copy from the background buffer
    //Any that don't can override
    virtual void copyAllBackgroundBuffer(){
        m_pEngine->getForegroundSurface()->
                copyEntireSurface(m_pEngine->getBackgroundSurface());
    };
    virtual void postDraw() = 0;

    virtual void beforeUpdate() = 0;
    virtual void postUpdate() = 0;

    virtual void handleMouseDown(int iButton, int iX, int iY) = 0;
    virtual void handleMouseUp(int iButton, int iX, int iY) = 0;
    virtual void handleKeyDown(int iKeyCode) = 0;
    virtual void handleKeyUp(int iKeyCode) = 0;

    ZEngine *m_pEngine;
};

#endif //G52CPP_ISTATEHANDLER_H
