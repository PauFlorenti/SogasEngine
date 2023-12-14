#include <defines.h>
#include <logger.h>

namespace sogas
{
namespace log
{

static i32 string_format(char* dest, const char* format, va_list va_args)
{
    if (dest != nullptr)
    {
        char       buffer[char_buffer_size];
        const auto number_written =
          _vsnprintf_s(buffer, sizeof(buffer), (size_t)char_buffer_size, format, va_args);
        memcpy(dest, buffer, static_cast<u64>(number_written) + 1);
        return number_written;
    }

    return -1;
}

static i32 string_format(char* dest, const char* format, ...)
{
    if (dest != nullptr)
    {
        va_list argptr;
        va_start(argptr, format);
        const auto number_written = string_format(dest, format, argptr);
        va_end(argptr);
        return number_written;
    }

    return -1;
}

void output(Logger_types type, const char* message, ...)
{
    // TODO: This is only supported in windows right now.
    // Writing to console should be handled by platform specific code. See platform folder.

    // TODO: Add the log type at the beginning of the sentence. Eg. [ERROR] ...

    // Windows only code ...
    auto handle = GetStdHandle(STD_OUTPUT_HANDLE);

    // FATAL, ERROR, WARNING, INFO, DEBUG, TRACE
    static u8   reset_color = 7;
    static u8   levels[6]   = {64, 4, 6, 2, reset_color, 8};
    const char* level_type_strings[6] =
      {"[FATAL]: ", "[ERROR]: ", "[WARN]: ", "[INFO]: ", "[DEBUG]: ", "[TRACE]: "};

    auto level_index = static_cast<u8>(type);

    char* buffer = (char*)malloc(char_buffer_size);

    if (!buffer)
        return;

    memset(buffer, 0, char_buffer_size);

    va_list argptr;
    va_start(argptr, message);
    const auto length = string_format(buffer, message, argptr);
    UNUSED(length); // TODO Check if that can be removed in release build.
    va_end(argptr);

    ASSERT(length >= 0); // In case we want to print an empty line.

    char       whole_content[char_buffer_size];
    const auto number_written =
      string_format(whole_content, "%s%s\n", level_type_strings[level_index], buffer);

    ASSERT(number_written > 0);

    free(buffer);

    SetConsoleTextAttribute(handle, levels[level_index]);
    WriteConsoleA(handle, &whole_content, number_written, nullptr, 0);

    // Reset to default colour.
    SetConsoleTextAttribute(handle, reset_color);
}
} // namespace log
} // namespace sogas
