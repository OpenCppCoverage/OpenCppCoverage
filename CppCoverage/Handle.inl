#include "stdafx.h"

#include "tools/Log.hpp"
#include "tools/Tool.hpp"

#include "Handle.hpp"
#include "CppCoverageException.hpp"


namespace CppCoverage
{
	//-------------------------------------------------------------------------
	template<typename T_Handle, typename T_Releaser>
	Handle<T_Handle, T_Releaser>::Handle(T_Handle handle, T_Releaser releaser)
			: handle_(handle)
			, releaser_(releaser)
		{
			if (!handle_)
				THROW(L"Handle is not valid");
		}

	//-------------------------------------------------------------------------
	template<typename T_Handle, typename T_Releaser>
	Handle<T_Handle, T_Releaser>::Handle(Handle&& handle)
			: handle_()
			, releaser_()
		{
			std::swap(handle_, handle.handle_);
			std::swap(releaser_, handle.releaser_);
		}

	//----------------------------------------------------------------------------
	template<typename T_Handle, typename T_Releaser>
	Handle<T_Handle, T_Releaser>::~Handle()
	{
		Tools::Try([&]
		{
			if (handle_ && !releaser_(handle_))
			{
				LOG_ERROR << "Cannot release handler";
			}
		});		
	}

	//----------------------------------------------------------------------------
	template<typename T_Handle, typename T_Releaser>
	const T_Handle& Handle<T_Handle, T_Releaser>::GetValue() const
	{
		return handle_;
	}

	//----------------------------------------------------------------------------
	template<typename T_Handle, typename T_Releaser>
	Handle<T_Handle, T_Releaser> CreateHandle(T_Handle handle, T_Releaser releaser)
	{
		return Handle<T_Handle, T_Releaser>(handle, releaser);
	}
}
