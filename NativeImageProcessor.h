#pragma once

#include "ManagedImageProcessor.h"

class NativeImageProcessor
{
public:
	NativeImageProcessor(gcroot<ManagedImageProcessor^> mip);
	~NativeImageProcessor();

	CImage*		m_LogoBitmap;
	
	gcroot<ManagedImageProcessor^> prc;
};

