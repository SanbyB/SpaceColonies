#include <SDL2/SDL_image.h>
#include <iostream>
#include "../include/SpaceObject.h"
#include "../include/Utils.h"
#include "../include/Constants.h"

SpaceObject::SpaceObject(double mass, Vector2D pos, Vector2D vel, int size): 
    mass(mass), position(pos), velocity(vel), size(size), texture(nullptr){};

SpaceObject::~SpaceObject(){
    if(texture){
        SDL_DestroyTexture(texture);
    }
};


void SpaceObject::render(SDL_Renderer* renderer, Vector2D cameraOffset, double scale) {
    if (!texture) return;
    
    SDL_Rect destRect;
    destRect.x = static_cast<int>((position.x * scale) + (SCREEN_WIDTH / 2) - (size / 2) + cameraOffset.x);
    destRect.y = static_cast<int>((position.y * scale) + (SCREEN_HEIGHT / 2) - (size / 2) + cameraOffset.y);
    destRect.w = size;
    destRect.h = size;
    
    SDL_RenderCopy(renderer, texture, NULL, &destRect);
};

void SpaceObject::loadTexture(SDL_Renderer* renderer, const char* path) {
    SDL_Surface* surface = IMG_Load(path);
    if (surface) {
        texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
    } else {
        std::cerr << "Failed to load image: " << path << std::endl;
    }
};
