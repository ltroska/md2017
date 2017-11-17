#include "ljpotential.hpp"

#include <cmath>
#include <iostream>

real LJPotential::force(Particle& p, Particle &q, real distance_sq)
{
  real force[DIM];
  real difference[DIM];
  real const sigma = 1.0;
  real const epsilon = 1.0;
  real potential = 0;
  real temp =0.0;

  // TODO: ideal place to execute mixing.

  for(std::size_t i = 0; i<DIM; ++i) {
    difference[i] = q.x[i]-p.x[i];
  }


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
