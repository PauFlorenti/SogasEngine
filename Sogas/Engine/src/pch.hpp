
#ifndef PCH_HPP
#define PCH_HPP

// C++ includes
#include <array>
#include <bitset>
#include <cassert>
#include <fstream>
#include <map>
#include <memory>
#include <stdint.h>
#include <string>
#include <unordered_map>
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
#include <windowsx.h>
#endif

// Third party includes
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#pragma warning(disable : 4201)
#include <glm/ext/quaternion_float.hpp>
#pragma warning(default : 4201)
#include <glm/ext/quaternion_common.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/quaternion.hpp>
#include <nlohmann/json.hpp>

// own external includes.
#include <logger.h>

// engine includes
#include <engine/defines.h>
#include <engine/math.h>
#include <engine/smemory.h>
#include <handle/handle.h>
#include <platform/platform.h>

#endif //PCH_HPP
