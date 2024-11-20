//
// Created by Chris Greer on 21/04/2024.
//

#ifndef G52CPP_SURFACEMANAGER_H
#define G52CPP_SURFACEMANAGER_H

#include <utility>

#include "../../header.h"
#include "../ZEngine.h"
#include "../ZMaps/MapLoader.h"
#include "../ZMaps/MapTileManager.h"
#include "../../DrawingSurface.h"

//Responsible for loading and maintaining surfaces, tile managers and offsets
//Called on when initialising a level to set these up
class SurfaceManager {

public:
    ~SurfaceManager() {
        m_collisionMap.reset();
        //m_effectsSurface.reset();
        m_mapFilter.reset();
        m_srcSurface.reset();
        m_hudSurface.reset();
        m_waveSurface.reset();
        for (auto& wave : m_backgroundWaves) { wave.reset(); }
        m_backgroundWaves.clear();
    }
    shared_ptr<MapTileManager> getCollisionMap() const { return m_collisionMap; }
    DrawingSurface* getEffectsSurface() const { return m_effectsSurface.get(); }
    DrawingSurface* getSrcSurface() const { return m_srcSurface.get(); }
    shared_ptr<MapOffsetFilter> getOffsetFilter() const { return m_mapFilter; }

protected:
    void setUpSurfaces(ZEngine* pEngine){

        setUpWavesBackground(pEngine);
        setUpSourceSurface(pEngine);
        //loadLevelMaps(pEngine, level);
        setUpEffectsSurface(pEngine);
        initialiseHUD(pEngine);
        m_collisionMap = make_shared<MapTileManager>(pEngine, pEngine->getTileSize(), pEngine->getTileSize());

    }

    void loadLevelMaps(ZEngine* pEngine, const string& level) {

        //First clear our main surfaces in case we drew on them earlier
        
       /* m_effectsSurface->fillSurface(0);
        m_srcSurface->fillSurface(0);*/
        m_effectsSurface->setAlpha(0);
        m_srcSurface->setAlpha(0);
        

        //Load up our source map layers
        //First layer is just the general background grass etc
        MapLoader::loadTileMap(pEngine, m_srcSurface.get(),
            pEngine->getTilesX(), pEngine->getTilesY(),
            "./resources/TileMaps/" + level + "/GrassTiles.txt");

        //Second layer is floors etc
        MapLoader::loadTileMap(pEngine, m_srcSurface.get(),
            pEngine->getTilesX(), pEngine->getTilesY(),
            "./resources/TileMaps/" + level + "/FloorTiles.txt");

        //Load our second layer which determines collision
        //(Along with being out of bounds, maybe do this as the wave tile type)
        //m_collisionMap.reset();
        //m_collisionMap = make_shared<MapTileManager>(pEngine, pEngine->getTileSize(), pEngine->getTileSize());
        m_collisionMap->setTopLeftPositionOnScreen(0, 0);
        m_collisionMap->setMapSize(pEngine->getTilesX(), pEngine->getTilesY());
        MapLoader::loadTileMap(pEngine, m_srcSurface.get(), m_collisionMap.get(),
            "./resources/TileMaps/" + level + "/CollisionTiles.txt");
    }


    //Draw the actual information to the hud
    void drawHudInfo(ZEngine* pEngine, DrawingSurface* surface){

        Font *m_font32 = pEngine->getFont("./resources/Fonts/Branda-yolq.ttf", 32);
        ZPlayer* player = pEngine->getPlayer();

        //Draw Current Health
        drawStatBar(pEngine,player->getHealth(),170,180,750,0x03C04A);
        //Draw Current Armour
        drawStatBar(pEngine,player->getArmour(),170,550,750,0x0CCCCC);
        if (player->getWeapon() == ZPlayer::w_pistol){
            //Draw Current Weapon
            surface->drawFastString(950, 735, "PISTOL", 0xFFFFFF, m_font32);
            //Draw ammo count
            surface->drawFastString(1190, 735, "INF.", 0xFFFFFF, m_font32);
        } else if(player->getWeapon() == ZPlayer::w_rifle){
            //Draw Current Weapon
            surface->drawFastString(950, 735, "RIFLE", 0xFFFFFF, m_font32);
            //Draw ammo count
            surface->drawFastString(1190, 735, to_string(player->getAmmo()).c_str() , 0xFFFFFF, m_font32);
        }

    }

    void setUpSourceSurface(ZEngine* pEngine) {
        //Create our larger 'source' background where we will blit from
        m_srcSurface.reset();
        m_srcSurface = make_shared<DrawingSurface>(pEngine);
        m_srcSurface->createSurface(pEngine->getTilesX() * pEngine->getTileSize(),
            pEngine->getTilesY() * pEngine->getTileSize());
        m_srcSurface->removeBoundsCheck();
        //Need to set this to nullptr to avoid having issues drawing 'outside' the screen
        m_srcSurface->setDrawPointsFilter(nullptr);
    }


