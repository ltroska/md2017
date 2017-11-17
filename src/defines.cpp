//
// Created by troska on 11/17/17.
//
#include "defines.hpp"
#ifdef MD_HAVE_3D
int neighbor_offsets[NUM_NEIGHBORS][3] = {
{-1, -1, -1},
{-1, -1, 0},
{-1, -1, 1},
{-1, 0, -1},
{-1, 0, 0},
{-1, 0, 1},
{-1, 1, -1},
{-1, 1, 0},
{-1, 1, 1},
{0, -1, -1},
{0, -1, 0},
{0, -1, 1},
{0, 0, -1},
{0, 0, 0},
{0, 0, 1},
{0, 1, -1},
{0, 1, 0},
{0, 1, 1},
{1, -1, -1},
{1, -1, 0},
{1, -1, 1},
{1, 0, -1},
{1, 0, 0},
{1, 0, 1},
{1, 1, -1},
{1, 1, 0},
{1, 1, 1}
};
#else
int neighbor_offsets[NUM_NEIGHBORS][2] = {
{-1, -1},
{-1, 0},
{-1, 1},
{0, -1},
{0, 0},
{0, 1},
{1, -1},
{1, 0},
{1, 1}
};
#endif