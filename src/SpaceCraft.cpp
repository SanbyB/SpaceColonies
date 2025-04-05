#include "../include/SpaceCraft.h"
#include "../include/CelestialBody.h"
#include "../include/Constants.h"

Spacecraft::Spacecraft(double mass, Vector2D pos, Vector2D vel, double fuel, double enginePower, int size) 
    : SpaceObject(mass, pos, vel, size), fuel(fuel), enginePower(enginePower), thrustActive(false) {
    thrustDirection = Vector2D(0, -1); // Default pointing upward
    orbitTrail = {};
};

void Spacecraft::update(const std::vector<std::shared_ptr<CelestialBody>>& bodies, double dt, bool RK4){
    // Apply gravitational forces from all celestial bodies

    if(RK4){
        // RK4 integration for spacecraft
        Vector2D k1_v = calculateAcceleration(bodies, position);
        Vector2D k1_p = velocity;
        
        Vector2D k2_v = calculateAcceleration(bodies, position + k1_p * (dt/2));
        Vector2D k2_p = velocity + k1_v * (dt/2);
        
        Vector2D k3_v = calculateAcceleration(bodies, position + k2_p * (dt/2));
        Vector2D k3_p = velocity + k2_v * (dt/2);
        
        Vector2D k4_v = calculateAcceleration(bodies, position + k3_p * dt);
        Vector2D k4_p = velocity + k3_v * dt;
        
        // Update position and velocity
        position = position + (k1_p + k2_p*2 + k3_p*2 + k4_p) * (dt/6);
        velocity = velocity + (k1_v + k2_v*2 + k3_v*2 + k4_v) * (dt/6);
    }
    else{
        Vector2D acceleration = calculateAcceleration(bodies, position);
        // Update velocity and position using simple Euler integration
        velocity = velocity + (acceleration * dt);
        position = position + (velocity * dt);
    }    
    
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
            // not sure on the maths of this 
            velocity = velocity - (normal * (2 * (velocity.x * normal.x + velocity.y * normal.y)));
            // Move outside the planet
            position = body->position + (normal * body->radius * 1.1);
        }
    }
};

Vector2D Spacecraft::calculateAcceleration(const std::vector<std::shared_ptr<CelestialBody>>& bodies, const Vector2D& pos) {
    Vector2D acceleration(0, 0);
    
    // Apply gravitational forces
    for (const auto& body : bodies) {
        Vector2D direction = body->position - pos;
        double distance = direction.magnitude();
        
        if (distance < body->radius) continue; // Inside body
        
        double forceMagnitude = GRAVITATIONAL_CONSTANT * body->mass / (distance * distance);
        acceleration = acceleration + direction.normalized() * forceMagnitude;
    }
    
    // Apply thrust
    if (thrustActive && fuel > 0) {
        double thrustAcceleration = enginePower / mass;
        acceleration = acceleration + (thrustDirection * thrustAcceleration);
        fuel -= enginePower * 0.01; // *dt Consume fuel
        if (fuel < 0) fuel = 0;
    }
    
    return acceleration;
}

void Spacecraft::applyThrust(bool active) {
    thrustActive = active && fuel > 0;
};

void Spacecraft::setThrustDirection(const Vector2D& direction) {
    thrustDirection = direction.normalized();
};

void Spacecraft::renderTrail(SDL_Renderer* renderer, Vector2D cameraOffset, double scale) {
    if (orbitTrail.size() < 2) return;
    
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 128);
    for (size_t i = 1; i < orbitTrail.size(); i++) {
        SDL_RenderDrawLine(
            renderer,
            static_cast<int>((orbitTrail[i-1].x * scale) + (SCREEN_WIDTH / 2) + cameraOffset.x),
            static_cast<int>((orbitTrail[i-1].y * scale) + (SCREEN_HEIGHT / 2) + cameraOffset.y),
            static_cast<int>((orbitTrail[i].x * scale) + (SCREEN_WIDTH / 2) + cameraOffset.x),
            static_cast<int>((orbitTrail[i].y * scale) + (SCREEN_HEIGHT / 2) + cameraOffset.y)
        );
    }
};

void Spacecraft::render(SDL_Renderer* renderer, Vector2D cameraOffset, double scale){
    // Render the trail first so spacecraft appears on top
    renderTrail(renderer, cameraOffset, scale);
    
    // Then render the spacecraft itself
    SpaceObject::render(renderer, cameraOffset, scale);
    
    // Render thrust if active
    if (thrustActive && fuel > 0) {
        SDL_SetRenderDrawColor(renderer, 255, 165, 0, 255); // Orange for thrust
        int shipX = static_cast<int>((position.x * scale) + (SCREEN_WIDTH / 2) + cameraOffset.x);
        int shipY = static_cast<int>((position.y * scale) + (SCREEN_HEIGHT / 2) + cameraOffset.y);
        int thrustEndX = shipX - static_cast<int>(thrustDirection.x * size);
        int thrustEndY = shipY - static_cast<int>(thrustDirection.y * size);
        SDL_RenderDrawLine(renderer, shipX, shipY, thrustEndX, thrustEndY);
    }
};