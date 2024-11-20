//
// Created by Chris Greer on 07/04/2024.
//

#ifndef G52CPP_PIXELMAPCREATOR_H
#define G52CPP_PIXELMAPCREATOR_H

#include "../../header.h"
#include <vector>
#include "../../SimpleImage.h"

using namespace std;
//Define our pixel Map (2d array/vector of bools)
using PixelMap = vector<vector<bool>>;
//Used to create pixel maps for each pixel that isn't a transparent background
//Used for game objects and images to allow for pixel perfect collision detection
class PixelMapCreator{
public:

    //Creates and returns a 2d array of booleans indicating either coloured or not
    static PixelMap createPixelMap(const shared_ptr<SimpleImage>& m_image, int maskColour) {

        int width = m_image->getWidth();
        int height = m_image->getHeight();

        auto pixelMap = PixelMap(height, vector<bool>(width));

        //Iterate through each pixel
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                ///If it matches the mask colour then false, otherwise true
                pixelMap[y][x] = m_image->getPixelColour(x, y) != maskColour;
            }
        }

        return pixelMap;

    }
    //Creates an array of pointers to pixel maps (which are in turn arrays of arrays of bools!)
    static shared_ptr<vector<PixelMap>> createPixelMaps(const vector<shared_ptr<SimpleImage>>& images,
                                                        int maskColour) {
        vector<PixelMap> pixelMaps;

        //Set up the size of our array based on number of images
        pixelMaps.reserve(images.size());
        for (const auto& image : images) {
            //Create pixel map for each image and add it to the array
            pixelMaps.push_back(createPixelMap(image, maskColour));
        }

        return make_shared<vector<PixelMap>>(pixelMaps);
    }
    

    //Creates an array of pointers to Front Only pixel maps (for melee)
    static shared_ptr<vector<PixelMap>> createPixelMaps(const vector<shared_ptr<SimpleImage>>& images, int maskColour, double centreY) {
        vector<PixelMap> pixelMaps;

        //Set up the size of our array based on number of images
        pixelMaps.reserve(images.size());
        for (const auto& image : images) {
            //Create pixel map for each image and add it to the array
            pixelMaps.push_back(createPixelMap(image, maskColour,centreY));
        }

        return make_shared<vector<PixelMap>>(pixelMaps);
    }

    //Creates and Pixel map BUT only for the bottom (i.e. front) half of the image (for melee attacks)
    static PixelMap createPixelMap(const shared_ptr<SimpleImage>& m_image, int maskColour, double centreY){
        int width = m_image->getWidth();
        int height = m_image->getHeight();

        //Determine where the center starts
        int intCentreY = static_cast<int>(height * centreY);

        //Create a pixel map for the bottom half
        auto pixelMap = PixelMap(height, vector<bool>(width));

        //Iterate through each pixel from the middle to the bottom
        for (int y = intCentreY; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                ///If it matches the mask colour then false, otherwise true
                int colour = m_image->getPixelColour(x, y);
                pixelMap[y][x] = colour != maskColour;
            }
        }

        // Return the pointer to the pixel map
        return pixelMap;

    }




};

#endif //G52CPP_PIXELMAPCREATOR_H
