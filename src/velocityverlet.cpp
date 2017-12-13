#include "velocityverlet.hpp"

extern int neighbor_offsets[NUM_NEIGHBORS][DIM];

#ifdef MD_HAVE_3D
#define LOOP_INDEX(_index, _limits, _code)\
	for(std::size_t __id0 = _limits[0][0]; __id0 < _limits[1][0]; ++__id0) {\
		for(std::size_t __id1 = _limits[0][1]; __id1 < _limits[1][1]; ++__id1) {\
            for(std::size_t __id2 = _limits[0][2]; __id2 < _limits[1][2]; ++__id2) {\
                _index[0]=__id0;\
                _index[1]=__id1;\
                _index[2]=__id2;\
                _code\
            }\
		}\
	}
#else
#define LOOP_INDEX(_index, _limits, _code)\
	for(std::size_t __id0 = _limits[0][0]; __id0 < _limits[1][0]; ++__id0) {\
		for(std::size_t __id1 = _limits[0][1]; __id1 < _limits[1][1]; ++__id1) {\
            _index[0]=__id0;\
            _index[1]=__id1;\
            _code\
		}\
	}
#endif

VelocityVerlet::VelocityVerlet(SubDomain& _S, Potential& _Pot, Observer& _O) : TimeDiscretization(_S,_Pot,_O)
{
    // empty constructor
}

VelocityVerlet::VelocityVerlet(SubDomain& _S, Potential* _Pot, Observer& _O) : TimeDiscretization(_S,(*_Pot),_O)
{
}

void VelocityVerlet::simulate()
{
    // while simulation end time not reached
    for (auto& c : W.cells)
        for (auto& p : c.particles)
            for (std::size_t d = 0; d < DIM; ++d)
                p.F[d] = 0;

    W.communicate_boundary();
    comp_F();
    W.clear_ghostborder();
    iter = 0;

    std::size_t total_iters = std::ceil(W.t_end / W.delta_t);

    while (W.t < W.t_end) {
        timestep(W.delta_t);
        if (iter % W.output_interval == 0 && W.rank == 0)
            std::cout << "Iteration " << iter << "/" << total_iters << " (" <<  iter/(double)total_iters * 100 << "%) done"
                      << std::endl;
        ++iter;
    }
}

void VelocityVerlet::timestep(real delta_t)
{
    W.t += delta_t;

    update_X();
    W.communicate_ghostborder();
    W.clear_ghostborder();
    W.communicate_boundary();
    comp_F();
    W.clear_ghostborder();
    update_V();

    // notify observer
    if (iter % W.output_interval == 0)
        O.notify();
}

void VelocityVerlet::comp_F() {
    W.e_pot = 0;
    index_t neighbor_cell_index{};
    std::size_t neighbor_cell_linear_index;
    real distance_sqr = 0;
    vector_t difference{};
    real potential = 0;

    index_t index{};
    std::array<index_t, 2> limits{};
    for (std::size_t d = 0; d < DIM; ++d) {
        limits[0][d] = W.ghost_border_width[d];
        limits[1][d] = W.n_cells[d] - W.ghost_border_width[d];
    }

    LOOP_INDEX(index, limits,
        auto const idx = W.get_linear_index(index);
        auto& c = W.cells[idx];
        for (std::size_t n = 0; n < NUM_NEIGHBORS; ++n) {
            for (std::size_t d = 0; d < DIM; ++d)
                neighbor_cell_index[d] = c.index[d] + neighbor_offsets[n][d];

            neighbor_cell_linear_index = W.get_linear_index(neighbor_cell_index);

            for (auto &p : c.particles) {
                // compute force
                for (auto &q : W.cells[neighbor_cell_linear_index].particles) {
                    if (p.id < q.id || !W.is_in_domain(q)) {
                        distance_sqr = 0;

                        for (std::size_t d = 0; d < DIM; ++d) {
                            if (c.index[d] == W.ghost_border_width[d] && neighbor_cell_index[d] == 0 && W.indices[d] == 0 &&
                                W.lower_border[d] == periodic) {
                                difference[d] = q.x[d]- p.x[d] - W.length[d];
                            } else if (neighbor_cell_index[d] == W.n_cells[d] - 1 && c.index[d] == W.n_cells[d] - W.ghost_border_width[d] - 1
                                        && W.indices[d] == W.numprocs[d] - 1 &&  W.upper_border[d] == periodic) {
                                difference[d] = q.x[d]- p.x[d] + W.length[d];
                            } else {
                                difference[d] = q.x[d] - p.x[d];
                            }

                            distance_sqr += sqr(difference[d]);
                        }

                        potential = Pot.force(p, q, distance_sqr, difference);
                        if (!W.is_in_domain(q))
                            potential /= 2;
                        W.e_pot += potential;
                    }
                }
            }
        }
    );
}


