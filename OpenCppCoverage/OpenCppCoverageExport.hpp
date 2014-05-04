#pragma once

#ifdef OPEN_CPP_COVERAGE_EXPORTS
#define OPEN_CPP_COVERAGE_DLL __declspec(dllexport)
#else
#define OPEN_CPP_COVERAGE_DLL _declspec(dllimport)
#endif

#pragma warning (disable: 4251)