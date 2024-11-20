//
// Created by Chris Greer on 31/03/2024.
//

#ifndef G52CPP_ANIMATOR_H
#define G52CPP_ANIMATOR_H


#include "../../header.h"
#include "../ZPixels/ImagePixelRepo.h"

using namespace std;

class Animator {
public:
    template<typename T>
    //Animates depending on current time and time lag, returns whether it did update
    static bool animate(T& shownFrame,
                        const vector<T>& frames,
                        int currentTime,
                        int &counter,
                        int &lastUpdated,
                        int timeLag){

        bool updated = false;

        int timeBetween = (currentTime - lastUpdated );

        if(timeBetween > timeLag){
            counter++;
            lastUpdated = currentTime;
            updated = true;
        }

        counter = counter % static_cast<int>(frames.size());
        shownFrame = frames[counter];
        return updated;

    }


    //Paints a random blood splatter in the given location on the effects surface
    static void paintBlood(ZEngine *pEngine, int xVal, int yVal){
        //Get the blood image from our repo
        shared_ptr<vector<shared_ptr<SimpleImage>>> bloodImages = ImagePixelRepo::getMultiImages()->at("Blood");
        int randomImage = rand() % bloodImages->size();
        shared_ptr<SimpleImage> blood = (*bloodImages)[randomImage];

        xVal = xVal - blood->getWidth()/2;
        yVal = yVal - blood->getHeight()/2;

        //DrawingSurface* surface = pEngine->getSrcSurface();
        DrawingSurface* surface = pEngine->getEffectsSurface();
        surface->mySDLLockSurface();
        blood->renderImageWithMaskAndTransparency(surface,
                                                  0,0,
                                                  xVal,yVal,
                                                  blood->getWidth(),blood->getHeight(),
                                                  blood->getPixelColour(0,0),100);
        surface->mySDLUnlockSurface();
    }

};


#endif //G52CPP_ANIMATOR_H