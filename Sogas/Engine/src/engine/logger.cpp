#include "pch.hpp"

#include <engine/logger.h>

namespace sogas
{
namespace engine
{
namespace log
{
void output(Logger_types type, const char* message, ...)
{
    // TODO: This is only supported in windows right now.
    // Writing to console should be handled by platform specific code. See platform folder.

    // TODO: Add the log type at the beginning of the sentence. Eg. [ERROR] ...

    // Windows only code ...
    auto handle = GetStdHandle(STD_OUTPUT_HANDLE);

    // FATAL, ERROR, WARNING, INFO, DEBUG, TRACE
    static u8 reset_color = 7;
    static u8 levels[6]   = {64, 4, 6, 2, reset_color, 8};

    SetConsoleTextAttribute(handle, levels[static_cast<u8>(type)]);

    va_list argptr;
    va_start(argptr, message);
    char dest[1024 * 8];
    int  n      = _vsnprintf_s(dest, sizeof(dest), message, argptr);
    dest[n]     = '\n';
    va_end(argptr);
    WriteConsoleA(handle, &dest, n + 1, nullptr, 0);

    // Reset to default colour.
    SetConsoleTextAttribute(handle, reset_color);
}
} // namespace log
} // namespace engine
} // namespace sogas