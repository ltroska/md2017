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

//------------------------------ NEIGHBOR OFFSETS ------------------------------//

#ifdef MD_HAVE_3D
    #define DIM 3
    #define NUM_NEIGHBORS 27
#else
    #define DIM 2
    #define NUM_NEIGHBORS 9
#endif

extern int neighbor_offsets[NUM_NEIGHBORS][DIM];


//------------------------------ HELPERS ------------------------------//

#define sqr(_x) ((_x)*(_x))

#ifdef MD_HAVE_3D
#define LOOP_INDEX_OVER_FACE(_index, _limits1, _limits2, _fixed_, _DIM1, _DIM2, _DIM3, _code) \
				for (std::size_t __id1 = _limits1[0]; __id1 < _limits1[1]; ++__id1) { \
					for (std::size_t __id2 = _limits2[0]; __id2 < _limits2[1]; ++__id2) { \
						_index[_DIM1] = __id1;\
						_index[_DIM2] = __id2;\
						_index[_DIM3] = fixed;\
						_code\
					}\
				}

	#define LOOP_INDEX_OVER_XY_FACE(_index, _limits1, _limits2, _fixed_, _code)\
				LOOP_INDEX_OVER_FACE(_index, _limits1, _limits2, _fixed, 0, 1, 2, _code)

	#define LOOP_INDEX_OVER_XZ_FACE(_index, _limits1, _limits2, _fixed_, _code)\
				LOOP_INDEX_OVER_FACE(_index, _limits1, _limits2, _fixed, 0, 2, 1, _code)

	#define LOOP_INDEX_OVER_YZ_FACE(_index, _limits1, _limits2, _fixed_, _code)\
				LOOP_INDEX_OVER_FACE(_index, _limits1, _limits2, _fixed, 1, 2, 0, _code)
#else
#define LOOP_INDEX_OVER_FACE(_index, _limits, _fixed_, _DIM1, _DIM2, _code) \
				for (std::size_t id = _limits[0]; id1 < _limits[1]; ++id) { \
						_index[_DIM1] = id1;\
						_index[_DIM2] = fixed;\
						_code\
					}\
				}
#endif

#endif // _DEFINES_HPP
// vim:set et sts=4 ts=4 sw=4 ai ci cin cino=g0,t0:
