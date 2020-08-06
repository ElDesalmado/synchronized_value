#pragma once

#include <mutex>

struct Dummy
{
    int64_t id;
    bool b;
};

// false if check is successful
bool run_check(Dummy& d, uint64_t assign, size_t intervals)
{
    d.id = assign;

    for (size_t i = 0; i != intervals; ++i)
        ++d.id;

    return bool(d.id - assign - intervals);
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