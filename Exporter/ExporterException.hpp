#ifndef EXPORTER_EXPORTEREXCEPTION_HEADER_GARD
#define EXPORTER_EXPORTEREXCEPTION_HEADER_GARD

namespace Exporter
{
	class ExporterException
	{
	public:
		ExporterException() = default;
	private:
		ExporterException(const ExporterException&) = delete;
		ExporterException& operator=(const ExporterException&) = delete;
	};
}

#endif
