
#include "synchronized_value/synchronized_value.h"

#include "run_check.h"

#include <future>
#include <random>

#include <string>
#include <iostream>

template <typename T1, typename T2>
constexpr bool is_same_v = std::is_same<T1, T2>::value;

int main(int argc, const char **argv)
{
    size_t intervals = 10000;

    if (argc == 2)
    {
        try
        {
            intervals = std::stoll(argv[1]);
        }
        catch (const std::invalid_argument &)
        {
            std::cerr << "Invalid argument for intervals: using " << intervals <<
                      " intervals fot test" << std::endl;
        }
    }

    static_assert(is_aggregate_constructable<Dummy, uint64_t, bool>(), "Invalid result");
    static_assert(is_aggregate_constructable<Dummy, int, bool>(), "Invalid result");
    static_assert(is_aggregate_constructable<Dummy, int, int>(), "Invalid result");
    static_assert(!is_aggregate_constructable<Dummy, double, int>(), "Invalid result");

    static_assert(is_same_v<typename xlock<Dummy>::value_type,
            Dummy>, "Invalid result");
    static_assert(is_same_v<typename xlock<const Dummy>::value_type,
            const Dummy>, "Invalid result");

    std::mutex mutex;
    Dummy d{1};
    {
        xlock<Dummy> xlock1{d, mutex};
        xlock1->id;
        Dummy dummy = *xlock1;
        Dummy& refDummy = *xlock1;
        const Dummy& crefDummy = *xlock1;
    }
    {
        xlock<const Dummy> xlock_const{d, mutex};
        xlock_const->id;

        Dummy dummy = *xlock_const;
        // Dummy& refDummy = *xlock_const;
        const Dummy& crefDummy = *xlock_const;
    }
    {
        const xlock<const Dummy> xlock_const{d, mutex};
        xlock_const->id;
    }
    {
        synchronized_value<Dummy> synchValue{d};
        synchValue->id;
        Dummy dummy = *synchValue;
        Dummy& refDummy = *synchValue;
        const Dummy& crefDummy = *synchValue;
    }
    {
        /* this does not compile
        const synchronized_value<Dummy> synchValueConst =
                make_synch_value<Dummy>(4, true);
                */
        const synchronized_value<Dummy> synchValueConst2{d};
        synchValueConst2->id;
        Dummy dummy = *synchValueConst2;
        // Dummy& refDummy = *synchValueConst2;
        const Dummy& crefDummy = *synchValueConst2;
    }

    synchronized_value<Dummy> dummy{{16}};

    bool first_check = run_check(*dummy, 42, 1000);

    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_int_distribution<int64_t> dist(-69800, 385697);

    std::vector<std::future<bool>> results;

    for (size_t i = 0; i != 1000; ++i)
    {
        results.push_back(std::async(std::launch::async,
                &run_check,
                std::ref((Dummy&)*dummy),
                dist(rng),
                intervals));
    }

    int result = 0;
    for (size_t i = 0; i != 1000; ++i)
        result += (int)results[i].get();

    return result;
}