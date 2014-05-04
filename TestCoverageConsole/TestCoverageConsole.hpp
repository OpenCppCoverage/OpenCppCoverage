#include <string>

namespace boost
{
	namespace filesystem
	{
		class path;
	}
}

namespace TestCoverageConsole
{
	__declspec(dllexport) boost::filesystem::path GetMainCppPath();
	__declspec(dllexport) boost::filesystem::path GetOutputBinaryPath();

	const std::wstring TestThrowHandledException = L"TestThrowHandledException";
	const std::wstring TestThrowUnHandledException = L"TestThrowUnHandledException";
	const std::wstring TestSharedLib = L"TestSharedLib";
}
