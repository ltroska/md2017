#include <cstdlib>
#include <iostream>

#include "world.hpp"
#include "ljpotential.hpp"
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
    LJPotential Pot;

    // create Worldtest
    World W;

    // read Parameters
    W.read_Parameter(argv[1]);

    // read Particles
    W.read_Particles(argv[2]);

    // print World configuration
    std::cout << W << std::endl;

    // create the Observer
    Observer O(W);

    // instanciate timediscretization
    // remark: & is used to get the address of Pot
    VelocityVerlet Verlet(W, &Pot, O);

    // run the simulation
    Verlet.simulate();

    return EXIT_SUCCESS;
}

// vim:set et sts=2 ts=2 sw=2 ai ci cin:
