#include "Timer.hpp"

#include <chrono>

using namespace std;
using namespace std::chrono;

void Timer::Update()
{
    static auto start = high_resolution_clock::now();
    auto end = high_resolution_clock::now();
    auto duration = end - start;

    dt_ = duration_cast<milliseconds>(duration).count();
    dt_ /= 1000.f;

    all_t_ += dt_;

    start = end;
}