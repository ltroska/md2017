#include "timediscretization.hpp"

TimeDiscretization::TimeDiscretization(SubDomain& _S, Potential& _Pot, Observer &_O) : W(_S), Pot(_Pot), O(_O)
{
}

// vim:set et sts=4 ts=4 sw=4 ai ci cin:
