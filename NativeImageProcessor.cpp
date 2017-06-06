#include "stdafx.h"
#include "NativeImageProcessor.h"


NativeImageProcessor::NativeImageProcessor(gcroot<ManagedImageProcessor^> mip)
{
	prc = mip;

	m_LogoBitmap = new CImage();
	m_LogoBitmap->Attach((HBITMAP)((gcnew System::Drawing::Bitmap(prc->managedImage))->GetHbitmap().ToPointer()));
	m_LogoBitmap->SetHasAlphaChannel(System::Drawing::Image::IsAlphaPixelFormat(prc->managedImage->PixelFormat));
	m_LogoBitmap->Save(_T("H:\\test.png"), Gdiplus::ImageFormatPNG);
}


NativeImageProcessor::~NativeImageProcessor()
{
	if (m_LogoBitmap)
	{
		m_LogoBitmap->Detach();
		delete m_LogoBitmap;
		m_LogoBitmap = NULL;
	}
}
