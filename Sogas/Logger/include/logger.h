
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

#define PFATAL(message, ...) output(sogas::log::Logger_types::FATAL, message, ##__VA_ARGS__);
#define PERROR(message, ...) output(sogas::log::Logger_types::ERROR, message, ##__VA_ARGS__);
#define PWARN(message, ...) output(sogas::log::Logger_types::WARNING, message, ##__VA_ARGS__);
#define PINFO(message, ...) output(sogas::log::Logger_types::INFO, message, ##__VA_ARGS__);
#define PDEBUG(message, ...) output(sogas::log::Logger_types::DEBUG, message, ##__VA_ARGS__);

} // namespace log
} // namespace sogas
