//
// Created by troska on 11/17/17.
//

#ifndef MD2017_CELL_HPP
#define MD2017_CELL_HPP

#include <vector>
#include "particle.hpp"

struct Cell {
    index_t index;
    std::vector<std::size_t> neighbors;
    std::vector<Particle> particles;
};

#endif //MD2017_CELL_HPP
