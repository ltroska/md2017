#ifndef _GRAVITYPOTENTIAL_HPP
#define _GRAVITYPOTENTIAL_HPP

#include "potential.hpp"

/**
 * @brief TODO add the documentation
 */
class LJPotential : public Potential {
public:
    LJPotential() : sigma(1), cutoff_sq(std::numeric_limits<real>::max()) {}
    LJPotential(real _sigma, real _cutoff_sq) : sigma(_sigma), cutoff_sq(_cutoff_sq) {}

    /**
     * @brief calculate the force bewteen the two particles and add it to p
     *
     * @param p particle p
     * @param q particl q
     *
     * @return potential energy
     */
    real force(Particle &p, Particle &q, real distance_sq, vector_t difference);

    real sigma;
    real cutoff_sq;
};

#endif // _GRAVITYPOTENTIAL_HPP
// vim:set et sts=4 ts=4 sw=4 ai ci cin:
