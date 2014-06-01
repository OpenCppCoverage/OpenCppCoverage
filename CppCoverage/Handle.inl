// OpenCppCoverage is an open source code coverage for C++.
// Copyright (C) 2014 OpenCppCoverage
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

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
