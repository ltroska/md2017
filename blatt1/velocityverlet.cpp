#include "velocityverlet.hpp"

VelocityVerlet::VelocityVerlet(World& _W, Potential& _Pot, Observer& _O) : TimeDiscretization(_W,_Pot,_O)
{
    // empty constructor
}

VelocityVerlet::VelocityVerlet(World& _W, Potential* _Pot, Observer& _O) : TimeDiscretization(_W,(*_Pot),_O)
{
    // empty constructor
}

void VelocityVerlet::simulate()
{
    // while simulation end time not reached
    comp_F();
    while (W.t < W.t_end) {
        timestep(W.delta_t);
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

void VelocityVerlet::comp_F()
{
    W.e_pot = 0;
    for (std::size_t idx = 0; idx < W.particles.size(); ++idx) {
        auto& p = W.particles[idx];
        for (std::size_t d = 0; d < DIM; ++d) {
            p.F[d] = 0;
        }

        for (std::size_t idy = 0; idy < W.particles.size(); ++idy) {
            auto& q = W.particles[idy];
            if (idx != idy) {
                W.e_pot += Pot.force(p, q);
            }
        }
    }
}

void VelocityVerlet::update_V()
{
    W.e_kin = 0;
    for (auto &p : W.particles) {

        for (std::size_t d = 0; d < DIM; ++d) {
            p.v[d] = p.v[d] + W.delta_t * .5 / p.m * (p.F_old[d] + p.F[d]);
        }

        W.e_kin += 0.5 * p.m * (p.v[0] * p.v[0] + p.v[1] * p.v[1]);
    }
}

void VelocityVerlet::update_X()
{
    for (auto &p : W.particles) {

        for (std::size_t d = 0; d < DIM; ++d) {
            p.x[d] = p.x[d] + W.delta_t * (p.v[d] + .5 / p.m * p.F[d] * W.delta_t);
            p.F_old[d] = p.F[d];
        }
    }
}

// vim:set et sts=4 ts=4 sw=4 ai ci cin:
