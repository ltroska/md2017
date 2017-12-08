#ifndef _GRAVITYPOTENTIAL_HPP
#define _GRAVITYPOTENTIAL_HPP

#include "potential.hpp"

/**
 * @brief TODO add the documentation
 */
class LJPotential : public Potential {
public:
    /**
     * @brief calculate the force bewteen the two particles and add it to p
     *
     * @param p particle p
     * @param q particl q
     *
     * @return potential energy
     */
    real force(Particle &p, Particle &q, real cutoff_sq = std::numeric_limits<real>::max(), real difference_offset[DIM] = {0});

    real sigma;
};

#endif // _GRAVITYPOTENTIAL_HPP
// vim:set et sts=4 ts=4 sw=4 ai ci cin:
