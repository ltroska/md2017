#include "world.hpp"
#include <stdexcept>
#include <sstream>
#include <cmath>
#include <limits>

World::World() : name("unknown"),t(0),delta_t(0),t_end(0),cell_r_cut(0), cell_r_cut_sq(0), e_kin(0),e_pot(0),e_tot(0),
                 n_total_particles(0)
{
    for (std::size_t d = 0; d < DIM; ++d) {
        upper_border[d] = unknown;
        lower_border[d] = unknown;
        length[d] = 0;
    }

}

void World::read_Parameter(const std::string &filename)
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
                strstr >> length[d];
            }
        }
        if (option=="cell_r_cut") {
            strstr >> cell_r_cut;
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
    }
    // close file
    parfile.close();

    // if cutoff or length not set
    for (std::size_t d = 0; d < DIM; ++d) {
        if (cell_r_cut == 0) {
            cell_length[d] = length[d];
        } else
            cell_length[d] = cell_r_cut;

        if (length[d] == 0)
            n_cells[d] = 1;
        else
            n_cells[d] = std::ceil(length[d] / cell_length[d]);
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

void World::read_Particles(const std::string &filename)
{
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
            }

            for (std::size_t d = 0; d < DIM; ++d) {
                strstr >> part.v[d];
            }

            auto const cell_index = get_cell_index(part);

            cells[cell_index].particles.emplace_back(std::move(part));

            ++n_total_particles;
        }
    }

    // close file
    parfile.close();
}

std::size_t World::get_cell_index(Particle const &p) {
    std::size_t new_index[DIM] = {0};

    for (std::size_t d = 0; d < DIM; ++d) {
        if (p.x[d] < 0) {
            switch(lower_border[d]) {
                // probably dont need periodic case here
//                case periodic:
//                    new_index[d] = n_cells[d] - 1;
//                    break;
                case unknown:
                    new_index[d] = 0;
                    break;
                case leaving:
                    return n_total_cells;
                default:
                    return -1;
            }
        } else if (p.x[d] > length[d]) {
            switch(upper_border[d]) {
                // probably dont need periodic case here
//                case periodic:
//                    new_index[d] = 0;
//                    break;
                case unknown:
                    new_index[d] = n_cells[d] - 1;
                    break;
                case leaving:
                    return n_total_cells;
                default:
                    return -1;
            }

        }
        else {
            new_index[d] = std::floor(p.x[d] / cell_length[d]);
        }
    }

    return get_linear_index(new_index);
}

std::ostream& operator << (std::ostream& os, World& W) {
    os << "t=" << W.t << " delta_t=" << W.delta_t << " t_end=" << W.t_end
       << "\ncutoff=" << W.cell_r_cut << " cutoff_sq=" << W.cell_r_cut_sq
       << "\nNumber of Particles=" << W.n_total_particles << " Number of Cells=" << W.cells.size();

    os << "\nCells: ";
    for (std::size_t d = 0; d < DIM; ++d)
        os << W.n_cells[d] << " ";

    os  << "\nlength: ";
    for (std::size_t d = 0; d < DIM; ++d)
        os << W.length[d] << " ";



    os << "\nupper_border: ";
    for (std::size_t d = 0; d < DIM; ++d)
        os << BorderTypes[W.upper_border[d]] << " ";

    os << "\nlower_border: ";
    for (std::size_t d = 0; d < DIM; ++d)
        os << BorderTypes[W.lower_border[d]] << " ";
    return os;
}
// vim:set et sts=4 ts=4 sw=4 ai ci cin:
