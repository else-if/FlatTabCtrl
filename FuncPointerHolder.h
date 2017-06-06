#pragma once
#include "TestClass.h"

class CFuncPointerHolder
{
public:

	typedef BOOL(*someFunc)(TestClassA*& a);

	someFunc f;
	
	CFuncPointerHolder();

	void InvokeFunc()
	{
		TestClassA *a = NULL;
		BOOL res = f(a);
	}
};

