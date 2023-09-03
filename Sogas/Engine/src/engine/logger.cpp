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
    static u8   reset_color           = 7;
    static u8   levels[6]             = {64, 4, 6, 2, reset_color, 8};
    const char* level_type_strings[6] = {"[FATAL]: ", "[ERROR]: ", "[WARN]: ", "[INFO]: ", "[DEBUG]: ", "[TRACE]: "};

    auto level_index = static_cast<u8>(type);

    char* buffer = (char*)malloc(platform::char_buffer_size);

    if (!buffer)
        return;

    memset(buffer, 0, platform::char_buffer_size);

    va_list argptr;
    va_start(argptr, message);
    const auto length = platform::string_format(buffer, message, argptr);
    UNUSED(length); // TODO Check if that can be removed in release build.
    va_end(argptr);

    ASSERT(length >= 0); // In case we want to print an empty line.

    char       whole_content[platform::char_buffer_size];
    const auto number_written = platform::string_format(whole_content, "%s%s\n", level_type_strings[level_index], buffer);

    ASSERT(number_written > 0);

    free(buffer);

    SetConsoleTextAttribute(handle, levels[level_index]);
    WriteConsoleA(handle, &whole_content, number_written, nullptr, 0);

    // Reset to default colour.
    SetConsoleTextAttribute(handle, reset_color);
}
} // namespace log
} // namespace engine
} // namespace sogas