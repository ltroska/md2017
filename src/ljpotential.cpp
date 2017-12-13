#include "ljpotential.hpp"

#include <cmath>
#include <iostream>

real LJPotential::force(Particle& p, Particle &q, real distance_sq, vector_t difference)
{
  vector_t force;
  real const epsilon = 1.0;
  real potential = 0;
  real temp =0.0;

  // TODO: ideal place to execute mixing.

  if (distance_sq > cutoff_sq)
    return 0;

  temp = sigma * sigma / distance_sq; // = (sigma / r)^2
  temp = std::pow(temp,3) ;                 // = (sigma / r)^6


  potential = 4.0 * epsilon * temp *(temp - 1.0); //potential completely computed

  temp *= 48.0 * epsilon * (0.5 - temp) / distance_sq; // temp now contains all of force and only needs direction, spares computations in DIM loop.

  for (std::size_t i = 0; i<DIM; ++i) {
    force[i]= temp * difference[i]; // only multiplying with direction, rest already in temp.
    p.F[i] += force[i];
    q.F[i] -= force[i];
  }

  return potential;
}

// vim:set et sts=4 ts=4 sw=4 ai ci cin:
