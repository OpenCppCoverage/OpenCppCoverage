#ifndef CPPCOVERAGE_EXPORT_HEADER_GARD
#define CPPCOVERAGE_EXPORT_HEADER_GARD

#ifdef CPPCOVERAGE_EXPORTS
	#define CPPCOVERAGE_DLL __declspec(dllexport)
#else
	#define CPPCOVERAGE_DLL _declspec(dllimport)
#endif

#pragma warning (disable: 4251)

#endif

