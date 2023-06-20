
#include "universe.hpp"
#include <random>
#include <cmath>
#include <iostream>

Universe::Universe() : World() {
    Sphere* s = new Sphere();

    //addChild(s);
    this->createSystem();
    lastTime = -1;
}

void Universe::tick(double time) {
    if(lastTime == -1) {
        lastTime = time;
        return;
    }
    World::tick(time);
    this->applyGravity(time);
    this->applyVelocity(time);

    lastTime = time;
}

// creates solarsystem based on a scale factor
/*
Planet	Radius (km)	Mass (kg)	Distance to Sun (million km)	Velocity (km/s)	Color
(Sun	695,700	1.989 × 10^30	-	-	(1.0, 1.0, 0.0))
Mercury	2,439	3.30 × 10^23	57.9	47.87	Gray (0.5, 0.5, 0.5)
Venus	6,052	4.87 × 10^24	108.2	35.02	Yellow (0.92, 0.87, 0.4)
Earth	6,371	5.97 × 10^24	149.6	29.78	Blue (0.0, 0.0, 1.0)
Mars	3,389	6.39 × 10^23	227.9	24.07	Red (1.0, 0.0, 0.0)
Jupiter	69,911	1.89 × 10^27	778.5	13.07	Orange (1.0, 0.5, 0.0)
Saturn	58,232	5.69 × 10^26	1,429.0	9.69	Yellow (1.0, 1.0, 0.6)
Uranus	25,362	8.68 × 10^25	2,870.6	6.81	Blue (0.42, 0.75, 0.89)
Neptune	24,622	1.02 × 10^26	4,498.4	5.43	Blue (0.0, 0.0, 0.5)
Pluto	1,188	1.31 × 10^22	5,906.4	4.67	Brown (0.66, 0.36, 0.25)
*/
void Universe::createSystem() {

    std::vector<PlanetData> system;
    system.push_back(PlanetData(695700, 1.989e30, 0, 0, Vec3(1,1,0)));
    system.push_back(PlanetData(2439, 3.3e23, 57.9e6, 47.87, Vec3(0.5,0.5,0.5)));
    system.push_back(PlanetData(6052, 4.87e24, 108.2e6, 35.02, Vec3(0.92,0.87,0.4)));
    system.push_back(PlanetData(6371, 5.97e24, 149.6e6, 29.78, Vec3(0,0,1)));
    system.push_back(PlanetData(3389, 6.39e23, 227.9e6, 24.07, Vec3(1,0,0)));
    system.push_back(PlanetData(69911, 1.89e27, 778.5e6, 13.07, Vec3(1,0.5,0)));
    system.push_back(PlanetData(58232, 5.69e26, 1429e6, 9.69, Vec3(1,1,0.6)));
    system.push_back(PlanetData(25362, 8.68e25, 2870.6e6, 6.81, Vec3(0.42,0.75,0.89)));
    system.push_back(PlanetData(24622, 1.02e26, 4498.4e6, 5.43, Vec3(0,0,0.5)));
    system.push_back(PlanetData(1188, 1.31e22, 5906.4e6, 4.67, Vec3(0.66,0.36,0.25)));
    for(PlanetData p : system) {
        Sphere* s = p.createSphere(scale);
        this->addSphere(s);
    }


}

// calculates gravity between planets and changes velocity
void Universe::applyGravity(double time) {
    // time in seconds (default time scale is 1 day per second)
    double gravityscale = 7.5e-34;
    constexpr double G = 6.67408e-11;

    double timeDiff = time - lastTime;


    std::vector<Sphere*> usedSpheres;
    for(Sphere* s : this->spheres) {

        // calculate gravity
        for(Sphere* s2 : this->spheres){
            if(s == s2) continue;
            if(std::find(usedSpheres.begin(), usedSpheres.end(), s2) != usedSpheres.end()) continue;

            //distance of center of mass
            auto r = s->getPosition() - s2->getPosition();

            // distance is in millions of km
            //r = r * 1000000;

            auto r3 = Vec3(r.x*r.x*r.x, r.y*r.y*r.y, r.z*r.z*r.z);

            double distance = r3.length();

            // calculate force
            Vec3 F = gravityscale * G * s->getMass() * s2->getMass() / distance * r;

            //std::cout << F.x << " " << F.y << " " << F.z << std::endl;

            auto acc1 = F / s->getMass();
            auto acc2 = F / s2->getMass();
            //std::cout << acc1.x << " " << acc1.y << " " << acc1.z << std::endl;

            s->setVelocity(s->getVelocity() - acc1 * timeDiff);
            //std::cout << "VEL:" << s->getVelocity().x << " " << s->getVelocity().y << " " << s->getVelocity().z << std::endl;
            s2->setVelocity(s2->getVelocity() + acc2 * timeDiff);


        }

        usedSpheres.push_back(s);

    }

}

// moves planets based on velocity
void Universe::applyVelocity(double time) {

    for(Sphere* s : this->spheres) {
        s->move(s->getVelocity() * (time - lastTime));

        //std::cout << s->getPosition().x << " " << s->getPosition().y << " " << s->getPosition().z << std::endl;
    }


}

Sphere* PlanetData::createSphere(double scale) {
    constexpr double biggestRadius = 695700;
    constexpr double smallestRadius = 1188;
    
    constexpr double maxRadius = 0.5;
    constexpr double minRadius = 0.04;

    double radius = minRadius + (maxRadius - minRadius) * log10(this->radius / smallestRadius) / log10(biggestRadius / smallestRadius);

    if(radius < minRadius) radius = minRadius;
    if(radius > maxRadius) radius = maxRadius;

    double mass = this->mass;
    double distance = this->distance * scale;
    double velocity = this->velocity * scale;

    double orbitPosition = (double)rand() / RAND_MAX * 2 * PI;
    Vec3 position = Vec3(distance * cos(orbitPosition), 0, distance * sin(orbitPosition));

    Sphere* s = new Sphere(position, radius, 25);
    s->setMass(mass);

    Vec3 velDir = position.cross(Vec3(0,1,0));
    velDir = velDir.normalized();
    velDir *= velocity;
    s->setVelocity(velDir);
    if(velocity == 0) s->setVelocity(Vec3(0));
    s->setColor(color);

    return s;

}


