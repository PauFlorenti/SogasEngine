
#pragma once

namespace sogas
{
namespace log
{
enum class Logger_types
{
    FATAL, // Redder
    ERROR, // Red
    WARNING, // Yellow
    INFO, // Green
    DEBUG // Grey
};

constexpr auto char_buffer_size = 1024 * 4;

void output(sogas::log::Logger_types type, const char* message, ...);

#define FATAL(message, ...) output(sogas::log::Logger_types::FATAL, message, ##__VA_ARGS__);
#define ERROR(message, ...) output(sogas::log::Logger_types::ERROR, message, ##__VA_ARGS__);
#define WARN(message, ...) output(sogas::log::Logger_types::WARNING, message, ##__VA_ARGS__);
#define INFO(message, ...) output(sogas::log::Logger_types::INFO, message, ##__VA_ARGS__);
#define DEBUG(message, ...) output(sogas::log::Logger_types::DEBUG, message, ##__VA_ARGS__);

} // namespace log
} // namespace sogas
