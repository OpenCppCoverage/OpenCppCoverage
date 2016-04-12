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
#include "ScopedAction.hpp"

#include <boost/optional/optional.hpp>

#include "Tool.hpp"

#include "Log.hpp"

namespace Tools
{
	//-------------------------------------------------------------------------
	ScopedAction::ScopedAction(std::function<void()> action)
		: action_(action)
	{
	}

	//-------------------------------------------------------------------------
	ScopedAction::~ScopedAction()
	{
		auto error = Try([&]
		{
			action_();
		});

		if (error)
			LOG_ERROR << *error;
	}

}
