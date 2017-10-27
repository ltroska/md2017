#include "observer.hpp"

Observer::Observer(World &_W) : W(_W)
{
    // open statistics file
    std::string statistics_filename = W.name + ".statistics";
    // open file, overwrite existing files, take no prisoners
    statistics.open(statistics_filename.c_str());
    // and tell the world
    std::cout << "Opened " << statistics_filename << " for writing." << std::endl;

    // open coordinates file
    std::string coordinates_filename = W.name + ".coordinates";
    // open file, overwrite existing files, take no prisoners
    coordinates.open(coordinates_filename.c_str());
    // and tell the world
    std::cout << "Opened " << coordinates_filename << " for writing." << std::endl;
}


Observer::~Observer()
{
    // close the statistics file
    if ( statistics.is_open() )
        statistics.close();
    // close the coordinates file
    if ( coordinates.is_open() )
        coordinates.close();
}

void Observer::output_statistics()
{
    // write statistics into the filestream, seperated with tabulars
    statistics
        << W.t << "\t"
        << W.e_pot << "\t"
        << W.e_kin << "\t"
        << W.e_kin + W.e_pot << "\t"
        << std::endl;
}

void Observer::output_coordinates()
{
    // write coordinates into the filestream, separated with tabulars
    coordinates << W.t << "\t";
    for (auto &p : W.particles) {
        for (std::size_t d = 0; d < DIM; ++d) {
            coordinates << p.x[d] << "\t";
        }
    }
    
    coordinates << std::endl;
}

void Observer::notify()
{
    // call output functions
    output_statistics();
    output_coordinates();
}

// vim:set et sts=4 ts=4 sw=4 ai ci cin:
