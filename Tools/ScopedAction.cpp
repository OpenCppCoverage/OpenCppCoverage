#include "stdafx.h"
#include "ScopedAction.hpp"
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
		Try([&]
		{
			action_();
		});
	}

}
