#pragma once

class CWBButton : public CButton
{
public:
	CWBButton();
	
public:
	
	bool LoadBitmaps(UINT id,          // ResourceID
		int count,        // Num Of Pics
		int TopHeight,    // Top Margin for Resizing
		int BottomHeight, // Bottom Margin for Resizing
		int LeftWidth,    // Left Margin for Resizing
		int RightWidth    // Right Margin for Resizing
		);
	
	void SetButtonDef(int TopHeight, int BottomHeight, int LeftWidth, int RightWidth);

protected:

	enum mode
	{
		normal = 0,
		select = 1,
		focus = 2,
		disable = 3
	};

	enum state
	{
		notInited = 0,
		FileLoaded = 1,
		BitmapInited = 2
	};
	
	bool InitBitmap(CBitmap & src, CBitmap & dist, int index, int count);

	void DrawBitmap(CDC * pDC, int mode);

	UINT m_RcId;       // Resource ID
	int  m_NumofPics;

	CBitmap NormalBitmap;
	CBitmap SelectBitmap;
	CBitmap FocusBitmap;
	CBitmap DisableBitmap;

	int m_TopHeight;
	int m_BottomHeight;
	int m_LeftWidth;
	int m_RightWidth;

	int m_dwWidth;
	int m_dwHeight;

	int m_State;

	COLORREF m_TransparentColor;
	CFont m_TextFont;

	bool m_bTracking;

public:

	void SetTransparentColor(COLORREF color) { m_TransparentColor = color; }
	void ActAsCheckBox(bool fActAsCheckBox);
	COLORREF GetBackColor() { return m_TransparentColor; }
	bool m_bCheckBox;
	bool m_bChecked;

	//{{AFX_VIRTUAL(CWBButton)
public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

public:
	virtual ~CWBButton();

protected:
	//{{AFX_MSG(CWBButton)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);	
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG

	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnMouseLeave();

	DECLARE_MESSAGE_MAP()
};
