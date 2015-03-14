#include "stdafx.h"
#include "VSPackageException.hpp"

namespace VSPackageUnManagedWrapper
{
	VSPackageException::VSPackageException(System::String^ message)
		: System::Exception(message)
	{
	}
}
