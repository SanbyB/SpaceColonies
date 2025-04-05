#include <iostream>
#include <SDL2/SDL_image.h>

#include "../include/Constants.h"
#include "../include/Game.h"


Game::Game() : window(nullptr), renderer(nullptr), running(false), followPlayerShip(true) {
    scaleFac = SCALE_FACTOR;
}

Game::~Game() {
    cleanup();
}

bool Game::init() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }
    
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        std::cerr << "SDL_image could not initialize! IMG_Error: " << IMG_GetError() << std::endl;
        return false;
    }
    
    window = SDL_CreateWindow("2D Space Colony Game", 
                                SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
                                SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }
    
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }
    
    // Initialize game objects
    createGameObjects();
    
    running = true;
    return true;
}

void Game::createGameObjects() {
    // Create a star at the center
    auto star = std::make_shared<CelestialBody>(1.989e30, 696340000, Vector2D(0, 0), Vector2D(0, 0), 60);
    star->loadTexture(renderer, "assets/star.png");
    celestialBodies.push_back(star);
    
    // Create a planet in orbit
    auto planet = std::make_shared<CelestialBody>(5.97e29, 6371000, Vector2D(1.5e13, 0), Vector2D(0, 29800), 30);
    planet->loadTexture(renderer, "assets/planet.png");
    celestialBodies.push_back(planet);
    
    // Create player spacecraft
    playerShip = std::make_shared<Spacecraft>(1000, Vector2D(1e13, 0), Vector2D(0, 1600), 1000, 50000, 20);
    playerShip->loadTexture(renderer, "assets/spacecraft.png");
}

void Game::handleEvents() {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            running = false;
        } else if (e.type == SDL_KEYDOWN) {
            switch (e.key.keysym.sym) {
                case SDLK_ESCAPE:
                    running = false;
                    break;
                case SDLK_w:
                    playerShip->setThrustDirection(Vector2D(0, -1));
                    playerShip->applyThrust(true);
                    break;
                case SDLK_s:
                    playerShip->setThrustDirection(Vector2D(0, 1));
                    playerShip->applyThrust(true);
                    break;
                case SDLK_a:
                    playerShip->setThrustDirection(Vector2D(-1, 0));
                    playerShip->applyThrust(true);
                    break;
                case SDLK_d:
                    playerShip->setThrustDirection(Vector2D(1, 0));
                    playerShip->applyThrust(true);
                    break;
                case SDLK_f:
                    followPlayerShip = !followPlayerShip;
                    break;
                case SDLK_SPACE:
                    // Toggle between normal speed and fast forward
                    timeWarpFactor = (timeWarpFactor > 1000) ? 1000 : 5000;
                    break;
                case SDLK_PERIOD:  // '>'
                    // Increase time warp (up to MAX_WARP)
                    timeWarpFactor = std::min(timeWarpFactor * 2.0, MAX_WARP);
                    break;
                case SDLK_COMMA:   // '<'
                    // Decrease time warp (down to MIN_WARP)
                    timeWarpFactor = std::max(timeWarpFactor / 2.0, MIN_WARP);
                    break;
            }
        } else if (e.type == SDL_KEYUP) {
            switch (e.key.keysym.sym) {
                case SDLK_w:
                case SDLK_s:
                case SDLK_a:
                case SDLK_d:
                    playerShip->applyThrust(false);
                    break;
            }
        }  else if (e.type == SDL_MOUSEWHEEL) {
            // Handle mouse wheel for zooming
            if (e.wheel.y > 0) {
                // Zoom in
                zoomAt(ZOOM_SPEED, mousePosition);
            } else if (e.wheel.y < 0) {
                // Zoom out
                zoomAt(1.0 / ZOOM_SPEED, mousePosition);
            }
        } else if (e.type == SDL_MOUSEMOTION) {
            // Track mouse position for zoom targeting
            mousePosition.x = e.motion.x;
            mousePosition.y = e.motion.y;
        }
    }
}

void Game::zoomAt(double factor, Vector2D targetPos) {
    // Store pre-zoom camera-space coordinates of zoom target
    double worldX = (targetPos.x - SCREEN_WIDTH/2 - cameraOffset.x) / scaleFac;
    double worldY = (targetPos.y - SCREEN_HEIGHT/2 - cameraOffset.y) / scaleFac;
    
    // Apply zoom factor, clamping to min/max zoom levels
    double newScale = scaleFac * factor;
    if (newScale < MIN_SCALE_FACTOR) newScale = MIN_SCALE_FACTOR;
    if (newScale > MAX_SCALE_FACTOR) newScale = MAX_SCALE_FACTOR;
    
    // If we're following the player, don't adjust camera offset
    if (!followPlayerShip) {
        // Calculate new screen position of the target point after zoom
        double newScreenX = worldX * newScale + SCREEN_WIDTH/2 + cameraOffset.x;
        double newScreenY = worldY * newScale + SCREEN_HEIGHT/2 + cameraOffset.y;
        
        // Adjust camera offset to keep target point under mouse
        cameraOffset.x += targetPos.x - newScreenX;
        cameraOffset.y += targetPos.y - newScreenY;
    }
    
    scaleFac = newScale;
}

void Game::update() {
    std::cout << timeWarpFactor << "\n";
    // For normal-to-moderate time warps (up to ~100x)
    if (timeWarpFactor <= 10000000) {
        // Use the multiple fixed steps approach
        int numSteps = std::min(static_cast<int>(timeWarpFactor), 
                                static_cast<int>(MAX_PHYSICS_STEPS_PER_FRAME));
        double remainder = timeWarpFactor - numSteps;
        
        for (int i = 0; i < numSteps; i++) {
            updatePhysics(TIME_STEP);
        }
        
        if (remainder > 0.001) {
            updatePhysics(TIME_STEP * remainder);
        }
    }
    // For extreme time warps (>100x)
    else {
        // Calculate an adaptive time step
        double adaptiveTimeStep = std::min(TIME_STEP * 100, MAX_TIME_STEP);
        int steps = std::min(static_cast<int>(timeWarpFactor / 100), 
                            static_cast<int>(MAX_PHYSICS_STEPS_PER_FRAME));
        
        // Apply fewer, larger steps
        for (int i = 0; i < steps; i++) {
            updatePhysics(adaptiveTimeStep);
        }
    }
    
    // Camera update
    if (followPlayerShip) {
        cameraOffset.x = -playerShip->position.x * SCALE_FACTOR;
        cameraOffset.y = -playerShip->position.y * SCALE_FACTOR;
    }
}

// New method that performs a single physics update step
void Game::updatePhysics(double dt) {
    playerShip->update(celestialBodies, dt);
    
    for (auto& body : celestialBodies) {
        body->update(celestialBodies, dt);
    }
}

// void Game::updatePhysicsRK4(double dt) {
//     // For each object that needs updating
//     playerShip->updateRK4(celestialBodies, dt);
    
//     // Update celestial bodies if needed
//     for (auto& body : celestialBodies) {
//         // add updateRK4 if needed
//         body->update(celestialBodies, dt);
//     }
// }

void Game::renderUI() {
    // Add a time warp indicator
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    
    // Draw time warp text (you'll need to implement text rendering)
    // For now, we can use a visual indicator
    
    int indicatorX = 50;
    int indicatorY = SCREEN_HEIGHT - 50;
    int indicatorWidth = static_cast<int>(3 * timeWarpFactor / 1000);
    int indicatorHeight = 10;
    
    SDL_Rect timeWarpIndicator = {indicatorX, indicatorY, indicatorWidth, indicatorHeight};
    SDL_RenderFillRect(renderer, &timeWarpIndicator);
}

void Game::render() {
    // Clear screen
    SDL_SetRenderDrawColor(renderer, 0, 0, 20, 255);
    SDL_RenderClear(renderer);
    
    // Render celestial bodies
    for (auto& body : celestialBodies) {
        body->renderOrbit(renderer, cameraOffset);
        body->render(renderer, cameraOffset, scaleFac);
    }
    
    // Render player spacecraft
    playerShip->render(renderer, cameraOffset, scaleFac);
    
    // Render UI elements
    renderUI();
    
    // Present renderer
    SDL_RenderPresent(renderer);
}


void Game::run() {
    const int FPS = 600;
    const int frameDelay = 1000 / FPS;
    
    Uint32 frameStart;
    int frameTime;
    
    while (running) {
        frameStart = SDL_GetTicks();
        
        handleEvents();
        update();
        render();
        
        frameTime = SDL_GetTicks() - frameStart;
        
        if (frameDelay > frameTime) {
            SDL_Delay(frameDelay - frameTime);
        }
    }
}

void Game::cleanup() {
    celestialBodies.clear();
    playerShip.reset();
    
    if (renderer) {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }
    
    if (window) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }
    
    IMG_Quit();
    SDL_Quit();
}