#pragma once

namespace sogas
{
struct Clock
{
    f64 start_time{0};
    f64 elapsed_time{0};
};

void start_clock(Clock* clock);
void update_clock(Clock* clock);
void stop_clock(Clock* clock);
} // namespace sogas
