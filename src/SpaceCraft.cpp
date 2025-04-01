#include "../include/SpaceCraft.h"
#include "../include/CelestialBody.h"
#include "../include/Constants.h"

Spacecraft::Spacecraft(double mass, Vector2D pos, Vector2D vel, double fuel, double enginePower, int size) 
    : SpaceObject(mass, pos, vel, size), fuel(fuel), enginePower(enginePower), thrustActive(false) {
    thrustDirection = Vector2D(0, -1); // Default pointing upward
};

void Spacecraft::update(const std::vector<std::shared_ptr<CelestialBody>>& bodies, double dt){
    // Apply gravitational forces from all celestial bodies
    Vector2D acceleration(0, 0);
    
    for(const auto& body : bodies){
        acceleration = acceleration + body->calculateGravitationalAcceleration(position);
    }
    
    // Apply thrust if active and fuel available
    if(thrustActive && fuel > 0){
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
    if(orbitTrail.size() > 1000){
        orbitTrail.erase(orbitTrail.begin());
    }
    
    // Check for collisions with celestial bodies
    for(const auto& body : bodies){
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
};

void Spacecraft::applyThrust(bool active) {
    thrustActive = active && fuel > 0;
};

void Spacecraft::setThrustDirection(const Vector2D& direction) {
    thrustDirection = direction.normalized();
};

void Spacecraft::renderTrail(SDL_Renderer* renderer, Vector2D cameraOffset) {
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
};

void Spacecraft::render(SDL_Renderer* renderer, Vector2D cameraOffset){
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
};