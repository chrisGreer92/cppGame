//
// Created by Chris Greer on 23/03/2024.
//

#ifndef G52CPP_IMAGELOADER_H
#define G52CPP_IMAGELOADER_H

#include <filesystem>
#include "../../header.h"
#include "../../SimpleImage.h"

#include "../ZEngine.h"

using namespace std;

//Extension to the ImageManager, handles bulk loading of images
class ImageLoader : public ImageManager {

public :

    static vector<shared_ptr<SimpleImage>> loadImagesFromDirectory(const string& directory){

        //Need to use a name/image pair so that I can sort them...
        vector<pair<string, shared_ptr<SimpleImage>>> imagesAndNames;

        //For each file in the directory
        for (const auto& entry : filesystem::directory_iterator(directory)) {
            //Check if regular
            if (entry.is_regular_file()) {
                //Get path
                string filePath = entry.path().string();

                //Check if it's a valid image file
                if (isValidImage(filePath)) {
                    //Load image and add to vector
                    shared_ptr<SimpleImage> image =
                            make_shared<SimpleImage>(loadImage(filePath, false)); //Will only load these once
                    imagesAndNames.emplace_back(filePath, image);
                }
            }
        }

        //Sort images by name (since the animations are ordered)
        sort(imagesAndNames.begin(), imagesAndNames.end(),
                  [&](const pair<string, shared_ptr<SimpleImage>>& a,
                          const pair<string, shared_ptr<SimpleImage>>& b) {
            return a.first < b.first;
        });

        //Create a vector list of just the images themselves to return
        vector<shared_ptr<SimpleImage>> images;
        //Set size since we know how many we're adding
        images.reserve(imagesAndNames.size());
        for (const auto& pair : imagesAndNames) {
            images.push_back(pair.second);
        }

        return images;
    };

private:
    static bool isValidImage(const string& path) {
        //List of valid image extensions
        const vector<string> validExtensions = {".png", ".jpg", ".jpeg", ".gif", ".bmp"};

        //Get this extension
        size_t dotIndex = path.find_last_of('.');
        if (dotIndex != string::npos) {
            string extension = path.substr(dotIndex);
            //check the valid extensions against the one of this file
            for (const string& valid : validExtensions) {
                if (extension == valid) {
                    return true;
                }
            }
        }
        return false;
    }
};

#endif //G52CPP_IMAGELOADER_H