    void setUpEffectsSurface(ZEngine* pEngine){

        //Create our larger 'effects' background where we will blit effects from
        m_effectsSurface.reset();
        m_effectsSurface = make_shared<DrawingSurface>(pEngine);
        m_effectsSurface->removeBoundsCheck();
        m_effectsSurface->setDrawPointsFilter(nullptr);
        m_effectsSurface->createSurface(pEngine->getTilesX() * pEngine->getTileSize(),
                                        pEngine->getTilesY() * pEngine->getTileSize());
    }

private:
    void setUpWavesBackground(ZEngine* pEngine){
        shared_ptr<DrawingSurface> backSource = make_shared<DrawingSurface>(pEngine);

        backSource->setDrawPointsFilter(nullptr);
        backSource->createSurface(pEngine->getTilesX() * pEngine->getTileSize(),
                                  pEngine->getTilesY() * pEngine->getTileSize());
        MapLoader::loadTileMap(pEngine, backSource.get(),
                             pEngine->getTilesX() * pEngine->getTileSize(),
                             pEngine->getTilesY() * pEngine->getTileSize(),
                               "./resources/TileMaps/WavesTiles.txt");
        //Set our our multiple waves surfaces
        for (int i = 0; i < 16; ++i) {
            shared_ptr<DrawingSurface> back = make_shared<DrawingSurface>(pEngine);
            back->createSurface(pEngine->getWindowWidth(),pEngine->getWindowHeight());
            back->copyRectangleFrom(backSource.get(),
                                    0,0,
                                    pEngine->getWindowWidth(),pEngine->getWindowHeight(),
                                    i*4, 0);
            m_backgroundWaves.push_back(back);
        }
        backSource.reset();
    }

    

    void initialiseHUD(ZEngine* pEngine) {

        Font *m_font32 = pEngine->getFont("./resources/Fonts/Branda-yolq.ttf", 32);

        m_hudSurface.reset();
        m_hudSurface = make_shared<DrawingSurface>(pEngine);
        m_hudSurface->setDrawPointsFilter(nullptr); //Remove the drawing issues
        m_hudSurface->createSurface(pEngine->getWindowWidth(), //Can draw anywhere but will only draw to small area
                                    pEngine->getWindowHeight());
        m_hudSurface->mySDLLockSurface();


        SimpleImage hudBack = pEngine->loadImage("./resources/SurfaceImages/HUDBack.png");
        hudBack.renderImage(m_hudSurface.get(), 0, 0, 0, 700,
                            hudBack.getWidth(), hudBack.getHeight());

        //Label Health
        m_hudSurface->drawFastString(28, 735, "HEALTH:", 0xFFFFFF, m_font32);
        //Draw Health Background
        m_hudSurface->drawThickLine(180, 750, 350, 750, 0xFF0000, 40);
        //Label armour
        m_hudSurface->drawFastString(430, 735, "ARMOR:", 0xFFFFFF, m_font32);
        //Draw Armour Background
        m_hudSurface->drawThickLine(550, 750, 720, 750, 0x808080, 40);
        //Weapon Label
        m_hudSurface->drawFastString(770, 735, "EQUIPPED:", 0xFFFFFF, m_font32);
        //Ammo Label
        m_hudSurface->drawFastString(1080, 735, "AMMO:", 0xFFFFFF, m_font32);
        m_hudSurface->mySDLUnlockSurface();
    }

    void drawStatBar(ZEngine* pEngine, int stat, int barSize, int locX, int locY, int fillColour){

        if (stat > 0) {
            float totalLeft = static_cast<float>(barSize) * (static_cast<float>(stat)/100);
            pEngine->getForegroundSurface()->drawThickLine(locX, locY,
                                                           locX + totalLeft, locY,
                                                           fillColour, 40);
        }


    }


protected:
    shared_ptr<MapOffsetFilter> m_mapFilter = nullptr;
    shared_ptr<MapTileManager> m_collisionMap = nullptr;
    shared_ptr<DrawingSurface> m_srcSurface = nullptr;
    shared_ptr<DrawingSurface> m_effectsSurface = nullptr; //For blood effects
    shared_ptr<DrawingSurface> m_hudSurface = nullptr; //Needs to be drawn on last
    shared_ptr<DrawingSurface> m_waveSurface = nullptr; //Needs to be drawn on last
    vector<shared_ptr<DrawingSurface>> m_backgroundWaves;
    int m_wavesCounter = 0;
    int m_wavesLastUpdated = 0;
};

#endif //G52CPP_SURFACEMANAGER_H
