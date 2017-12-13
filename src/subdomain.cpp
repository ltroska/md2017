#include "subdomain.hpp"
#include "logging.hpp"

#include <stdexcept>
#include <sstream>
#include <cmath>
#include <limits>

#ifdef MD_HAVE_3D
    const std::size_t all_but[DIM][DIM - 1] = {{2, 1}, {2, 0}, {1, 0}};
#else
	const std::size_t all_but[DIM][DIM - 1] = {{1}, {0}};
#endif

#ifdef MD_HAVE_3D
#define LOOP_INDEX_OVER_FACE(_index, _limits, _normal_dim, _code)\
	for(std::size_t __id0 = _limits[0][all_but[_normal_dim][0]]; __id0 < _limits[1][all_but[_normal_dim][0]]; ++__id0) {\
		for(std::size_t __id1 = _limits[0][all_but[_normal_dim][1]]; __id1 < _limits[1][all_but[_normal_dim][1]]; ++__id1) {\
			_index[all_but[_normal_dim][0]]=__id0;\
			_index[all_but[_normal_dim][1]]=__id1;\
			_code\
		}\
	}
#else
#define LOOP_INDEX_OVER_FACE(_index, _limits, _normal_dim, _code)\
	for(std::size_t __id = _limits[0][all_but[_normal_dim][0]]; __id < _limits[1][all_but[_normal_dim][0]]; ++__id) {\
			_index[all_but[_normal_dim][0]]=__id;\
			_code\
	}
#endif
SubDomain::SubDomain(int r, int n) : rank(r), np(n), name("unknown"),t(0),delta_t(0),t_end(0),cell_r_cut(0), cell_r_cut_sq(0), e_kin(0),e_pot(0),e_tot(0),
                 sigma(0), output_interval(1), n_total_particles(0), numprocs()
{
    for (std::size_t d = 0; d < DIM; ++d) {
        upper_border[d] = unknown;
        lower_border[d] = unknown;
        total_length[d] = 0;
    }

}

