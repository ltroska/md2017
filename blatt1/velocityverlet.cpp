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
    for (std::vector<real>::iterator i = W.particles.begin(); i != W.particles.end(); ++i) {
        for (std::vector<real>::iterator j = W.particles.begin(); j != W.particles.end(); ++j){
            if (i != j) {
                Pot.force(*i,*j);
                Pot.force(*j,*i);
            }
        }
    }
}

void VelocityVerlet::update_V()
{
 
    for (auto &p : W.particles) {
        p.v[0] += (p.F_old[0] + p.F[0]) * W.delta_t / (2 * p.m);
        p.v[1] += (p.F_old[1] + p.F[1]) * W.delta_t / (2 * p.m);
    }
    
}

void VelocityVerlet::update_X()
{
    for (auto &p : W.particles) {
        p.x[0] += W.delta_t * p.v[0] + p.F_old[0] * W.delta_t * W.delta_t / (2 * p.m);
        p.x[1] += W.delta_t * p.v[1] + p.F_old[1] * W.delta_t * W.delta_t / (2 * p.m);
    }
}

// vim:set et sts=4 ts=4 sw=4 ai ci cin:
