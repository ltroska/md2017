#ifndef _LOGGING_HPP
#define _LOGGING_HPP
#include <iostream>

#ifdef NDEBUG
    #define LOG_CHECK false &&
#else
    #define LOG_CHECK
#endif

#define GET_MACRO(_1,_2,NAME,...) NAME

#define LOG1(_msg) std::cout << _msg << std::endl

#define LOG2(_rank, _msg) std::cout << "Rank " << _rank << ": " << _msg << std::endl

#define LOG(...) GET_MACRO(__VA_ARGS__, LOG2, LOG1)(__VA_ARGS__)


#define LOG_DBG(_msg) LOG_CHECK LOG(_msg)

#define LOG_DBG_RANK(_rank, _msg) LOG_CHECK LOG(_rank, _msg)

#define LOG_DBG_ROOT(_rank, _msg) LOG_CHECK (_rank == 0 ? !(LOG(_rank, _msg)).fail() : false)

#endif