void SubDomain::read_Parameter(const std::string &filename)
{

    // create input filestream
    std::ifstream parfile(filename.c_str());
    // check if file is open
    if (!parfile.is_open())
        throw std::runtime_error(
            "read_Parameter(): Can't open file '" + filename + "' for reading."
            );

    // helper strings
    std::string line, option;

    // read file till eof
    while (parfile.good())
    {
        // read line from file
        getline(parfile,line);
        // create a string stream
        std::stringstream strstr;
        // put line into string stream
        strstr << line;
        // read option and value from stringstream
        strstr >> option;
        // check options and read values
        if (option=="delta_t")
            strstr >> delta_t;
        if (option=="t_end")
            strstr >> t_end;
        if (option=="name")
            strstr >> name;
        if (option=="length") {
            for (std::size_t d = 0; d < DIM; ++d) {
                strstr >> total_length[d];
            }
        }

        if (option=="cell_r_cut") {
            strstr >> cell_r_cut;
        }
        if (option=="sigma") {
            strstr >> sigma;
        }
        if (option=="output_interval") {
            strstr >> output_interval;
        }

        std::string tmp;
        if (option=="upper_border") {
            for (std::size_t d = 0; d < DIM; ++d) {
                strstr >> tmp;
                if (tmp == "leaving")
                    upper_border[d] = leaving;
                else if (tmp == "periodic")
                    upper_border[d] = periodic;
            }
        }

        if (option=="lower_border") {
            for (std::size_t d = 0; d < DIM; ++d) {
                strstr >> tmp;
                if (tmp == "leaving")
                    lower_border[d] = leaving;
                else if (tmp == "periodic")
                    lower_border[d] = periodic;
            }
        }

        if (option=="num_procs") {
            for (std::size_t d = 0; d < DIM; ++d)
                strstr >> numprocs[d];
        }
    }
    // close file
    parfile.close();

    for (std::size_t d = 0; d < DIM; ++d) {
        //numprocs[d] = (int) std::round(std::pow(np, 1. / DIM));
        ghost_border_width[d] = 1;
    }

    indices = from_linear_proc_index(rank);

    for (std::size_t d = 0; d < DIM; ++d) {
        length[d] = total_length[d] / numprocs[d];

        offset[d] = indices[d] * length[d];
    }

    index_t neighbor_index;
    for (std::size_t d = 0; d < DIM; ++d) {
        for (std::size_t j = 0; j < DIM; ++j)
            neighbor_index[j] = indices[j];

        if (indices[d] == 0) {
            if (lower_border[d] == periodic) {
                neighbor_index[d] = numprocs[d] - 1;
                neighbors[d][0] = to_linear_proc_index(neighbor_index);
            } else {
                neighbors[d][0] = -1;
            }
        }
        else {
            neighbor_index[d] = indices[d] - 1;

            neighbors[d][0] = to_linear_proc_index(neighbor_index);
        }

        if (indices[d] == numprocs[d] - 1) {
            if (upper_border[d] == periodic) {
                neighbor_index[d] = 0;
                neighbors[d][1] = to_linear_proc_index(neighbor_index);
            }
            else {
                neighbors[d][1] = -1;
            }
        }
        else {
            neighbor_index[d] = indices[d] + 1;

            neighbors[d][1] = to_linear_proc_index(neighbor_index);
        }
    }

    // if cutoff or total_length not set
    for (std::size_t d = 0; d < DIM; ++d) {
        if (cell_r_cut == 0) {
            cell_length[d] = length[d];
        } else
            cell_length[d] = cell_r_cut;

        if (total_length[d] == 0)
            n_cells[d] = 1 + 2*ghost_border_width[d];
        else
            n_cells[d] = std::ceil(length[d] / cell_length[d]) + 2*ghost_border_width[d];
    }

    if (cell_r_cut == 0) {
        cell_r_cut = std::numeric_limits<real>::max();
        cell_r_cut_sq = std::numeric_limits<real>::max();
    } else {
        cell_r_cut_sq = cell_r_cut * cell_r_cut;
    }

    n_total_cells = 1;

    for(std::size_t d = 0; d < DIM; ++d)
        n_total_cells *= n_cells[d];

    LOG(rank, "total cells: " << n_total_cells);
    LOG(rank, "cells: " << n_cells);
    LOG(rank, "cell length: " << cell_length);
    LOG(rank, "offset: " << offset);

    cells.resize(n_total_cells);

    for (std::size_t i = 0; i < n_cells[0]; ++i) {
        for (std::size_t j = 0; j < n_cells[1]; ++j) {
#ifdef MD_HAVE_3D
            for (std::size_t k = 0; k < n_cells[2]; ++k)
            {
                auto const index = get_linear_index(i, j, k);
                cells[index].index[0] = i;
                cells[index].index[1] = j;
                cells[index].index[2] = k;
            }
#else
            auto const index = get_linear_index(i, j);
            cells[index].index[0] = i;
            cells[index].index[1] = j;
#endif
        }
    }

}

void SubDomain::read_Particles(const std::string &filename)
{
    MPI_PARTICLE = setup_particle_mpi_type();

    // create input filestream
    std::ifstream parfile(filename.c_str());
    // check if file is open
    if (!parfile.is_open())
        throw std::runtime_error(
            "read_Particles(): Can't open file '" + filename + "' for reading."
        );

    // helper strings
    std::string line;
    // read file till eof
    while (parfile.good())
    {
        Particle part;
        // read line from file
        getline(parfile,line);

        if (!line.empty()) {
            // create a string stream
            std::stringstream strstr;
            // put line into string stream
            strstr << line;
            // read option and value from stringstream
            strstr >> part.id;
            strstr >> part.m;

            for (std::size_t d = 0; d < DIM; ++d) {
                strstr >> part.x[d];
                part.F[d] = 0;
                part.F_old[d] = 0;
            }

            for (std::size_t d = 0; d < DIM; ++d) {
                strstr >> part.v[d];
            }

            if(is_in_domain(part)) {
                auto const cell_index = get_cell_index(part);

                cells[cell_index].particles.emplace_back(std::move(part));
            }
        }
    }

    LOG(rank, "total particles: " << n_total_particles);

    // close file
    parfile.close();
}

