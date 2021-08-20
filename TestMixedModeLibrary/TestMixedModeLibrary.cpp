// OpenCppCoverage is an open source code coverage for C++.
// Copyright (C) 2017 OpenCppCoverage
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

#pragma unmanaged
int NativeCountVowels(wchar_t* pString)
{
	int count = 0;
	const wchar_t* vowels = L"aeiouAEIOU";
	while (*pString)
	{
		if (wcschr(vowels, *pString++))
			count++;
	}
	return count;
}
#pragma managed
using namespace System;

namespace TestMixedModeLibrary
{
	public ref class Mixed
	{
	public: 
		int CountVowels(String^ subject)
		{
			pin_ptr<Char> p =
			const_cast<interior_ptr<Char>> (
				PtrToStringChars(subject));
			return NativeCountVowels(p);
		};
	};
}
