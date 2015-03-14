#pragma once

namespace VSPackageUnManagedWrapper
{
	public ref class VSPackageException : System::Exception
	{
	public:
		VSPackageException(System::String^ message);
	};
}

