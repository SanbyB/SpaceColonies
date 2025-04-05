#pragma once
#include <SDL2/SDL.h>
#include <vector>
#include <memory>
#include "Utils.h"

// forward declaration
class CelestialBody;

// Base class for objects in space
class SpaceObject {
public:
    Vector2D position;
    Vector2D velocity;
    double mass;
    SDL_Texture* texture;
    int size;
    
    SpaceObject(double mass, Vector2D pos, Vector2D vel, int size);
    
    virtual ~SpaceObject();
    
    virtual void update(const std::vector<std::shared_ptr<CelestialBody>>& bodies, double dt, bool RK4=true) = 0;
    
    virtual void render(SDL_Renderer* renderer, Vector2D cameraOffset, double scale);
    
    void loadTexture(SDL_Renderer* renderer, const char* path);
};