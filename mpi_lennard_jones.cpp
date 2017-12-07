#include <cstdlib>
#include <iostream>

#include <mpi.h>

#include "subdomain.hpp"
#include "ljpotential.hpp"
#include "velocityverlet.hpp"

#include "logging.hpp"

int main(int argc, char *argv[]) {

//    // check arguments
//    if (argc < 2) {
//        std::cerr << "error: missing arguments";
//        std::cerr << "usage: "
//                  << "\t" << argv[0] << " parameterfile particledatafile";
//        return EXIT_FAILURE;
//    }

    int rank, numprocs;

    MPI_Status status;
    MPI_Init (&argc, &argv);
    MPI_Comm_size (MPI_COMM_WORLD, &numprocs);
    MPI_Comm_rank (MPI_COMM_WORLD, &rank);

    LOG(rank, "Hello");
    MPI_Datatype MPI_PARTICLE = setup_particle_mpi_type();

    int source = 1 - rank;
    int dest = source;
    int tag = 0;

    std::vector<Particle> v_send;
    std::vector<Particle> v_recv;

    v_send.reserve(10);

    LOG_DBG_RANK(rank, "filling send vector");

    for (std::size_t i = 0; i < 10; ++i) {
        Particle p;
        p.id = i + 100*rank;
        p.m = 3.4+i + 100*rank;

        for (std::size_t d = 0; d < DIM; ++d) {
            p.x[d] = i + 1.2 + d + 100*rank;
            p.v[d] = i + 5.3 + d + 100*rank;
            p.F[d] = i + 9.4 + d + 100*rank;
            p.F_old[d] = i + 13.5 + d + 100*rank;
        }

        LOG_DBG_RANK(rank, p);
        v_send.emplace_back(p);
    }

    LOG_DBG_RANK(rank, "ISend to " << dest);

    MPI_Request req;

    MPI_Isend(&v_send[0], v_send.size(), MPI_PARTICLE, dest, tag, MPI_COMM_WORLD, &req);
    MPI_Request_free(&req);

    LOG_DBG_RANK(rank, "Probing receive");
    MPI_Status recv_status;
    MPI_Probe(source, tag, MPI_COMM_WORLD, &recv_status);
    int num;

    MPI_Probe(source, tag, MPI_COMM_WORLD, &recv_status);
    MPI_Get_count(&recv_status, MPI_PARTICLE, &num);

    LOG_DBG_RANK(rank, "Receiving " << num);
    v_recv.resize(num);

    MPI_Recv(&v_recv[0], num, MPI_PARTICLE, source, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    LOG_DBG_RANK(rank, "Received");
    for (auto& p : v_recv)
        LOG_DBG_RANK(rank, p);

    MPI_Finalize ();

   /* // instanciate Potential
    LJPotential Pot;

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
