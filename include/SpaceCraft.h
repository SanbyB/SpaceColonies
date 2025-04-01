#pragma once
#include "SpaceObject.h"

// Class for player spacecraft
class Spacecraft : public SpaceObject {
public:
    double fuel;
    double enginePower;
    bool thrustActive;
    Vector2D thrustDirection;
    std::vector<Vector2D> orbitTrail;
    
    Spacecraft(double mass, Vector2D pos, Vector2D vel, double fuel, double enginePower, int size);
    
    void update(const std::vector<std::shared_ptr<CelestialBody>>& bodies, double dt) override;
    
    void applyThrust(bool active);
    
    void setThrustDirection(const Vector2D& direction);
    
    void renderTrail(SDL_Renderer* renderer, Vector2D cameraOffset);
    
    void render(SDL_Renderer* renderer, Vector2D cameraOffset) override;
};