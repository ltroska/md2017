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

    // increase time
    W.t += delta_t;

    // notify observer
    O.notify();
}

void VelocityVerlet::comp_F()
{
    // TODO
}

void VelocityVerlet::update_V()
{
    // TODO
}

void VelocityVerlet::update_X()
{
    // TODO
}

// vim:set et sts=4 ts=4 sw=4 ai ci cin:
