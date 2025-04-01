#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <cmath>
#include <vector>
#include <memory>
#include <iostream>

// Constants
const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;
const double GRAVITATIONAL_CONSTANT = 6.67430e-11;
const double TIME_STEP = 0.1; // Simulation time step in seconds
const double SCALE_FACTOR = 1e-6; // Scale for rendering orbital distances

// Forward declarations
class CelestialBody;
class SpaceObject;
class Spacecraft;

// Vector2D class for physics calculations
class Vector2D {
public:
    double x, y;
    
    Vector2D() : x(0), y(0) {}
    Vector2D(double x, double y) : x(x), y(y) {}
    
    Vector2D operator+(const Vector2D& v) const { return Vector2D(x + v.x, y + v.y); }
    Vector2D operator-(const Vector2D& v) const { return Vector2D(x - v.x, y - v.y); }
    Vector2D operator*(double scalar) const { return Vector2D(x * scalar, y * scalar); }
    
    double magnitude() const { return std::sqrt(x*x + y*y); }
    
    Vector2D normalized() const {
        double mag = magnitude();
        if (mag > 0) {
            return Vector2D(x / mag, y / mag);
        }
        return Vector2D(0, 0);
    }
};

// Base class for objects in space
class SpaceObject {
public:
    Vector2D position;
    Vector2D velocity;
    double mass;
    SDL_Texture* texture;
    int size;
    
    SpaceObject(double mass, Vector2D pos, Vector2D vel, int size) 
        : mass(mass), position(pos), velocity(vel), size(size), texture(nullptr) {}
    
    virtual ~SpaceObject() {
        if (texture) {
            SDL_DestroyTexture(texture);
        }
    }
    
    virtual void update(const std::vector<std::shared_ptr<CelestialBody>>& bodies, double dt) = 0;
    
    virtual void render(SDL_Renderer* renderer, Vector2D cameraOffset) {
        if (!texture) return;
        
        SDL_Rect destRect;
        destRect.x = static_cast<int>((position.x * SCALE_FACTOR) + (SCREEN_WIDTH / 2) - (size / 2) + cameraOffset.x);
        destRect.y = static_cast<int>((position.y * SCALE_FACTOR) + (SCREEN_HEIGHT / 2) - (size / 2) + cameraOffset.y);
        destRect.w = size;
        destRect.h = size;
        
        SDL_RenderCopy(renderer, texture, NULL, &destRect);
    }
    
    void loadTexture(SDL_Renderer* renderer, const char* path) {
        SDL_Surface* surface = IMG_Load(path);
        if (surface) {
            texture = SDL_CreateTextureFromSurface(renderer, surface);
            SDL_FreeSurface(surface);
        } else {
            std::cerr << "Failed to load image: " << path << std::endl;
        }
    }
};

// Class for planets, stars, etc.
class CelestialBody : public SpaceObject {
public:
    double radius; // Actual physical radius in meters
    
    CelestialBody(double mass, double radius, Vector2D pos, Vector2D vel, int renderSize) 
        : SpaceObject(mass, pos, vel, renderSize), radius(radius) {}
    
    void update(const std::vector<std::shared_ptr<CelestialBody>>& bodies, double dt) override {
        // Celestial bodies typically don't move in this simplified simulation
        // but you could implement orbital motion for moons, etc.
    }
    
    // Calculate gravitational acceleration for other objects
    Vector2D calculateGravitationalAcceleration(const Vector2D& objectPosition) const {
        Vector2D direction = position - objectPosition;
        double distance = direction.magnitude();
        
        // Avoid division by zero and apply inverse square law
        if (distance < radius) {
            return Vector2D(0, 0); // Inside the body, no gravity for simplicity
        }
        
        double forceMagnitude = GRAVITATIONAL_CONSTANT * mass / (distance * distance);
        return direction.normalized() * forceMagnitude;
    }
    
    void renderOrbit(SDL_Renderer* renderer, Vector2D cameraOffset) {
        // For a stationary body like a star or planet in this demo, we don't render an orbit
        // but we could render influence radius or similar
        int centerX = static_cast<int>((position.x * SCALE_FACTOR) + (SCREEN_WIDTH / 2) + cameraOffset.x);
        int centerY = static_cast<int>((position.y * SCALE_FACTOR) + (SCREEN_HEIGHT / 2) + cameraOffset.y);
        
        // Draw a circle to represent the gravitational influence
        int radius = static_cast<int>(this->radius * SCALE_FACTOR / 10);
        SDL_SetRenderDrawColor(renderer, 100, 100, 100, 50);
        
        // Simple circle drawing algorithm
        for (int w = 0; w < radius * 2; w++) {
            for (int h = 0; h < radius * 2; h++) {
                int dx = radius - w;
                int dy = radius - h;
                if ((dx*dx + dy*dy) <= (radius * radius)) {
                    SDL_RenderDrawPoint(renderer, centerX + dx, centerY + dy);
                }
            }
        }
    }
};

// Class for player spacecraft
class Spacecraft : public SpaceObject {
public:
    double fuel;
    double enginePower;
    bool thrustActive;
    Vector2D thrustDirection;
    std::vector<Vector2D> orbitTrail;
    
    Spacecraft(double mass, Vector2D pos, Vector2D vel, double fuel, double enginePower, int size) 
        : SpaceObject(mass, pos, vel, size), fuel(fuel), enginePower(enginePower), thrustActive(false) {
        thrustDirection = Vector2D(0, -1); // Default pointing upward
    }
    
    void update(const std::vector<std::shared_ptr<CelestialBody>>& bodies, double dt) override {
        // Apply gravitational forces from all celestial bodies
        Vector2D acceleration(0, 0);
        
        for (const auto& body : bodies) {
            acceleration = acceleration + body->calculateGravitationalAcceleration(position);
        }
        
        // Apply thrust if active and fuel available
        if (thrustActive && fuel > 0) {
            double thrustAcceleration = enginePower / mass;
            acceleration = acceleration + (thrustDirection * thrustAcceleration);
            fuel -= enginePower * dt * 0.01; // Consume fuel
            if (fuel < 0) fuel = 0;
        }
        
        // Update velocity and position using simple Euler integration
        velocity = velocity + (acceleration * dt);
        position = position + (velocity * dt);
        
        // Store position for orbit trail (limited to 1000 points)
        orbitTrail.push_back(position);
        if (orbitTrail.size() > 1000) {
            orbitTrail.erase(orbitTrail.begin());
        }
        
        // Check for collisions with celestial bodies
        for (const auto& body : bodies) {
            Vector2D distanceVector = position - body->position;
            double distance = distanceVector.magnitude();
            if (distance < body->radius) {
                // Simple bounce for now - in a real game you might destroy the spacecraft
                Vector2D normal = distanceVector.normalized();
                velocity = velocity - (normal * (2 * (velocity.x * normal.x + velocity.y * normal.y)));
                // Move outside the planet
                position = body->position + (normal * body->radius * 1.1);
            }
        }
    }
    
    void applyThrust(bool active) {
        thrustActive = active && fuel > 0;
    }
    
    void setThrustDirection(const Vector2D& direction) {
        thrustDirection = direction.normalized();
    }
    
    void renderTrail(SDL_Renderer* renderer, Vector2D cameraOffset) {
        if (orbitTrail.size() < 2) return;
        
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 128);
        for (size_t i = 1; i < orbitTrail.size(); i++) {
            SDL_RenderDrawLine(
                renderer,
                static_cast<int>((orbitTrail[i-1].x * SCALE_FACTOR) + (SCREEN_WIDTH / 2) + cameraOffset.x),
                static_cast<int>((orbitTrail[i-1].y * SCALE_FACTOR) + (SCREEN_HEIGHT / 2) + cameraOffset.y),
                static_cast<int>((orbitTrail[i].x * SCALE_FACTOR) + (SCREEN_WIDTH / 2) + cameraOffset.x),
                static_cast<int>((orbitTrail[i].y * SCALE_FACTOR) + (SCREEN_HEIGHT / 2) + cameraOffset.y)
            );
        }
    }
    
    void render(SDL_Renderer* renderer, Vector2D cameraOffset) override {
        // Render the trail first so spacecraft appears on top
        renderTrail(renderer, cameraOffset);
        
        // Then render the spacecraft itself
        SpaceObject::render(renderer, cameraOffset);
        
        // Render thrust if active
        if (thrustActive && fuel > 0) {
            SDL_SetRenderDrawColor(renderer, 255, 165, 0, 255); // Orange for thrust
            int shipX = static_cast<int>((position.x * SCALE_FACTOR) + (SCREEN_WIDTH / 2) + cameraOffset.x);
            int shipY = static_cast<int>((position.y * SCALE_FACTOR) + (SCREEN_HEIGHT / 2) + cameraOffset.y);
            int thrustEndX = shipX - static_cast<int>(thrustDirection.x * size);
            int thrustEndY = shipY - static_cast<int>(thrustDirection.y * size);
            SDL_RenderDrawLine(renderer, shipX, shipY, thrustEndX, thrustEndY);
        }
    }
};

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