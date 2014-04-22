#pragma once

#ifdef TOOLS_EXPORTS
#define TOOLS_DLL __declspec(dllexport)
#else
#define TOOLS_DLL _declspec(dllimport)
#endif

#pragma warning (disable: 4251)
