//
// Created by Chris Greer on 09/04/2024.
//

#ifndef G52CPP_IMAGEPIXELREPO_H
#define G52CPP_IMAGEPIXELREPO_H

#include "ImageLoader.h"
#include "../../header.h"
#include "PixelMapCreator.h"
#include <sstream>

using namespace std;
//Used to store all the references to object images and their pixel maps
//Though image loading is already set up to avoid re-loading,
//this avoids pixel maps being created multiple times for same image
//Also, makes for cleaner code and a single location I can update paths/directories if/when they change
//There are however some UI images that are hardcoded, mostly in surfaceLoader
class ImagePixelRepo{

public:
    
    static void initialise(){

        //Set up our maps
        m_singleImages = make_unique<map<string, shared_ptr<SimpleImage>>>();
        m_singlePixelMaps = make_unique<map<string, shared_ptr<PixelMap>>>();
        m_multiImages = make_unique<map<string, shared_ptr<vector<shared_ptr<SimpleImage>>>>>();
        m_multiPixelMaps = make_unique<map<string, shared_ptr<vector<PixelMap>>>>();

        string path = "./resources/";


        //Load our tileMap image
        loadSingleImage(path + "TileMaps/TilesImage.png", "Tiles",true);

        //Load our Level Maps
        loadSingleImage(path + "TileMaps/LevelOne/Map.jpeg", "LevelOne");
        loadSingleImage(path + "TileMaps/LevelTwo/Map.jpeg", "LevelTwo");
        loadSingleImage(path + "TileMaps/LevelThree/Map.jpeg", "LevelThree");

        //Load Pickup Items
        loadSingleImage(path + "Pickups/Health.png", "H", true);
        loadSingleImage(path + "Pickups/Armor.png", "B", true);
        loadSingleImage(path + "Pickups/Ammo.png", "R", true);

        //Load player
        loadDirectory(path + "Player/Pistol", "PistolWalk", true);
        loadDirectory(path + "Player/Rifle", "RifleWalk", true);
        loadDirectory(path + "Player/RifleShot", "RifleShot", true);
        loadDirectory(path + "Player/PistolShot", "PistolShot", true);
        //Center point in the bat means you only collide with enemies that are in front, more natural
        loadDirectory(path + "Player/Bat", "BatAttack", true, 0.4);

        //Load the multiple skins of various enemy types
        loadMultipleEnemies("Z", 3);
        loadMultipleEnemies("A", 2);
        loadMultipleEnemies("S", 1);
        //Other ones too...

        //Load Map Tile Details images
        loadDirectory(path + "Details/GrassDetails", "GrassDetails");
        loadDirectory(path + "Details/BloodDetails", "BloodDetails");
        loadDirectory(path + "Details/Cracks", "CracksDetails");
        //Load our blood splat images
        loadDirectory(path + "Blood", "Blood");
    }
    static map<string, shared_ptr<SimpleImage>>* getSingleImages(){ return m_singleImages.get();};
    static map<string, shared_ptr<PixelMap>>* getSinglePixelMaps(){return m_singlePixelMaps.get();};
    static map<string, shared_ptr<vector<shared_ptr<SimpleImage>>>>* getMultiImages(){ return m_multiImages.get();};
    static map<string, shared_ptr<vector<PixelMap>>>* getMultiPixelMaps(){return m_multiPixelMaps.get();};

private:
    static void loadDirectory(const string& directoryName, const string& keyString,
                              bool createPixelMap = false, double centreY = 0, int maskColour = 0){

        auto images = make_shared <vector<shared_ptr<SimpleImage>>>(ImageLoader::loadImagesFromDirectory(directoryName));
        //If we're creating pixel map...
        if (createPixelMap){
            shared_ptr<vector<PixelMap>> pixelMaps;
            //...First check if this is a centred map
            if (centreY != 0)
                pixelMaps = PixelMapCreator::createPixelMaps(*images, maskColour, centreY);
            else
                pixelMaps = PixelMapCreator::createPixelMaps(*images, maskColour);

            m_multiPixelMaps->insert({keyString, std::move(pixelMaps)});
        }
        m_multiImages->insert({keyString, std::move(images)});

    }

    static void loadMultipleEnemies(const string& prefix, int total){

        string path = "./resources/";

        for (int i = 0; i < total; ++i) {
            string pref = prefix + to_string(i);
            stringstream stream;
            stream << path << "Enemies/" << pref;
            string fullPath = stream.str();
            loadDirectory(fullPath + "Walk", pref + "Walk", true);
            loadDirectory(fullPath + "Attack", pref + "Attack", true);
            loadDirectory(fullPath + "Death", pref + "Death");
        }

    }

    static void loadSingleImage(const string& singPath, const string& keyString,
                                bool createPixelMap = false,
                                double centreY = 0, int maskColour = 0){

        shared_ptr<SimpleImage> image = make_shared<SimpleImage>(ImageManager::loadImage(singPath));

        if (createPixelMap){
            shared_ptr<PixelMap> pixelMap;
            //...First check if this is a centred map
            if (centreY != 0)
                pixelMap = make_shared<PixelMap>(PixelMapCreator::createPixelMap(image, maskColour, centreY));
            else
                pixelMap = make_shared<PixelMap>(PixelMapCreator::createPixelMap(image, maskColour));

            m_singlePixelMaps->insert({keyString, std::move(pixelMap)});
        }
        m_singleImages->insert({keyString, std::move(image)});
    }

public:
    static void deleteRepo() {

        for (auto& pair : *m_singleImages) { pair.second.reset(); }
        m_singleImages.reset();

        for (auto& pair : *m_singlePixelMaps) { pair.second.reset(); }
        m_singlePixelMaps.reset();

        for (auto& pair : *m_multiImages) {
            for (auto& ptr : *(pair.second)) { ptr.reset(); }
            pair.second->clear();
            pair.second.reset();
        }
        m_multiImages.reset();

        for (auto& pair : *m_multiPixelMaps) {
            pair.second->clear();
            pair.second.reset();
        }
        m_multiPixelMaps.reset();
    }

private:
    //All the multi images
    static inline unique_ptr<map<string, shared_ptr<vector<shared_ptr<SimpleImage>>>>> m_multiImages;
    //All the multi pixel maps
    static inline unique_ptr<map<string, shared_ptr<vector<PixelMap>>>> m_multiPixelMaps;
    //All the single images
    static inline unique_ptr<map<string, shared_ptr<SimpleImage>>> m_singleImages;
    //All the single pixel maps
    static inline unique_ptr<map<string, shared_ptr<PixelMap>>> m_singlePixelMaps;
};
#endif //G52CPP_IMAGEPIXELREPO_H
