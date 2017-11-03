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
 * - World
 * - Observer
 *
 *
 */

#ifndef _DEFINES_HPP
#define _DEFINES_HPP

// define the dimension of the particles
#ifdef MD_HAVE_3D
#define DIM 3
#else
#define DIM 2
#endif
// reals in double precision
typedef double real;
// squre define
#define sqr(_x) ((_x)*(_x))

#endif // _DEFINES_HPP
// vim:set et sts=4 ts=4 sw=4 ai ci cin cino=g0,t0:
