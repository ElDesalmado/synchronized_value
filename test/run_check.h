#pragma once

#include "synchronized_value/synchronized_value.h"

#include <iostream>
#include <thread>

struct Dummy
{
    int64_t id;
    bool b;
};

struct Wrapped
{
    synchronized_value<Dummy>& sv;
};

// false if check is successful
bool run_check(Dummy& d, int64_t assign, size_t intervals)
{
    d.id = assign;

    // std::cout << "run_check " << assign << " thread: " <<
       //       std::this_thread::get_id() << '\n';
    for (size_t i = 0; i != intervals; ++i)
        ++d.id;
    // std::cout << "end_check " << assign << " thread: " <<
       //       std::this_thread::get_id() << '\n' << std::endl;

    return bool(d.id - assign - intervals);
}

bool run_check_w(Wrapped& w, int64_t assign, size_t intervals)
{
    return run_check(*w.sv, assign, intervals);
}

template <typename Guard>
bool check_mutex(Dummy& d, std::mutex& mutex, uint64_t assign, size_t intervals)
{
    Guard guard{mutex};
    d.id = assign;

    for (size_t i = 0; i != intervals; ++i)
        ++d.id;

    return bool(d.id - assign - intervals);
}