//
// pch.h
//

#pragma once

#ifdef _WIN64
#pragma warning (disable : 4615)
#pragma warning (disable : 4389)
#include <gtest/gtest.h>
#pragma warning (enable : 4389)
#pragma warning (enable : 4615)
#else
#error "Only win64 platform implemented at the moment."
#endif

#include <engine/defines.h>
