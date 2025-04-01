#include "../include/CelestialBody.h"
#include "../include/Constants.h"
    
CelestialBody::CelestialBody(double mass, double radius, Vector2D pos, Vector2D vel, int renderSize) 
    : SpaceObject(mass, pos, vel, renderSize), radius(radius) {}


// Calculate gravitational acceleration for other objects
Vector2D CelestialBody::calculateGravitationalAcceleration(const Vector2D& objectPosition) const {
    Vector2D direction = position - objectPosition;
    double distance = direction.magnitude();
    
    // Avoid division by zero and apply inverse square law
    if (distance < radius) {
        return Vector2D(0, 0); // Inside the body, no gravity for simplicity
    }
    
    double forceMagnitude = GRAVITATIONAL_CONSTANT * mass / (distance * distance);
    return direction.normalized() * forceMagnitude;
}

void CelestialBody::renderOrbit(SDL_Renderer* renderer, Vector2D cameraOffset) {
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