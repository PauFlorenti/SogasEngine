
#ifndef PCH_HPP
#define PCH_HPP

// C++ includes
#include <cassert>
#include <map>
#include <memory>
#include <stdint.h>
#include <string>
#include <vector>

#ifdef _WIN64
// Window includes
// Reduce the amount of symbols/headers when we include the windows.h
#define WIN32_LEAN_AND_MEAN
// Windows defines a macro min/max as define that interfieres with std::min/max
#define NOMINMAX
// Other macros to remove windows stuff
#define NODRAWTEXT
#define NOGDI
#define NOBITMAP
#define NOMCX
#define NOSERVICE
#define NOHELP
#include <Windows.h>
#endif

// engine includes
#include <engine/defines.h>

#endif //PCH_HPP
