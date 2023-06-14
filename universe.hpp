
#ifndef UNIVERSE_HPP
#define UNIVERSE_HPP

#include <simulation.hpp>


class Universe : public World {

public:
    Universe();
    void tick(unsigned long long time);

};

#endif // UNIVERSE_HPP
