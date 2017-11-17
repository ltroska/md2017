#include "gravitypotential.hpp"
#include <cmath>
#include <iostream>

real GravityPotential::force(Particle &p, Particle &q, real distance = 0)
{
    real diff[DIM];
    real r_sq = 0;

    for (std::size_t d = 0; d < DIM; ++d) {
        diff[d] = q.x[d] - p.x[d];
        r_sq += diff[d] * diff[d];
    }

    if (r_sq > r_cutoff_sq) {

        std::cout << "cutoff" << std::endl;
        return 0;
    }

    const auto r = std::sqrt(r_sq);

    const auto potential = -p.m*q.m / r;

    const auto factor = p.m*q.m/(r*r_sq);

    for (std::size_t d = 0; d < DIM; ++d) {
        p.F[d] += factor * diff[d];
        q.F[d] -= factor * diff[d];
    }

    return potential;
}