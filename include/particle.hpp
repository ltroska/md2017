#ifndef _PARTICLE_HPP
#define _PARTICLE_HPP

#include "defines.hpp"

#include <iostream>
#include <cstdlib>
#include <cmath>

#include "mpi.h"


/**
 * @brief Particle data
 *
 * this class contains the particle data
 */
class Particle {
public:
    std::size_t id;
    /// Mass
    real m;
    /// Position
    vector_t x;
    /// Velocity
    vector_t v;
    /// Force
    vector_t F;
    /// Force (previous step)
    vector_t F_old;
};

MPI_Datatype setup_particle_mpi_type();

std::ostream& operator<<(std::ostream& os, Particle& p);

#endif // _PARTICLE_HPP
// vim:set et sts=4 ts=4 sw=4 ai ci cin:
