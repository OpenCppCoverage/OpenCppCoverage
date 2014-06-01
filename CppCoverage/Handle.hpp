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

#pragma once

namespace CppCoverage
{
	template<typename T_Handle, typename T_Releaser>
	class Handle
	{
	public:
		Handle(T_Handle handle, T_Releaser releaser);
		Handle(Handle&& handle);

		~Handle();

		const T_Handle& GetValue() const;

	private:
		Handle(const Handle&) = delete;
		Handle& operator=(const Handle&) = delete;

	private:
		T_Handle handle_;
		T_Releaser releaser_;
	};

	template<typename T_Handle, typename T_Releaser>
	Handle<T_Handle, T_Releaser> CreateHandle(T_Handle handle, T_Releaser releaser);	
}

#include "Handle.inl"


