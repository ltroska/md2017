#include "velocityverlet.hpp"

extern int neighbor_offsets[NUM_NEIGHBORS][DIM];

VelocityVerlet::VelocityVerlet(World& _W, Potential& _Pot, Observer& _O) : TimeDiscretization(_W,_Pot,_O)
{
    // empty constructor
}

VelocityVerlet::VelocityVerlet(World& _W, Potential* _Pot, Observer& _O) : TimeDiscretization(_W,(*_Pot),_O)
{
}

void VelocityVerlet::simulate()
{
    // while simulation end time not reached
    for (auto& c : W.cells)
        for (auto& p : c.particles)
            for (std::size_t d = 0; d < DIM; ++d)
                p.F[d] = 0;

    comp_F();
    std::size_t iter = 0;

    real percentage = 0.05;
    std::size_t total_iters = std::ceil(W.t_end / W.delta_t);
    std::size_t mod = std::floor(total_iters * percentage);

    while (W.t < W.t_end) {
        timestep(W.delta_t);
        if (iter % mod == 0)
            std::cout << "Iteration " << iter << "/" << total_iters << " (" <<  iter/(double)total_iters * 100 << "%) done"
                      << std::endl;
        ++iter;
    }
}

void VelocityVerlet::timestep(real delta_t)
{
    W.t += delta_t;

    update_X();
    comp_F();
    update_V();

    // notify observer
    O.notify();
}

void VelocityVerlet::comp_F() {
    W.e_pot = 0;
    std::size_t neighbor_cell_index[DIM] = {0};
    real distance_sqr = 0;
    real difference_offset[DIM] = {0};
    bool skip_neighbor = 0;
    for (auto &c : W.cells) {
        for (auto &p : c.particles) {
            for (std::size_t n = 0; n < NUM_NEIGHBORS; ++n) {
                skip_neighbor = false;

                //get neighbor index
                for (std::size_t d = 0; d < DIM; ++d) {
                    auto const offset = neighbor_offsets[n][d];

                    if (c.index[d] == 0 && offset == -1) {
                        if (W.lower_border[d] == periodic)
                            neighbor_cell_index[d] = W.n_cells[d] - 1;
                        else
                            skip_neighbor = true;
                    } else if (c.index[d] == W.n_cells[d] - 1 && offset == 1) {
                        if (W.upper_border[d] == periodic)
                            neighbor_cell_index[d] = 0;
                        else
                            skip_neighbor = true;
                    } else {
                        neighbor_cell_index[d] = c.index[d] + neighbor_offsets[n][d];
                    }
                }

                if (skip_neighbor)
                    continue;


                auto const neighbor_cell_linear_index = W.get_linear_index(neighbor_cell_index);

                // compute force
                for (auto &q : W.cells[neighbor_cell_linear_index].particles) {
                    if (p.id != q.id) {
                        distance_sqr = 0;

                        for (std::size_t d = 0; d < DIM; ++d) {
                            if (c.index[d] == 0 && neighbor_cell_index[d] == W.n_cells[d] - 1 &&
                                W.lower_border[d] == periodic) {
                                distance_sqr += sqr(p.x[d] + W.length[d] - q.x[d]);
                                difference_offset[d] = W.length[d];
                            } else if (neighbor_cell_index[d] == 0 && c.index[d] == W.n_cells[d] - 1 &&
                                       W.upper_border[d] == periodic) {
                                distance_sqr += sqr(p.x[d] - W.length[d] - q.x[d]);
                                difference_offset[d] = -W.length[d];
                            } else {
                                distance_sqr += sqr(p.x[d] - q.x[d]);
                                difference_offset[d] = 0;
                            }
                        }

                        W.e_pot += Pot.force(p, q, W.cell_r_cut_sq, difference_offset);
                    }
                }
            }
        }
    }
}


void VelocityVerlet::update_V()
{
    W.e_kin = 0;
    real tmp;
    for (auto &c : W.cells) {
        for (auto &p : c.particles) {
            tmp = 0;

            for (std::size_t d = 0; d < DIM; ++d) {
                p.v[d] = p.v[d] + W.delta_t * .5 / p.m * (p.F_old[d] + p.F[d]);
                tmp += p.v[d] * p.v[d];
            }

            W.e_kin += 0.5 * p.m * tmp;
        }
    }
}

void VelocityVerlet::update_X()
{
    for (std::size_t idx = 0; idx < W.cells.size(); ++idx) {
        auto& c = W.cells[idx];

        for (std::size_t p_id = 0; p_id < c.particles.size(); ++p_id) {
            auto& p = c.particles[p_id];
            // update x
            for (std::size_t d = 0; d < DIM; ++d) {
                p.x[d] = p.x[d] + W.delta_t * (p.v[d] + .5 / p.m * p.F[d] * W.delta_t);

                if (p.x[d] < 0 && W.lower_border[d] == periodic) {
                    p.x[d] += W.length[d];
                }
                else if (p.x[d] > W.length[d] && W.upper_border[d] == periodic) {
                    p.x[d] -= W.length[d];
                 }

                p.F_old[d] = p.F[d];
                p.F[d] = 0;
            }

            auto const new_cell_index = W.get_cell_index(p);

            // particle left with bordertype leaving
            if (new_cell_index >= W.n_total_cells) {
                c.particles.erase(c.particles.begin() + p_id);
                p_id--;
            }
            // move particle to new cell
            else if (new_cell_index != idx) {
                W.cells[new_cell_index].particles.emplace_back(p);

                c.particles.erase(c.particles.begin() + p_id);
                p_id--;
            }
        }
    }
}

// vim:set et sts=4 ts=4 sw=4 ai ci cin:
