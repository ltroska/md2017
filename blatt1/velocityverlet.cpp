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
    while (W.t < W.t_end)
    {
        timestep(W.delta_t);
    }
}

void VelocityVerlet::timestep(real delta_t)
{
    // test output
    std::cout << "timestep()" << std::endl;

    // TODO
    update_X();
    comp_F();
    update_V();

    // increase time
    W.t += delta_t;

    // notify observer
    O.notify();
}

void VelocityVerlet::comp_F()
{
    for (auto &p : W.particles) {
        for (auto &q : W.particles) {
            if (p.id != q.id) {
                W.e_pot += Pot.force(p, q);
            }
        }
    }
}

void VelocityVerlet::update_V()
{
 
    for (auto &p : W.particles) {
        p.v[0] += (p.F_old[0] + p.F[0]) * W.delta_t / (2 * p.m);
        p.v[1] += (p.F_old[1] + p.F[1]) * W.delta_t / (2 * p.m);
        W.e_kin += 0.5 * p.m * (p.v[0] * p.v[0] + p.v[1] * p.v[1]);

        
    }
    
}

void VelocityVerlet::update_X()
{
    for (auto &p : W.particles) {
        p.x[0] += W.delta_t * p.v[0] + p.F_old[0] * W.delta_t * W.delta_t / (2 * p.m);
        p.x[1] += W.delta_t * p.v[1] + p.F_old[1] * W.delta_t * W.delta_t / (2 * p.m);

        std::swap(p.F_old[0], p.F[0]);
        std::swap(p.F_old[1], p.F[1]);
    }
}

// vim:set et sts=4 ts=4 sw=4 ai ci cin:
