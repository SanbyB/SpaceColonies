#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <cmath>
#include <vector>
#include <memory>
#include <iostream>

#include "include/SpaceObject.h"
#include "include/SpaceCraft.h"
#include "include/CelestialBody.h"
#include "include/Constants.h"



// Game class to manage the simulation
class Game {
private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    bool running;
    
    std::vector<std::shared_ptr<CelestialBody>> celestialBodies;
    std::shared_ptr<Spacecraft> playerShip;
    
    Vector2D cameraOffset;
    bool followPlayerShip;
    
public:
    Game() : window(nullptr), renderer(nullptr), running(false), followPlayerShip(true) {}
    
    ~Game() {
        cleanup();
    }
    
    bool init() {
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
    
    void createGameObjects() {
        // Create a star at the center
        auto star = std::make_shared<CelestialBody>(1.989e30, 696340000, Vector2D(0, 0), Vector2D(0, 0), 60);
        star->loadTexture(renderer, "assets/star.png");
        celestialBodies.push_back(star);
        
        // Create a planet in orbit
        auto planet = std::make_shared<CelestialBody>(5.97e24, 6371000, Vector2D(1.5e11, 0), Vector2D(0, 29800), 30);
        planet->loadTexture(renderer, "assets/planet.png");
        celestialBodies.push_back(planet);
        
        // Create player spacecraft
        playerShip = std::make_shared<Spacecraft>(1000, Vector2D(1.5e11, -1e10), Vector2D(32000, 0), 1000, 50000, 20);
        playerShip->loadTexture(renderer, "assets/spacecraft.png");
    }
    
    void handleEvents() {
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
                        // Time warp (speed up simulation)
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
            }
        }
    }
    
    void update() {
        // Update physics for all objects
        playerShip->update(celestialBodies, TIME_STEP);
        
        for (auto& body : celestialBodies) {
            body->update(celestialBodies, TIME_STEP);
        }
        
        // Update camera if following player
        if (followPlayerShip) {
            cameraOffset.x = -playerShip->position.x * SCALE_FACTOR;
            cameraOffset.y = -playerShip->position.y * SCALE_FACTOR;
        }
    }
    
    void render() {
        // Clear screen
        SDL_SetRenderDrawColor(renderer, 0, 0, 20, 255);
        SDL_RenderClear(renderer);
        
        // Render celestial bodies
        for (auto& body : celestialBodies) {
            body->renderOrbit(renderer, cameraOffset);
            body->render(renderer, cameraOffset);
        }
        
        // Render player spacecraft
        playerShip->render(renderer, cameraOffset);
        
        // Render UI elements
        renderUI();
        
        // Present renderer
        SDL_RenderPresent(renderer);
    }
    
    void renderUI() {
        // A basic UI could show fuel, velocity, etc.
        // This is a placeholder for future implementation
    }
    
    void run() {
        const int FPS = 60;
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
    
    void cleanup() {
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
};

int main(int argc, char* args[]) {
    Game game;
    
    if (!game.init()) {
        std::cerr << "Failed to initialize game" << std::endl;
        return -1;
    }
    
    game.run();
    
    return 0;
}