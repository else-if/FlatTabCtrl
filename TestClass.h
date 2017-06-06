#pragma once

public class TestClassA
{
public:
	TestClassA();

	int i;
	int j;
};

public class TestClassB : public TestClassA
{
public:
	TestClassB() : TestClassA()
	{
		p = -1;
	};

	int p;	
};




