#include "pch.hpp"

#include <engine/clock.h>

namespace
{
static LARGE_INTEGER initial_time;
static f64           frequency_timer;

void setup_timer()
{
    LARGE_INTEGER frequency; // counts / second
    QueryPerformanceFrequency(&frequency);
    frequency_timer = 1.0 / (f64)frequency.QuadPart;
    QueryPerformanceCounter(&initial_time); // counts
}

static const f64 get_current_time()
{
    // If no timer, start timer.
    if (!frequency_timer)
    {
        setup_timer();
    }

    LARGE_INTEGER now;
    QueryPerformanceCounter(&now);
    // return value in micro seconds
    return (f64)now.QuadPart * frequency_timer;
}
} // namespace

namespace sogas
{
void start_clock(Clock* clock)
{
    clock->start_time   = get_current_time();
    clock->elapsed_time = 0;
}

void update_clock(Clock* clock)
{
    if (clock->start_time != 0)
    {
        clock->elapsed_time = get_current_time() - clock->start_time;
    }
}

void stop_clock(Clock* clock)
{
    clock->start_time = 0;
}
} // namespace sogas
