//
// Created by Chris Greer on 31/03/2024.
//

#ifndef G52CPP_MAPOFFSETFILTER_H
#define G52CPP_MAPOFFSETFILTER_H


#include "../../FilterPoints.h"
#include "../../header.h"

//My FilterPoints class, only concerned with providing offset logic for map and non-player objects
class MapOffsetFilter : public FilterPoints {

public:
    MapOffsetFilter(int xModifier = 0, int yModifier = 0, FilterPoints* pNextFilter = nullptr)
            : m_offsetX(xModifier), m_offsetY(yModifier), pNextFilter(pNextFilter)
    {
    }

    //This is the only difference between our filter class and others
    //Literally just turns off the filter that says you can't draw off the screen, we want to draw off the screen...
    bool filter(DrawingSurface* surface, int& xVirtual, int& yVirtual, unsigned int& uiColour, bool setting) override
    {
        //For our map filter we don't check for anything to do with whether it's on or off the map...
        //If we remove nextFilter we can just have this return true...
        return (pNextFilter == nullptr) || pNextFilter->filter(surface, xVirtual, yVirtual, uiColour, setting);
    }

    //Take the virtual location (relative to player) and return the real location (on large surface)
    int filterConvertVirtualToRealXPosition(int virtualX) override
    {
        //Apply other filter first?
        if (pNextFilter) return pNextFilter->filterConvertVirtualToRealXPosition(virtualX + m_offsetX);

        return virtualX + m_offsetX;
    }

    //Take the virtual location (relative to player) and return the real location (on large surface)
    int filterConvertVirtualToRealYPosition(int virtualY) override
    {
        //Apply other filter first?
        if (pNextFilter)
            return pNextFilter->filterConvertVirtualToRealYPosition(virtualY + m_offsetY);
        return virtualY + m_offsetY;
    }

    //Take the real location (on large surface) and return the virtual location (relative to player)
    int filterConvertRealToVirtualXPosition(int realX) override
    {
        //Apply other filter first?
        if (pNextFilter)
            realX = pNextFilter->filterConvertRealToVirtualXPosition(realX);
        return realX - m_offsetX;
    }

    //Take the real location (on large surface) and return the virtual location (relative to player)
    int filterConvertRealToVirtualYPosition(int realY) override
    {
        //Apply other filter first?
        if (pNextFilter)
            realY = pNextFilter->filterConvertRealToVirtualYPosition(realY);
        return realY - m_offsetY;
    }

    //Set the offset amount
    void setOffset(int offsetX, int offsetY)
    {
        m_offsetX = offsetX;
        m_offsetY = offsetY;
    }

    int getXOffset() const { return m_offsetX; }
    int getYOffset() const { return m_offsetY; }

private:
    FilterPoints* pNextFilter; //Maybe remove as not using currently
    int m_offsetX, m_offsetY;
};

#endif //G52CPP_MAPOFFSETFILTER_H
