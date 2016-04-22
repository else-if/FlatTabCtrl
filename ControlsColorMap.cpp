#pragma once

#include "stdafx.h"
#include "ControlsColorMap.h"

ControlsColorMap::ControlsColorMap()
{
	SetDefaultColors();
}

ControlsColorMap::~ControlsColorMap()
{	
}

COLORREF ControlsColorMap::GetColor(ControlState state, ColorArea area)
{
	return m_ColorMap[state][area];
}

void ControlsColorMap::SetColor(ControlState state, ColorArea area, COLORREF color)
{
	m_ColorMap[state][area] = color;
}

void ControlsColorMap::SetDefaultColors()
{
	m_ColorMap[Normal][Border] = RGB(153, 152, 158);
	m_ColorMap[Normal][BorderLight] = RGB(255, 255, 255);
	m_ColorMap[Normal][BackgroundTopGradientStart] = RGB(255, 255, 255);
	m_ColorMap[Normal][BackgroundTopGradientFinish] = RGB(255, 255, 255);
	m_ColorMap[Normal][BackgroundBottomGradientStart] = RGB(237, 239, 242);
	m_ColorMap[Normal][BackgroundBottomGradientFinish] = RGB(255, 255, 255);

	m_ColorMap[Press][Border] = RGB(211, 132, 56);
	m_ColorMap[Press][BorderLight] = RGB(237, 158, 70);
	m_ColorMap[Press][BackgroundTopGradientStart] = RGB(255, 205, 82);
	m_ColorMap[Press][BackgroundTopGradientFinish] = RGB(255, 205, 82);
	m_ColorMap[Press][BackgroundBottomGradientStart] = RGB(255, 205, 82);
	m_ColorMap[Press][BackgroundBottomGradientFinish] = RGB(255, 205, 82);

	m_ColorMap[Mouseover][Border] = RGB(216, 139, 62);
	m_ColorMap[Mouseover][BorderLight] = RGB(254, 251, 205);
	m_ColorMap[Mouseover][BackgroundTopGradientStart] = RGB(255, 214, 100);
	m_ColorMap[Mouseover][BackgroundTopGradientFinish] = RGB(255, 225, 144);
	m_ColorMap[Mouseover][BackgroundBottomGradientStart] = RGB(254, 251, 205);
	m_ColorMap[Mouseover][BackgroundBottomGradientFinish] = RGB(255, 214, 100);

	m_ColorMap[Focus][Border] = RGB(193, 192, 200);
	m_ColorMap[Focus][BorderLight] = RGB(252, 204, 86);
	m_ColorMap[Focus][BackgroundTopGradientStart] = RGB(255, 255, 255);
	m_ColorMap[Focus][BackgroundTopGradientFinish] = RGB(255, 255, 255);
	m_ColorMap[Focus][BackgroundBottomGradientStart] = RGB(237, 239, 242);
	m_ColorMap[Focus][BackgroundBottomGradientFinish] = RGB(255, 255, 255);

	m_ColorMap[Disable][Border] = RGB(217, 217, 217);
	m_ColorMap[Disable][BorderLight] = RGB(239, 239, 239);
	m_ColorMap[Disable][BackgroundTopGradientStart] = RGB(239, 239, 239);
	m_ColorMap[Disable][BackgroundTopGradientFinish] = RGB(239, 239, 239);
	m_ColorMap[Disable][BackgroundBottomGradientStart] = RGB(239, 239, 239);
	m_ColorMap[Disable][BackgroundBottomGradientFinish] = RGB(239, 239, 239);
}

void ControlsColorMap::AddToColorMap(int state, int area, COLORREF color)
{
	m_ColorMap[state][area] = color;
}

COLORREF ControlsColorMap::GetFromColorMap(int state, int area)
{
	return m_ColorMap[state][area];
}