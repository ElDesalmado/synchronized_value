
#include "run_check.h"

#include <future>
#include <random>

#include <string>
#include <iostream>

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


    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_int_distribution<int64_t> dist(-1369898, 8956334);

    std::vector<std::future<bool>> results;
    std::mutex mutex;
    Dummy dummy{42, false};


    for (size_t i = 0; i != 1000; ++i)
    {
        results.push_back(std::async(std::launch::async,
                                     &check_mutex<std::lock_guard<std::mutex>>,
                                     std::ref(dummy),
                                     std::ref(mutex),
                                     dist(rng),
                                     intervals));
    }

    int result = 0;
    for (size_t i = 0; i != 1000; ++i)
        result += (int)results[i].get();

    return result;
}