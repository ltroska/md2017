#ifndef _OBSERVER_HPP
#define _OBSERVER_HPP

#include "subdomain.hpp"
#include <iostream>
#include <fstream>
#include <queue>

/**
 * @brief an observer for the timediscretization
 */
class Observer {
public:
    /**
     * @brief constructor
     *
     * opens and creates the files written during observation
     *
     * @param _S
     */
    Observer(SubDomain& _S, std::string const& out_prefix);

    /**
     * @brief destructor
     *
     * closes the files written during the obervation
     */
    ~Observer();

    /**
     * @brief notify the observer that the world has changed
     */
    void notify();

    /** 
     * @brief output statistics like kinetic, potential and total energy
     */
    void output_statistics();

    /** 
     * @brief output the coordinates of the particles
     */
    void output_coordinates();

    /**
     * @brief output the coordinates of the particles in xyz format
     */
    void output_xyz();

protected:
    /// The world we are observing
    SubDomain &W;

    std::vector<real> energies;

    std::string output_prefix;

    /// Statistics filestream
    std::ofstream statistics;
    /// coordinates filestream
    std::ofstream coordinates;
    /// xyz filestream
    std::ofstream xyz_out;

private:
    /// Disabled Constructor
    Observer();
};

#endif // _OBSERVER_HPP
// vim:set et sts=4 ts=4 sw=4 ai ci cin:
