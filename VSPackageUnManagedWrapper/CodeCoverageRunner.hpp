#pragma once

#include "ILogger.hpp"

namespace VSPackageUnManagedWrapper 
{
	public ref class CodeCoverageRunner
	{
	public:
		System::String^ Run(
			System::String^ command,
			System::String^ arguments,
			System::String^ workingDir,
			System::Collections::Generic::IEnumerable<System::String^>^ modulePaths,
			System::Collections::Generic::IEnumerable<System::String^>^ sourcePaths,
			ILogger^ logger);
	};
}
