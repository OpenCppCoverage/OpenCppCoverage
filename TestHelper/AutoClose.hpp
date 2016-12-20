// OpenCppCoverage is an open source code coverage for C++.
// Copyright (C) 2016 OpenCppCoverage
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

namespace TestHelper
{
	//-------------------------------------------------------------------------
	template <typename Value, typename CloseAction>
	class AutoClose
	{
	public:
		//---------------------------------------------------------------------
		AutoClose(Value value, Value invalidValue, CloseAction closeAction)
			: value_{ value }
			, closeAction_{ closeAction }
		{
			if (value_ == invalidValue)
				throw std::runtime_error("The value is invalid");
		}

		//---------------------------------------------------------------------
		~AutoClose()
		{
			closeAction_(value_);
		}

		//---------------------------------------------------------------------
		Value operator*()
		{
			return value_;
		}

	private:
		Value value_;
		CloseAction closeAction_;
	};

	//-------------------------------------------------------------------------
	template <typename Value, typename InvalidValue, typename CloseAction>
	AutoClose<Value, CloseAction> MakeAutoClose(
		Value value, 
		InvalidValue invalidValue,
		CloseAction closeAction)
	{
		return AutoClose<Value, CloseAction>(value, invalidValue, closeAction);
	}
}