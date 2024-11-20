//
// Created by Chris Greer on 09/04/2024.
//

#ifndef G52CPP_SAVELOADUTIL_H
#define G52CPP_SAVELOADUTIL_H

#include "../../header.h"
#include "../ZEngine.h"
#include "../ZObjects/LivingObject.h"
#include <filesystem>
#include "../ZUtility/InfoStructs.h"

using namespace std;
namespace fs = filesystem;

//Class that handles the saving and loading of game data
class SaveLoadUtil{

private:
    //const inline static string savePath = "./resources/SaveDocs/";
    const static int totalSaveDocs = 7; //Used for bounds checking


public:
    static int getTotalDocs(){ return totalSaveDocs;}

    static bool saveGame(ZEngine *pEngine, int saveNumber, const string& saveName){

        //Get our (Various coordinates)
        vector<ObjectInfo> objectInfo = createCoordsVector(pEngine);
        vector<ObjectInfo> bloodCoordinates = pEngine->getBloodCoords();
        vector<KeyTile> keyTiles = pEngine->getKeyTiles();
        string savePath = "./resources/SaveDocs/";

        string saveFile = savePath + "Save" + to_string(saveNumber) + ".txt";
        ofstream mapDoc(saveFile);

        if (!mapDoc.is_open()) return false;

        //First write the save name to the top of the file
        mapDoc << saveName << endl;
        //Then write the Level Number below that
        mapDoc << pEngine->getLevelNumber() << endl;

        //Then write our tiles
        for (const auto& tiles : keyTiles) mapDoc << tiles << endl;

        //Write the blood coords to the file
        for (const auto& coord : bloodCoordinates) {
            mapDoc << 'D' << coord.typeKey << ' ' << coord.x << ' ' << coord.y << endl;
        }

        //Now loop through writing object coordinates to the file
        for (const auto& info : objectInfo) mapDoc << info << endl;

        mapDoc.close(); // Close the file
        objectInfo.clear();
        bloodCoordinates.clear();
        keyTiles.clear();
        return true;

    }


    static bool loadGame(ZEngine *pEngine, int saveNumber){

        vector<ObjectInfo> objectInfo;
        vector<ObjectInfo> bloodCoords;
        vector<KeyTile> keyTiles = vector<KeyTile>();
        string savePath = "./resources/SaveDocs/";

        string docName = savePath + "Save" + to_string(saveNumber) + ".txt";
        ifstream mapDoc(docName);

        if (!mapDoc.is_open()) return false;

        string line;
        //Read (and ignore) the save name line
        getline(mapDoc, line);
        //Then load the level number this file refers to
        if(getline(mapDoc, line))
            pEngine->setLevelNumber(line);
        else return false;

        int k = 0;
        //Scan the Main Unlocking Tiles
        while (mapDoc.peek() == 'K') { //Pulls in the tiles and their related info
            KeyTile tile(0,0);
            mapDoc >> tile;
            keyTiles.push_back(tile);
            k++;
        }
        pEngine->setTotalKeys(k); //Tell the engine how many keys this level

        //Then scan the tiles and their relevant keys
        while (mapDoc.peek() == 'T') { //Pulls in the tiles and their related info
            KeyTile tile(0,0);
            mapDoc >> tile;
            keyTiles.push_back(tile);

        }
        // Loop through and read blood coordinates from the file
        int mapX, mapY;
        while(mapDoc.peek() == 'D'){
            getline(mapDoc, line);
            if(!line.empty()) {
                if (sscanf(line.c_str(), "D0 %d %d", &mapX, &mapY) == 2) {
                    bloodCoords.push_back({mapX, mapY, 'D'});
                }
            }
        }
        //Now, using our overload to make this simpler (used to be 20+ lines!)
        //Pull in the object data
        ObjectInfo info{};
        while (mapDoc >> info) objectInfo.push_back(info);

        pEngine->setObjectInfo(objectInfo);
        pEngine->setBloodCoords(bloodCoords);
        pEngine->setKeyTiles(keyTiles);
        objectInfo.clear();
        bloodCoords.clear();
        keyTiles.clear();

        mapDoc.close();

        return true;
    }

    //Retrieves the names of each of the current saves you have
   //Used in the saving/loading screens to show current save files you have
    static vector<string> getSaveNames() {

        vector<string> saveNames(totalSaveDocs, "Empty"); //By default we set them to the string Empty
        string savePath = "./resources/SaveDocs/";

        //Iterate over the files in our save directory
        for (const fs::directory_entry& file : fs::directory_iterator(savePath)) {

            if (!file.is_regular_file()) continue; //Invalid file

            //Check name matches our Save# format
            string name = file.path().filename().string();
            if (name.find("Save") != 0 || name.size() != 9) continue; //Not a valid save file...

            //Get the save number from the file name
            int saveNum = stoi(name.substr(4)); //Work out which save number this is
            if (saveNum < 0 || saveNum > totalSaveDocs - 1) continue; //Not a valid save file...

            //Otherwise open the doc to find out the save name (first line)
            ifstream mapDoc(file.path());
            if (!mapDoc.is_open()) continue;
            //Read our doc to get the save name and level number
            string saveName;
            if (!getline(mapDoc, saveName)) continue; //Failed to read from this file (empty)

            //Get rid of the new line character
            saveName.erase(remove(saveName.begin(), saveName.end(), '\n'), saveName.end());

            string levelName; //Also get the level name
            if (!getline(mapDoc, levelName)) continue; //Again not a valid format (needs to contain level number)
            //Add a space after the word level (more readable)
            levelName.insert(levelName.begin() + 5, ' ');
            //Concat them together to show the details
            saveName += " [" + levelName + "]";
            //Add this to our vector at the appropriate place
            saveNames[saveNum] = saveName;

            mapDoc.close();

        }
        savePath.clear();
        return saveNames;
    }
    //Creates an object coordinates based on the current positions of each object
    static vector<ObjectInfo> createCoordsVector(ZEngine* pEngine) {

        vector<ObjectInfo> objectCoords;
        //Reserve space in our vector for all the objects currently in play
        objectCoords.reserve(pEngine->getNonNullObjectContentCount());

        //Add our single player object
        objectCoords.push_back({ pEngine->getPlayerCoords().x, pEngine->getPlayerCoords().y,
                                'P', 0, pEngine->getPlayer()->getHealth(),
                                pEngine->getPlayer()->getArmour(),pEngine->getPlayer()->getAmmo(), });

        //Iterate through the enemy objects and save them
        int i = 1; //Start from second object (After player)
        while (true) {
            auto* object = dynamic_cast<GameObject*>(pEngine->getDisplayableObject(i));
            if (object == nullptr) break; //Reached end of the objects so finish

            //Check if this is a character
            auto* character = dynamic_cast<LivingObject*>(pEngine->getDisplayableObject(i));
            if (character != nullptr) {
                //Save the Zombie coords to the vector, with all key information
                objectCoords.push_back({ character->getExactRealCenterX(), character->getExactRealCenterY(),
                                        character->getType()[0], character->getType()[1] - '0',
                                        character->getHealth(), character->getArmour() });
            }
            else { //Otherwise try to cast it to a static object
                auto* staticO = dynamic_cast<PickupObject*>(object);
                if (staticO == nullptr) continue; //Not a static object either, shouldn't happen that it's neither

                //If it is though, add it with the relevant info
                objectCoords.push_back({ staticO->getExactRealCenterX(), staticO->getExactRealCenterY(),
                                        staticO->getType()[0] });

            }

            i++;
        }

        return objectCoords;
    }


};

#endif //G52CPP_SAVELOADUTIL_H
