#pragma once
#include <SDL2/SDL.h>
#include <vector>
#include <memory>

#include "SpaceCraft.h"
#include "CelestialBody.h"
#include "Utils.h"

// Game class to manage the simulation
class Game {
private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    bool running;
    
    std::vector<std::shared_ptr<CelestialBody>> celestialBodies;
    std::shared_ptr<Spacecraft> playerShip;
    
    Vector2D cameraOffset;
    Vector2D mousePosition;
    bool followPlayerShip;

    double scaleFac;

    double timeWarpFactor = 1000;  // Normal speed by default
    const double MIN_WARP = 1;  //  slow motion
    const double MAX_WARP = 100000000; // fast forward

    const double MAX_PHYSICS_STEPS_PER_FRAME = 100; // Cap for performance
    const double MAX_TIME_STEP = 3600.0; // Max step size in seconds (1 hour)
    
public:
    Game();
    
    ~Game();
    
    bool init();
    
    void createGameObjects();
    
    void handleEvents();

    void zoomAt(double factor, Vector2D targetPos);
    
    void update();

    void updatePhysics(double dt);

    // void updatePhysicsRK4(double dt);
    
    void render();
    
    void renderUI();
    
    void run();
    
    void cleanup();
};