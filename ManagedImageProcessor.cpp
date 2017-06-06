#include "stdafx.h"
#include "ManagedImageProcessor.h"


ManagedImageProcessor::ManagedImageProcessor()
{
	managedImage = System::Drawing::Bitmap::FromFile("H:\\managed.png");
}
