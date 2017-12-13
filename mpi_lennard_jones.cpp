#include "subdomain.hpp"
#include "ljpotential.hpp"
#include "velocityverlet.hpp"

#include "logging.hpp"

#include <cstdlib>
#include <iostream>

#include <unistd.h>

#include <mpi.h>

int main(int argc, char *argv[]) {

    // check arguments
    if (argc < 2) {
        std::cerr << "error: missing arguments";
        std::cerr << "usage: "
                  << "\t" << argv[0] << " parameterfile particledatafile";
        return EXIT_FAILURE;
    }

    int rank, numprocs;

  /*  int i = 0;
    char hostname[256];
    gethostname(hostname, sizeof(hostname));
    printf("PID %d on %s ready for attach\n", getpid(), hostname);
    fflush(stdout);
    while (0 == i)
        sleep(5);*/

    MPI_Status status;
    MPI_Init (&argc, &argv);
    MPI_Comm_size (MPI_COMM_WORLD, &numprocs);
    MPI_Comm_rank (MPI_COMM_WORLD, &rank);

//
//
    SubDomain S(rank, numprocs);
    S.read_Parameter(argv[1]);
    LJPotential Pot(S.sigma, S.cell_r_cut_sq);

    S.read_Particles(argv[2]);

    Observer O(S, "../output/");

    VelocityVerlet Verlet(S, &Pot, O);

    Verlet.simulate();

    MPI_Finalize ();

   /* // instanciate Potential

    // create World
    SubDomain W;

    // read Parameters
    W.read_Parameter(argv[1]);

    // read Particles
    W.read_Particles(argv[2]);

    // print SubDomain configuration
    std::cout << W;

    // create the Observer
    Observer O(W, "../output/");

    // instanciate timediscretization
    // remark: & is used to get the address of Pot
    VelocityVerlet Verlet(W, &Pot, O);

    // run the simulation
    Verlet.simulate();*/

    return EXIT_SUCCESS;
}

// vim:set et sts=2 ts=2 sw=2 ai ci cin:
