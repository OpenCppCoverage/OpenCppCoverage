#pragma once 

namespace VSPackageUnManagedWrapper
{
	public interface class ILogger
	{
	public:
		bool WriteLine(System::String^ message);
	};
}
