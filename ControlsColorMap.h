#pragma once

#include "stdafx.h"
#include <map>

using namespace std;

public enum ControlState
{
	Normal = 0,
	Press = 1,
	Focus = 2,
	Mouseover = 3,
	Disable = 4
};

public enum ColorArea
{
	Border = 0,
	BorderLight = 1,
	BackgroundTopGradientStart = 2,
	BackgroundTopGradientFinish = 3,
	BackgroundBottomGradientStart = 4,
	BackgroundBottomGradientFinish = 5
};

class ControlsColorMap
{
private:
	map<int, map<int, COLORREF>> m_ColorMap;

public:
	ControlsColorMap();
	virtual ~ControlsColorMap();

	void SetDefaultColors();

	COLORREF GetColor(ControlState state, ColorArea area);
	void SetColor(ControlState state, ColorArea area, COLORREF color);
	void AddToColorMap(int state, int area, COLORREF color);
	COLORREF GetFromColorMap(int state, int area);

};
