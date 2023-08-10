
#pragma once

namespace sogas
{
namespace engine
{
namespace log
{
enum class Logger_types
{
    DEBUG, // Grey
    WARNING, // Yellow
    ERROR, // Red
    FATAL // Redder
};

void output(Logger_types type, const char* message, ...);

}
} // namespace engine
} // namespace sogas