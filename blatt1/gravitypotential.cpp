#include "gravitypotential.hpp"

real GravityPotential::force(Particle &p, Particle &q)
{
    const auto r = std::sqrt((p.x[0]-q.x[0])*(p.x[0]-q.x[0]) + (p.x[1]-q.x[1])*(p.x[1]-q.x[1]));

    const auto potential = -p.m*q.m / r;
        
    return potential;
}

// vim:set et sts=4 ts=4 sw=4 ai ci cin:
