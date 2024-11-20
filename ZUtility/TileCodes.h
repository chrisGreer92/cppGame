//
// Created by Chris Greer on 27/04/2024.
//

#ifndef G52CPP_TILECODES_H
#define G52CPP_TILECODES_H

//Class to handle hardcoded tile values that relate to various things
//This way don't need long IF statemenets throughout the code that refer to the same tiles
//Also means one place if I move tiles around in the raw data or add new ones.
class TileCodes {

public:
    //General collision tile (anything that you can't walk onto
    static bool isCollisionTile(int mapValue){
        return ((mapValue >= 15 && mapValue <= 26) //Our collision tiles
            || (mapValue >= 30 && mapValue <= 41)
            || (mapValue >= 45 && mapValue <= 60) ||
            (mapValue >= 66 && mapValue <= 123));
    }

    //Only tiles that actually block line of sight (i.e. laser would get stopped by these)
    static bool isLosBlockingTile(int mapValue){
        return (mapValue >= 30 && mapValue <= 41) || (mapValue >= 45 && mapValue <= 60) ||
               (mapValue >= 66 && mapValue <= 123);
    }
    //Just for storing a reference to specific tiles used throughout
    static bool doorKey(int mapValue){ return mapValue == 126; }
    static bool mainKey(int mapValue){ return mapValue == 124; }
    static int inactiveKey() {return 125;}
    static bool progressArea(int mapValue){ return mapValue == 2;}
    static bool zombie(int mapValue){ return mapValue == 64; }
    static bool speedy(int mapValue){ return mapValue == 63; }
    static bool armouredZombie(int mapValue){ return mapValue == 65; }


    static bool armorItem(int mapValue){ return mapValue == 127; }

    static bool ammoItem(int mapValue){ return mapValue == 128; }

    static bool healthItem(int mapValue){ return mapValue == 129; }
};
#endif //G52CPP_TILECODES_H
