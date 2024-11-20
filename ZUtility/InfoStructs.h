//
// Created by Chris Greer on 28/04/2024.
//

#ifndef G52CPP_INFOSTRUCTS_H
#define G52CPP_INFOSTRUCTS_H

#include "../../header.h"
#include <iostream>

using namespace std;

//Coordinate/Information structure used for saving/loading objects
struct ObjectInfo {
    int x;
    int y;
    char type; //Used to determine type (optional)
    int typeKey; //Represents the type key (i.e. diff skins of same type)
    int health;
    int armour;
    int ammo;
    //Overloaded << in order to simplify outputting (for saving data )
    inline friend ostream& operator<<(ostream& out, const ObjectInfo& info) {
        out << info.type << ' ' << info.typeKey << ' ' << info.x << ' ' << info.y <<
            ' ' << info.health << ' ' << info.armour << ' ' << info.ammo;
        return out;
    }
    //And equivalently overloading >> in order to simplify loading the same data
    inline friend istream& operator>>(istream& in, ObjectInfo& info) {
        return in >> info.type >> info.typeKey >> info.x >> info.y >> info.health >> info.armour >> info.ammo;
    }
};

//Structure to contain which tiles are keys and which tiles they then unlock
struct KeyTile{
    int x = 0;
    int y = 0;
    int newMap = 0;
    char type = 'x';
    bool triggered = false;
    vector<KeyTile> unlocksTiles;

    // Constructor to initialize x and y coordinates
    KeyTile(int tileX, int tileY) : x(tileX), y(tileY), triggered(false) {};
    ~KeyTile() { unlocksTiles.clear(); }

    inline friend ostream& operator<<(ostream& out, const KeyTile& tile) {
        //First output this tile's info, adding type and whether it's triggered at the start
        out << tile.type << ' ' << (tile.triggered ? 1 : 0) << ' ' << tile.x << ' ' << tile.y;
        //Then for each of the related tiles print same but with their new map value and no triggered flag
        for (KeyTile tiles : tile.unlocksTiles) {
            out << endl; //Add a new line as adding another
            out << tiles.type << ' ' << tiles.x << ' ' << tiles.y << ' ' << tiles.newMap;
        }

        return out;
    }

    //And equivalently overloading >> in order to simplify loading the same data
    inline friend istream& operator>>(istream& in, KeyTile& tile) {

        int trig;
        in >> tile.type >> trig >> tile.x >> tile.y; //Pull in info on main tile
        tile.triggered = (trig == 1); //Set whether it's triggered (might be loading from save)
        in.ignore(numeric_limits<std::streamsize>::max(), '\n'); //Skip to next line

        //Get all the related unlocking tiles
        while (in.peek() == 'U') {
            KeyTile unlocks(0, 0);
            in >> unlocks.type >> unlocks.x >> unlocks.y >> unlocks.newMap; //Include their new map value to draw them
            tile.unlocksTiles.push_back(unlocks); //Add this to related tiles it unlocks
            in.ignore(numeric_limits<std::streamsize>::max(), '\n'); //Skip to next line
        }

        return in;
    }
};
#endif //G52CPP_INFOSTRUCTS_H
