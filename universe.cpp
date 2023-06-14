
#include "universe.hpp"

Universe::Universe() : World() {
    Sphere* s = new Sphere();

    addChild(s);
}

void Universe::tick(unsigned long long time) {
    World::tick(time);
}

