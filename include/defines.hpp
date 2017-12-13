/** \mainpage MD Praktikum
 *
 *
 * Simulation von Planeten- und Kometenbahnen.
 *
 * \subsection algorithms_subsec Implementierte Verfahren und deren Klassen
 *
 * - GravityPotential das (skalierte) Gravitationspotential \f$U(r_{ij}) = -m_im_j/r_{ij}\f$
 * - LJPotential das Lennard-Jones-Potential
 * - VelocityVerlet mit \f$\dfrac{1}{2}O(N^2)\f$ Kraftberechnungen
 *
 * \subsection classes_subsec Weitere modifizierte Klassen
 *
 * - SubDomain
 * - Observer
 *
 *
 */

#ifndef _DEFINES_HPP
#define _DEFINES_HPP

#include <stdint.h>
#include <limits.h>
#include <cstdlib>
#include <array>

//------------------------------ BASICS ------------------------------//
#ifdef MD_HAVE_3D
#define DIM 3
#define NUM_NEIGHBORS 27
#else
#define DIM 2
    #define NUM_NEIGHBORS 9
#endif

//------------------------------ DATATYPES ------------------------------//

typedef double real;

#define my_MPI_REAL MPI_DOUBLE

#if SIZE_MAX == UCHAR_MAX
    #define my_MPI_SIZE_T MPI_UNSIGNED_CHAR
#elif SIZE_MAX == USHRT_MAX
    #define my_MPI_SIZE_T MPI_UNSIGNED_SHORT
#elif SIZE_MAX == UINT_MAX
    #define my_MPI_SIZE_T MPI_UNSIGNED
#elif SIZE_MAX == ULONG_MAX
    #define my_MPI_SIZE_T MPI_UNSIGNED_LONG
#elif SIZE_MAX == ULLONG_MAX
    #define my_MPI_SIZE_T MPI_UNSIGNED_LONG_LONG
#else
   #error "Can't find MPI_TYPE for std::size_t"
#endif

using index_t = std::array<std::size_t, DIM>;
using vector_t = std::array<real, DIM>;

//------------------------------ NEIGHBOR OFFSETS ------------------------------//

extern int neighbor_offsets[NUM_NEIGHBORS][DIM];


//------------------------------ HELPERS ------------------------------//

#define sqr(_x) ((_x)*(_x))

#endif // _DEFINES_HPP
// vim:set et sts=4 ts=4 sw=4 ai ci cin cino=g0,t0:
