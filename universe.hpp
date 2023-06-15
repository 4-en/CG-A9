
#ifndef UNIVERSE_HPP
#define UNIVERSE_HPP

#include <simulation.hpp>


class Universe : public World {

public:
    Universe();
    void tick(double time);

};

#endif // UNIVERSE_HPP
