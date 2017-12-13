#ifndef _GRAVITYPOTENTIAL_HPP
#define _GRAVITYPOTENTIAL_HPP

#include "potential.hpp"

/**
 * @brief TODO add the documentation
 */
class GravityPotential : public Potential {
public:
    GravityPotential() = default;
    GravityPotential(real _cutoff_sq) : cutoff_sq(_cutoff_sq) {}

    /**
     * @brief calculate the force between the two particles and add it to p
     *
     * @param p particle p
     * @param q particl q
     *
     * @return potential energy
     */
    real force(Particle &p, Particle &q, real distance_sq, vector_t difference_offset = {});

    real cutoff_sq;
};

#endif // _GRAVITYPOTENTIAL_HPP
// vim:set et sts=4 ts=4 sw=4 ai ci cin:
