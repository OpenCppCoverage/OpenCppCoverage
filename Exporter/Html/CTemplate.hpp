// OpenCppCoverage is an open source code coverage for C++.
// Copyright (C) 2014 OpenCppCoverage
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#pragma once 

#pragma warning(push)

#pragma warning(disable: 4005) // 'va_copy' : macro redefinition
#pragma warning(disable: 4251) // needs to have dll-interface to be used by clients of class
#pragma warning(disable: 4273) // inconsistent dll linkage
#pragma warning(disable: 4267) // 'initializing' : conversion from 'size_t' to 'int'

template std::vector<std::string>; // To avoid error C4251
#define _SILENCE_STDEXT_HASH_DEPRECATION_WARNINGS
#define __MINGW64__ // disable snprintf
#include <ctemplate/template.h>
#include <tests/template_test_util.h>
#undef _SILENCE_STDEXT_HASH_DEPRECATION_WARNING
#undef __MINGW64__

#pragma warning(pop)
