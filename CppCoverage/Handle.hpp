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


