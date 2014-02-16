#include "stdafx.h"
#include <tchar.h>

int _tmain(int argc, _TCHAR* argv[])
{
	::testing::InitGoogleMock(&argc, argv);
	return RUN_ALL_TESTS();
}