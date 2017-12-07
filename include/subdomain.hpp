#ifndef _SUBDOMAIN_HPP
#define _SUBDOMAIN_HPP

#include "defines.hpp"
#include "particle.hpp"
#include "bordertype.hpp"
#include "cell.hpp"
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
    SubDomain();

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

#ifdef MD_HAVE_3D
    inline std::size_t get_linear_index(std::size_t i, std::size_t j, std::size_t k) {
        return i + n_cells[0]*(j + n_cells[1]*k);
    }

    inline std::size_t get_linear_index(std::size_t idx[3]) {
        return get_linear_index(idx[0], idx[1], idx[2]);
    }
#else
    inline std::size_t get_linear_index(std::size_t i, std::size_t j) {
        return i + n_cells[0] * j;
    }

    inline std::size_t get_linear_index(std::size_t idx[2]) {
        return get_linear_index(idx[0], idx[1]);
    }
#endif

    std::size_t get_cell_index(Particle const& p);

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


    std::size_t n_total_particles;
    /// Vector of cells
    std::vector<Cell> cells;
    /// Number of cells in each dimension
    std::size_t n_cells[DIM];

    std::size_t n_total_cells;
    /// Length of each cell dimension
    real cell_length[DIM];


    std::array<double, DIM> length;

    std::array<BorderType, DIM> lower_border;
    std::array<BorderType, DIM> upper_border;
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
