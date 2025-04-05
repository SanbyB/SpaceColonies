#pragma once
#include "SpaceObject.h"

// Class for planets, stars, etc.
class CelestialBody : public SpaceObject {
public:
    double radius; // Actual physical radius in meters
    
    CelestialBody(double mass, double radius, Vector2D pos, Vector2D vel, int renderSize);
    
    void update(const std::vector<std::shared_ptr<CelestialBody>>& bodies, double dt, bool RK4=true) override {
        // Celestial bodies typically don't move in this simplified simulation
        // but you could implement orbital motion for moons, etc.
    }
    
    // Calculate gravitational acceleration for other objects
    Vector2D calculateGravitationalAcceleration(const Vector2D& objectPosition) const;
    
    void renderOrbit(SDL_Renderer* renderer, Vector2D cameraOffset);
};