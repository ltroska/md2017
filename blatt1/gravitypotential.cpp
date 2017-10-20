#include "gravitypotential.hpp"
#include <cmath>
#include <iostream>

real GravityPotential::force(Particle &p, Particle &q)
{
    const auto x_diff = q.x[0]-p.x[0];
    const auto y_diff = q.x[1]-p.x[1];
    const auto r_sq = x_diff*x_diff + y_diff*y_diff;
    const auto r = std::sqrt(r_sq);

    const auto potential = -p.m*q.m / r;

    const auto factor = p.m*q.m/(r*r_sq);

    p.F[0] += factor * x_diff;
    p.F[1] += factor * y_diff;

    return potential;
}