#include <cstdlib>
#include <iostream>

#include "subdomain.hpp"
#include "gravitypotential.hpp"
#include "velocityverlet.hpp"

int main(int argc, char *argv[]) {

  // check arguments
  if (argc < 2) {
    std::cerr << "error: missing arguments" << std::endl;
    std::cerr << "usage: " << std::endl
              << "\t" << argv[0] << " parameterfile particledatafile" << std::endl;
    return EXIT_FAILURE;
  }

  // instanciate Potential
  GravityPotential Pot;

  // create World
  SubDomain S;

  // read Parameters
  S.read_Parameter(argv[1]);

  // read Particles
  S.read_Particles(argv[2]);

  // print SubDomain configuration
  std::cout << S << std::endl;

  // create the Observer
  Observer O(S, "../output/");

  // instanciate timediscretization 
  // remark: & is used to get the address of Pot
  VelocityVerlet Verlet(S, &Pot, O);

  // run the simulation
  Verlet.simulate();

  return EXIT_SUCCESS;
}

// vim:set et sts=2 ts=2 sS=2 ai ci cin:
