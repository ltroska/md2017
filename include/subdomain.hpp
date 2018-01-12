#ifndef _SUBDOMAIN_HPP
#define _SUBDOMAIN_HPP

#include "defines.hpp"
#include "particle.hpp"
#include "bordertype.hpp"
#include "cell.hpp"


#include "mpi.h"
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <array>
#include <cmath>
#include <tuple>

/**
 * @brief the world class holds all information of the simulation environment
 */
class SubDomain {
public:
    SubDomain(int rank, int numprocs);

    /**
     * @brief read the world parameters from the given parameter file
     *
     * parameter file example
     * \code
     * delta_t 0.1
     * t_end 1.0
     * \endcode
     *
     * @param filename filename of the parameter file
     */
    void read_Parameter(const std::string &filename);

    /**
     * @brief read the particles from the given data file
     *
     * @param filename filename of the particle data file
     */
    void read_Particles(const std::string &filename);

    void communicate_boundary();

    void communicate_ghostborder();

    void clear_ghostborder();

#ifdef MD_HAVE_3D
    inline std::size_t get_linear_index(std::size_t i, std::size_t j, std::size_t k) {
        return i + n_cells[0]*(j + n_cells[1]*k);
    }
    inline std::size_t get_linear_index(index_t idx) {
        return get_linear_index(idx[0], idx[1], idx[2]);
    }

    inline index_t from_linear_proc_index(std::size_t idx) {
        index_t result;

        result[0] = idx % numprocs[0];
        idx -= result[0];
        idx /= numprocs[0];
        result[1] = idx % numprocs[1];
        result[2] = idx / numprocs[1];

        return result;
    }

    inline int to_linear_proc_index(std::size_t i, std::size_t j, std::size_t k) {
        return i + numprocs[0]*(j + numprocs[1]*k);
    }

    inline int to_linear_proc_index(index_t idx) {
        return to_linear_proc_index(idx[0], idx[1], idx[2]);
    }

#else
    inline std::size_t get_linear_index(std::size_t i, std::size_t j) {
        return i + n_cells[0] * j;
    }

    inline std::size_t get_linear_index(index_t idx) {
        return get_linear_index(idx[0], idx[1]);
    }

    inline index_t from_linear_proc_index(std::size_t idx) {
        index_t result;

        result[0] = idx % numprocs[0];
        result[1] = idx / numprocs[0];

        return result;
    }

     inline int to_linear_proc_index(std::size_t i, std::size_t j) {
        return i + numprocs[0]*j;
    }

    inline int to_linear_proc_index(index_t idx) {
        return to_linear_proc_index(idx[0], idx[1]);
    }
#endif

    inline bool is_in_domain(Particle& p) {
        for(std::size_t d = 0; d < DIM; ++d)
            if(p.x[d] < offset[d] || p.x[d] >= offset[d] + length[d])
                return false;

        return true;
    }

    std::size_t get_cell_index(Particle const& p);

    int rank;
    int np;

    // data structures
    /// Name of the simulated world
    std::string name;
    /// Current time
    real t;
    /// Timestep
    real delta_t;
    /// End of simulation
    real t_end;
    /// cutoff
    real cell_r_cut;
    real cell_r_cut_sq;
    /// kinetic energy
    real e_kin;
    /// potential energy
    real e_pot;
    /// total energy
    real e_tot;

    real sigma;

    real set_start_temperature;

    std::size_t thermostat_step_interval;

    real thermostat_target_temperature;

    int random_seed;

    std::size_t output_interval;


    std::size_t n_total_particles;

    std::array<std::size_t, DIM> indices;

    std::array<std::size_t, DIM> ghost_border_width;

    /// Vector of cells
    std::vector<Cell> cells;
    /// Number of cells in each dimension
    std::array<std::size_t, DIM> n_cells;

    std::size_t n_total_cells;
    /// Length of each cell dimension
    vector_t cell_length;


    vector_t total_length;
    vector_t length;
    vector_t offset;


    std::array<BorderType, DIM> lower_border;
    std::array<BorderType, DIM> upper_border;

    std::array<std::array<int, 2>, DIM> neighbors;

    std::array<std::size_t, DIM> numprocs;

    MPI_Datatype MPI_PARTICLE;
};

/**
 * @brief a ostream operator for the SubDomain class
 *
 * @param os stream object
 * @param W the world
 *
 * @return resulting stream object
 */
std::ostream& operator << (std::ostream& os, SubDomain& W);

#endif // _WORLD_HPP
// vim:set et sts=4 ts=4 sw=4 ai ci cin:
