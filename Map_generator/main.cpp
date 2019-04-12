//
//  main.cpp
//  Map_generator
//
//  Companion app to 2Dot to generate a custom map from a png image
//
//  Created by William Cole on 4/10/19.
//  Copyright © 2019 William Cole. All rights reserved.
//

#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>
#include <cstdio>
#include <fstream>
#include "lTexture.h"

using namespace std;

string inputImgFile = "level_1.png";
string outputMapFile = "GeneratedMap.map";
lTexture foo;
//these values are not used
SDL_Window* gWindow = NULL;
SDL_Renderer* gRenderer = NULL;

//markers for the types of pixels
const int RED_TILE = 0;
const int BLUE_TILE = 1;
const int RED_CHECK = 2;
const int GREEN_TILE = 3;
const int ENDZONE = 4;
const int TOP_CAP = 5;
const int CENTER = 6;
const int TOP_LEFT = 7;
const int TOP = 8;
const int TOP_RIGHT = 9;
const int LEFT = 10;
const int RIGHT = 11;
const int BOTTOM_LEFT = 12;
const int BOTTOM = 13;
const int BOTTOM_RIGHT = 14;
const int ALL_BORDER = 15;
const int VERT_BORDER = 16;
const int HORZ_BORDER = 17;
const int RIGHT_CAP = 18;
const int LEFT_CAP = 19;
const int BOT_CAP = 20;

Uint32 redTileColor;
Uint32 blueTileColor;
Uint32 redCheckColor;
Uint32 greenTileColor;
Uint32 endZoneColor;



//forward declarations
bool init();
bool loadMedia();
void close();
SDL_Color getPixelColor(Uint32* pixel);

/**
    Setup SDL and IMG
 
    @return true if everything was loaded successfully
 */
bool init(){
    bool successFlag = true;
    if(SDL_Init(SDL_INIT_VIDEO) < 0){
        printf("Could not start SDL! SDL error: %s\n", SDL_GetError());
        successFlag = false;
    }else{
        gWindow = SDL_CreateWindow("Manual Color Keying", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 80, 80, SDL_WINDOW_SHOWN);
        if(gWindow == NULL){
            printf("Could not create window! SDL error: %s\n", SDL_GetError());
            successFlag = false;
        }else{
            gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
            if(gRenderer == NULL){
                printf("Could not create renderer! SDL error: %s\n", SDL_GetError());
                successFlag = false;
            }else{
                //setup texture and renderer
                SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);
                foo = lTexture(gRenderer);
            }
        }
    }
    if(!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)){
        printf("Could not start IMG! IMG error: %s\n", IMG_GetError());
        successFlag = false;
    }
    return successFlag;
}

/**
    Free all allocated resources and close libs
 */
void close(){
    foo.free();
    SDL_DestroyRenderer(gRenderer);
    SDL_DestroyWindow(gWindow);
    gRenderer = NULL;
    gWindow = NULL;
    SDL_Quit();
    IMG_Quit();
}


//need to manually color key in the load media function
/**
    Loads the image file that is to be converted to the map, and performs pixel-level logic to generate a .map file
 
    @return true on success
 */
bool loadMedia(){
    bool successFlag = true;
    if(!foo.loadFromFileWithPixels(inputImgFile.c_str())){
        printf("Could not load foo image!");
        successFlag = false;
    }else{
        //here we do the color keying by accessing the individual pixels
        if(!foo.lockTexture()){
            printf("Could not lock texture!");
            successFlag = false;
        }else{
            SDL_PixelFormat* mappingFormat = SDL_AllocFormat(SDL_GetWindowPixelFormat(gWindow));
            //need to get the pixels of the texture and the size
            Uint32* pixels = (Uint32*)foo.getPixels();
            //the pitch is in bits and there is 1 pixel per bit
            int pixelCount = (foo.getPitch()/4) * foo.getHeight();
            //need array to track the tiles
            int tileArray[pixelCount];
            //set the color key
            
            redTileColor = foo.getPixel(0, 0);
            blueTileColor = foo.getPixel(1, 0);
            redCheckColor = foo.getPixel(2, 0);
            greenTileColor = foo.getPixel(3, 0);
            endZoneColor = foo.getPixel(4, 0);
            //here we want to loop through the pixels and convert the image to a map file
            for(int i = 0; i < pixelCount; i++){
                //test the pixel, why not do this with switch
                if(pixels[i] == redTileColor){
                    if(i == 0){
                        tileArray[i] = RED_TILE;
                    }else{
                        tileArray[i] = RED_TILE;
                    }
                }else if(pixels[i] == blueTileColor){
                    if(i == 1){
                        tileArray[i] = RED_TILE;
                    }else{
                        tileArray[i] = BLUE_TILE;
                    }
                }else if(pixels[i] == redCheckColor){
                    if(i == 2){
                        tileArray[i] = TOP_CAP;
                    }else{
                        tileArray[i] = RED_CHECK;
                    }
                }else if(pixels[i] == greenTileColor){
                    if(i == 3){
                        tileArray[i] = TOP_CAP;
                    }else{
                        tileArray[i] = GREEN_TILE;
                    }
                }else if(pixels[i] == endZoneColor){
                    if(i == 4){
                        tileArray[i] = TOP_CAP;
                    }else{
                        tileArray[i] = ENDZONE;
                    }
                }else{
                    //pixel is a wall
                    tileArray[i] = TOP_CAP;
                }
            }
            //now we can loop through the int array and change each wall to the appropriate value
            int row = 1;
            int column = 1;
            for(int i = 0; i < pixelCount; ++i){
                //get the row and column of the pixel
                if(i >= (foo.getWidth() * row)){
                    ++row;
                }
                column = (i % foo.getWidth()) + 1;
                if(tileArray[i] >= TOP_CAP){
                    //the tile is a wall tile, figure out what kind
                    //test each neighbor
                    bool topWall = false;
                    bool botWall = false;
                    bool rightWall = false;
                    bool leftWall = false;
                    //need to make sure we dont step out of the map
                    if((row - 1) < 1){
                        topWall = true;
                    }else if(tileArray[i - (foo.getWidth())] >= TOP_CAP){
                        topWall = true;
                    }
                    if((row + 1) > foo.getWidth()){
                        botWall = true;
                    }else if(tileArray[i + (foo.getWidth())] >= TOP_CAP){
                        botWall = true;
                    }
                    if((column - 1) < 1){
                        leftWall = true;
                    }else if(tileArray[i - 1] >= TOP_CAP){
                        leftWall = true;
                    }
                    if((column + 1) > foo.getHeight()){
                        rightWall = true;
                    }else if(tileArray[i + 1] >= TOP_CAP){
                        rightWall = true;
                    }
                
                    //now need to loop through the 16 cases, oh boi
                    if(!topWall && botWall && !leftWall && !rightWall){
                        tileArray[i] = TOP_CAP;
                    }
                    else if(topWall && botWall && leftWall && rightWall){
                        tileArray[i] = CENTER;
                    }
                    else if(!topWall && botWall && !leftWall && rightWall){
                        tileArray[i] = TOP_LEFT;
                    }
                    else if(!topWall && botWall && leftWall && rightWall){
                        tileArray[i] = TOP;
                    }
                    else if(!topWall && botWall && leftWall && !rightWall){
                        tileArray[i] = TOP_RIGHT;
                    }
                    else if(topWall && botWall && !leftWall && rightWall){
                        tileArray[i] = LEFT;
                    }
                    else if(topWall && botWall && leftWall && !rightWall){
                        tileArray[i] = RIGHT;
                    }
                    else if(topWall && !botWall && !leftWall && rightWall){
                        tileArray[i] = BOTTOM_LEFT;
                    }
                    else if(topWall && !botWall && leftWall && rightWall){
                        tileArray[i] = BOTTOM;
                    }
                    else if(topWall && !botWall && leftWall && !rightWall){
                        tileArray[i] = BOTTOM_RIGHT;
                    }
                    else if(!topWall && !botWall && !leftWall && !rightWall){
                        tileArray[i] = ALL_BORDER;
                    }
                    else if(topWall && botWall && !leftWall && !rightWall){
                        tileArray[i] = VERT_BORDER;
                    }
                    else if(!topWall && !botWall && leftWall && rightWall){
                        tileArray[i] = HORZ_BORDER;
                    }
                    else if(!topWall && !botWall && leftWall && !rightWall){
                        tileArray[i] = RIGHT_CAP;
                    }
                    else if(!topWall && !botWall && !leftWall && rightWall){
                        tileArray[i] = LEFT_CAP;
                    }
                    else if(topWall && !botWall && !leftWall && !rightWall){
                        tileArray[i] = BOT_CAP;
                    }
                }
            }
            //now write out to map file
            fstream mapFile(outputMapFile.c_str(), ifstream::out);
            if(!mapFile.is_open()){
                printf("Could not open output file!\n");
            }else{
                mapFile << to_string(foo.getWidth()) << endl;
                mapFile << to_string(foo.getHeight()) << endl;
                //write out
                int row = 1;
                for( int i = 0; i < pixelCount; ++i){
                    if(i >= (row * foo.getWidth())){
                        ++row;
                        mapFile << endl;
                    }
                    mapFile << to_string(tileArray[i]) + " ";
                }
            }
            mapFile.close();
            
            //then lock the texture again
            if(!foo.unlockTexture()){
                printf("Could not unlock texture!");
                successFlag = false;
            }else{
                SDL_FreeFormat(mappingFormat);
            }
        }
    }
    return successFlag;
}

int main(int argc, const char * argv[]) {
    if(!init()){
        printf("Could not load SDL!\n");
    }else{
        if(!loadMedia()){
            printf("Could not load and convert image!\n");
        }
    }
    close();
    return 0;
}
