#pragma once

#include <functional>

#include "ToolsExport.hpp"

namespace Tools
{
	class TOOLS_DLL ScopedAction
	{
	public:
		explicit ScopedAction(std::function<void ()>);
		~ScopedAction();

	private:
		ScopedAction(const ScopedAction&) = delete;
		ScopedAction& operator=(const ScopedAction&) = delete;

	private:
		std::function<void()> action_;
	};
}
