#include "gravitypotential.hpp"
#include <cmath>
#include <iostream>

real GravityPotential::force(Particle &p, Particle &q, real cutoff_sq, real difference_offset[DIM])
{
    real diff[DIM];
    real r_sq = 0;

    for (std::size_t d = 0; d < DIM; ++d) {
        diff[d] = q.x[d] - (p.x[d] + difference_offset[d]);
        r_sq += sqr(diff[d]);
    }

    if (r_sq > cutoff_sq)
        return 0;

    const auto r = std::sqrt(r_sq);

    const auto potential = -p.m*q.m / r;

    const auto factor = p.m*q.m/(r*r_sq);

    for (std::size_t d = 0; d < DIM; ++d) {
        p.F[d] += factor * diff[d];
        q.F[d] -= factor * diff[d];
    }

    return potential;
}