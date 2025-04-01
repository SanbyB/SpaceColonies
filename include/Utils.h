#pragma once
#include <cmath>

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