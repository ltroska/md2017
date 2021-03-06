#ifndef _POTENTIAL_HPP
#define _POTENTIAL_HPP

#include <gmpxx.h>
#include "particle.hpp"

/**
 * @brief abstract Potential class
 */
class Potential {
public:
    /**
     * @brief calculate the force bewteen the two particles and add it to p
     *
     * @param p particle p
     * @param q particl q
     *
     * @return potential energy
     */
    virtual real force(Particle &p, Particle &q, real distance_sq, vector_t difference) = 0;
};

#endif // _POTENTIAL_HPP
// vim:set et sts=4 ts=4 sw=4 ai ci cin:
