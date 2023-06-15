
#ifndef UNIVERSE_HPP
#define UNIVERSE_HPP

#include <simulation.hpp>


class PlanetData {
private:
    double radius;
    double mass;
    double distance;
    double velocity;
    Vec3 color;

public:
    PlanetData(){}
    PlanetData(double radius, double mass, double distance, double velocity, Vec3 color) : radius(radius), mass(mass), distance(distance), velocity(velocity), color(color) {}

    Sphere* createSphere(double scale);
};

class Universe : public World {

private:
    double scale = 1.0 / 1.0e8;
    double lastTime;
    void createSystem();
    void applyGravity(double time);
    void applyVelocity(double time);

public:
    Universe();
    void tick(double time);

};

#endif // UNIVERSE_HPP