std::size_t SubDomain::get_cell_index(Particle const &p) {
    index_t new_index{};

    for (std::size_t d = 0; d < DIM; ++d) {
        new_index[d] = std::floor((p.x[d] - offset[d] + ghost_border_width[d]*cell_length[d]) / cell_length[d]);
    }

    return get_linear_index(new_index);
}

void SubDomain::communicate_boundary() {
    std::array<index_t, 2> limits;
    index_t index;
    std::size_t cell_index;
    std::size_t ghost_index;
    std::size_t old_ghost_size;
    int num = 0;
    int tag = 0;
    MPI_Status status;
    MPI_Request req;
    
    for (std::size_t d = 0; d < DIM; ++d) {
        limits[0][d] = ghost_border_width[d];
        limits[1][d] = n_cells[d] - ghost_border_width[d];
    }
    
    for (std::size_t d = 0; d < DIM; ++d) {
        LOOP_INDEX_OVER_FACE(index, limits, d,

                             tag = indices[all_but[d][0]];

                             for (std::size_t j = 1; j < DIM - 1; ++j) {
                                 tag = tag*n_cells[all_but[d][j]] + indices[all_but[d][j]];
                             }

                                 // first half step
                                 //
                                 if (neighbors[d][0] >= 0) {

                                    index[d] = ghost_border_width[d];
                                    cell_index = get_linear_index(index);

                                   //

                                     MPI_Isend(&cells[cell_index].particles[0], cells[cell_index].particles.size(),
                                               MPI_PARTICLE, neighbors[d][0], tag, MPI_COMM_WORLD, &req);
                                     MPI_Request_free(&req);
                                 }

                                 if (neighbors[d][1] >= 0) {
                                     index[d] = n_cells[d] - 1;
                                     ghost_index = get_linear_index(index);



                                     MPI_Probe(neighbors[d][1], tag, MPI_COMM_WORLD, &status);
                                     MPI_Get_count(&status, MPI_PARTICLE, &num);

                                     old_ghost_size = cells[ghost_index].particles.size();
                                     cells[ghost_index].particles.resize(old_ghost_size + num);

                                     MPI_Recv(&cells[ghost_index].particles[old_ghost_size], num, MPI_PARTICLE, neighbors[d][1], tag,
                                              MPI_COMM_WORLD, MPI_STATUS_IGNORE);


                                 }
                                 // second half step
                                 if (neighbors[d][1] >= 0) {

                                     index[d] = n_cells[d] - 1 - ghost_border_width[d];
                                     cell_index = get_linear_index(index);

                                     MPI_Isend(&cells[cell_index].particles[0], cells[cell_index].particles.size(),
                                               MPI_PARTICLE, neighbors[d][1], tag, MPI_COMM_WORLD, &req);
                                     MPI_Request_free(&req);
                                 }

                                 if (neighbors[d][0] >= 0) {

                                     index[d] = 0;
                                     ghost_index = get_linear_index(index);

                                     MPI_Probe(neighbors[d][0], tag, MPI_COMM_WORLD, &status);
                                     MPI_Get_count(&status, MPI_PARTICLE, &num);


                                     old_ghost_size = cells[ghost_index].particles.size();
                                     cells[ghost_index].particles.resize(old_ghost_size + num);
                                     MPI_Recv(&cells[ghost_index].particles[old_ghost_size], num, MPI_PARTICLE, neighbors[d][0], tag,
                                              MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                                 }

        );
        limits[0][d] = 0;
        limits[1][d] = n_cells[d];
    }
}

void SubDomain::communicate_ghostborder() {
    std::array<index_t, 2> limits;
    index_t index;
    std::size_t cell_index;
    std::size_t ghost_index;
    std::size_t old_particle_size;
    int num = 0;
    int tag = 0;
    MPI_Status status;
    MPI_Request req;

    for (std::size_t d = 0; d < DIM; ++d) {
        limits[0][d] = 0;
        limits[1][d] = n_cells[d];
    }

    for (std::size_t d = 0; d < DIM; ++d) {
        LOOP_INDEX_OVER_FACE(index, limits, d,
                             tag = indices[all_but[d][0]];

                                 for (std::size_t j = 1; j < DIM - 1; ++j) {
                                     tag = tag*n_cells[all_but[d][j]] + indices[all_but[d][j]];
                                 }

                                 // first half step
                                 //
                                 if (neighbors[d][0] >= 0) {

                                     index[d] = 0;
                                     ghost_index = get_linear_index(index);

                                     MPI_Isend(&cells[ghost_index].particles[0], cells[ghost_index].particles.size(),
                                               MPI_PARTICLE, neighbors[d][0], tag, MPI_COMM_WORLD, &req);
                                     MPI_Request_free(&req);
                                 }

                                 if (neighbors[d][1] >= 0) {

                                     index[d] = n_cells[d] - 1 - ghost_border_width[d];
                                     cell_index = get_linear_index(index);


                                     MPI_Probe(neighbors[d][1], tag, MPI_COMM_WORLD, &status);
                                     MPI_Get_count(&status, MPI_PARTICLE, &num);

                                     old_particle_size = cells[cell_index].particles.size();
                                     cells[cell_index].particles.resize(old_particle_size + num);


                                     MPI_Recv(&cells[cell_index].particles[old_particle_size], num, MPI_PARTICLE, neighbors[d][1], tag,
                                              MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                                 }
                                 // second half step
                                 if (neighbors[d][1] >= 0) {

                                     index[d] = n_cells[d] - 1;
                                     ghost_index = get_linear_index(index);

                                     MPI_Isend(&cells[ghost_index].particles[0], cells[ghost_index].particles.size(),
                                               MPI_PARTICLE, neighbors[d][1], tag, MPI_COMM_WORLD, &req);
                                     MPI_Request_free(&req);
                                 }

                                 if (neighbors[d][0] >= 0) {
                                     index[d] = ghost_border_width[d];
                                     cell_index = get_linear_index(index);

                                     MPI_Probe(neighbors[d][0], tag, MPI_COMM_WORLD, &status);
                                     MPI_Get_count(&status, MPI_PARTICLE, &num);


                                     old_particle_size = cells[cell_index].particles.size();
                                     cells[cell_index].particles.resize(old_particle_size + num);
                                     MPI_Recv(&cells[cell_index].particles[old_particle_size], num, MPI_PARTICLE, neighbors[d][0], tag,
                                              MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                                 }

        );
        limits[0][d] = 1;
        limits[1][d] = n_cells[d] - ghost_border_width[d];
    }
}

void SubDomain::clear_ghostborder() {
    std::array<index_t, 2> limits;
    index_t index;
    std::size_t ghost_index;

    for (std::size_t d = 0; d < DIM; ++d) {
        limits[0][d] = 0;
        limits[1][d] = n_cells[d];
    }

    for (std::size_t d = 0; d < DIM; ++d) {
        LOOP_INDEX_OVER_FACE(index, limits, d,
                              index[d] = 0;
                             ghost_index = get_linear_index(index);
                             cells[ghost_index].particles.clear();

                             index[d] = n_cells[d] - 1;
                             ghost_index = get_linear_index(index);
                             cells[ghost_index].particles.clear();
        );
    }
}

std::ostream& operator << (std::ostream& os, SubDomain& W) {
    os << "t=" << W.t << " delta_t=" << W.delta_t << " t_end=" << W.t_end
       << "\ncutoff=" << W.cell_r_cut << " cutoff_sq=" << W.cell_r_cut_sq
       << "\nNumber of Particles=" << W.n_total_particles << " Number of Cells=" << W.cells.size();

    os << "\nCells: ";
    for (std::size_t d = 0; d < DIM; ++d)
        os << W.n_cells[d] << " ";

    os  << "\ntotal_length: ";
    for (std::size_t d = 0; d < DIM; ++d)
        os << W.total_length[d] << " ";



    os << "\nupper_border: ";
    for (std::size_t d = 0; d < DIM; ++d)
        os << BorderTypes[W.upper_border[d]] << " ";

    os << "\nlower_border: ";
    for (std::size_t d = 0; d < DIM; ++d)
        os << BorderTypes[W.lower_border[d]] << " ";
    return os;
}
// vim:set et sts=4 ts=4 sw=4 ai ci cin:
