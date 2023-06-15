
#include "universe.hpp"

Universe::Universe() : World() {
    Sphere* s = new Sphere();

    addChild(s);
}

void Universe::tick(double time) {
    World::tick(time);
}

