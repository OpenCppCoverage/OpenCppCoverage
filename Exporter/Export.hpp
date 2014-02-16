#ifndef EXPORTER_EXPORT_HEADER_GARD
#define EXPORTER_EXPORT_HEADER_GARD

#ifdef EXPORTER_EXPORTS
#define EXPORTER_DLL __declspec(dllexport)
#else
#define EXPORTER_DLL _declspec(dllimport)
#endif

#pragma warning (disable: 4251)

#endif

