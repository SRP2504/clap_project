/*******************************************************************************

INTEL CORPORATION PROPRIETARY INFORMATION
This software is supplied under the terms of a license agreement or nondisclosure
agreement with Intel Corporation and may not be copied or disclosed except in
accordance with the terms of that agreement
Copyright(c) 2013 Intel Corporation. All Rights Reserved.

*******************************************************************************/

#pragma once
#include <Windows.h>
#include "pxcimage.h"
#include "atlstr.h"

#include "AUXWidget.h"

#define MAX_STRINGS 5 

class CAUXTextRender : public CAUXWidget
{
public:
    CAUXTextRender(int posX, int posY, pxcCHAR *title );
    virtual ~CAUXTextRender(void);

    void SetBackground(HBITMAP bitmap);
    bool UpdateText(WCHAR* text, int counter);
    void Release() { delete this; }
    struct {
        HFONT font;
        CString str;
        int top;
        int bottom;
        int counter;
    } m_text[MAX_STRINGS];
    CString m_title;
    HFONT m_titleFont;
    int m_titleSize;

protected:
    HBITMAP m_bitmap;
    bool    m_bSmall;
    void    Destroy();
    void    Init(bool smallSize=false);

    virtual void OnPaint(HDC hdc);
};