void VelocityVerlet::update_V()
{
    index_t index;
    std::array<index_t, 2> limits;
    for (std::size_t d = 0; d < DIM; ++d) {
        limits[0][d] = W.ghost_border_width[d];
        limits[1][d] = W.n_cells[d] - W.ghost_border_width[d];
    }

    W.e_kin = 0;
    real tmp;
    LOOP_INDEX(index, limits,
               auto const idx = W.get_linear_index(index);
                auto& c = W.cells[idx];
                for (auto &p : c.particles) {
                    tmp = 0;

                    for (std::size_t d = 0; d < DIM; ++d) {
                        p.v[d] = p.v[d] + W.delta_t * .5 / p.m * (p.F_old[d] + p.F[d]);
                        tmp += p.v[d] * p.v[d];
                    }

                    W.e_kin += 0.5 * p.m * tmp;
                }
    );

    real global_sum;
    MPI_Reduce(&W.e_kin, &global_sum, 1, my_MPI_REAL, MPI_SUM, 0, MPI_COMM_WORLD);

    W.e_kin = global_sum;

    MPI_Reduce(&W.e_pot, &global_sum, 1, my_MPI_REAL, MPI_SUM, 0, MPI_COMM_WORLD);

    W.e_pot = global_sum;
}

void VelocityVerlet::update_X()
{
    index_t index;
    std::array<index_t, 2> limits;
    bool keep_it;
    for (std::size_t d = 0; d < DIM; ++d) {
        limits[0][d] = W.ghost_border_width[d];
        limits[1][d] = W.n_cells[d] - W.ghost_border_width[d];
    }

    LOOP_INDEX(index, limits,
               auto const idx = W.get_linear_index(index);
                auto& c = W.cells[idx];

                for (std::size_t p_id = 0; p_id < c.particles.size(); ++p_id) {
                    auto &p = c.particles[p_id];
                    // update x
                    for (std::size_t d = 0; d < DIM; ++d) {
                        p.x[d] = p.x[d] + W.delta_t * (p.v[d] + .5 / p.m * p.F[d] * W.delta_t);

                        p.F_old[d] = p.F[d];
                        p.F[d] = 0;
                    }

                    keep_it = false;
                    auto const new_cell_index = W.get_cell_index(p);

                    if (new_cell_index != idx) {

                        for (std::size_t d = 0; d < DIM; ++d) {

                            if (p.x[d] < 0) {
                                if (W.lower_border[d] == periodic)
                                    p.x[d] += W.total_length[d];
                                else if (W.lower_border[d] == unknown)
                                    keep_it = true;
                                else {
                                    keep_it = false;
                                    break;
                                }
                            } else if (p.x[d] > W.total_length[d]) {
                                if (W.upper_border[d] == periodic)
                                    p.x[d] -= W.total_length[d];
                                else if (W.upper_border[d] == unknown)
                                    keep_it = true;
                                else {
                                    keep_it = false;
                                    break;
                                }
                            }
                        }

                        if (!keep_it) {
                            W.cells[new_cell_index].particles.emplace_back(p);

                            c.particles.erase(c.particles.begin() + p_id);
                            p_id--;
                        }
                    }
                }
    );
}

// vim:set et sts=4 ts=4 sw=4 ai ci cin:
