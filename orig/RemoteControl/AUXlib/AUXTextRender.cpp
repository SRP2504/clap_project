/*******************************************************************************

INTEL CORPORATION PROPRIETARY INFORMATION
This software is supplied under the terms of a license agreement or nondisclosure
agreement with Intel Corporation and may not be copied or disclosed except in
accordance with the terms of that agreement
Copyright(c) 2013 Intel Corporation. All Rights Reserved.

*******************************************************************************/

#include <tchar.h>
#include <math.h>
#include "AUXTextRender.h"
#include "pxcsmartptr.h"

CAUXTextRender::CAUXTextRender(int posX, int posY, pxcCHAR *title) : CAUXWidget(posX, posY, title)
{
    m_title = title;
    m_bSmall = false;
    Init();
}

CAUXTextRender::~CAUXTextRender(void) 
{
    Destroy();
    if (m_bitmap) {
        DeleteObject(m_bitmap);
        m_bitmap = NULL;
    }
}

void CAUXTextRender::Init(bool smallSize)
{
    Destroy();
    m_bSmall = smallSize;
    int large_font_map[MAX_STRINGS+2] = { 10, 14,  24, 20, 16, 14, 12 };
    int small_font_map[MAX_STRINGS+2] = { 10, 8,  10, 10, 10, 8, 8 };
    int* heightmap =  large_font_map;
    if (m_bSmall) heightmap =  small_font_map;
    m_titleSize = heightmap[0];
    m_titleFont = CreateFont(m_titleSize,0,0,0,FW_DONTCARE,TRUE,TRUE,FALSE,DEFAULT_CHARSET,OUT_OUTLINE_PRECIS,
        CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY, VARIABLE_PITCH,TEXT("Times New Roman"));

    int shift = m_titleSize+heightmap[1];
    for (int i=0; i<MAX_STRINGS; i++) {
        m_text[i].top = shift;
        m_text[i].font = CreateFont(heightmap[i+2],0,0,0,FW_DONTCARE,FALSE,FALSE,i!=0,DEFAULT_CHARSET,OUT_OUTLINE_PRECIS,
            CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY, VARIABLE_PITCH,TEXT("Times New Roman"));
        shift += heightmap[i+2] + 1;
        m_text[i].bottom = shift;
        m_text[i].counter = 0;
    } 
}

void CAUXTextRender::Destroy()
{
    if (m_titleFont) {
         DeleteObject(m_titleFont);
         m_titleFont = NULL;
    }
    for (int i=0; i<MAX_STRINGS; i++) {
        if (m_text[i].font) {
            DeleteObject(m_text[i].font);
            m_text[i].font = NULL;
        }
    }
}

void CAUXTextRender::SetBackground(HBITMAP bitmap)
{
    if (m_bitmap) DeleteObject(m_bitmap);
    m_bitmap = bitmap;
}

bool CAUXTextRender::UpdateText(WCHAR* text, int counter) 
{
    if (!m_bPause && IsWindowVisible(m_hWnd)) {
        for (int i=MAX_STRINGS-1; i>0; i--) {
            m_text[i].str = m_text[i-1].str;
            m_text[i].counter = m_text[i-1].counter;
        }
        m_text[0].str = text;
        if (counter>0) {
            m_text[0].str.AppendFormat(L" (%i)", counter);
        }
        m_text[0].counter = counter;
        InvalidateRect(m_hWnd, 0, FALSE);
    }

    DoMessageLoop();
    return true;
}

void CAUXTextRender::OnPaint(HDC hdc)
{
    RECT rect;
    GetClientRect(m_hWnd, &rect);
    if (rect.right-rect.left<100) {
        if (!m_bSmall) Init(true);
    } else if (m_bSmall) Init(false);

    if (m_bSmall) {
        HBRUSH fillBrush = CreateSolidBrush(GetBkColor(hdc));
        FillRect(hdc, &rect, fillBrush);
        // draw contour
        HBRUSH frame = CreateSolidBrush(RGB(0,162,232));
        FrameRect(hdc, &rect, frame); 
        DeleteObject(frame);
        DeleteObject(fillBrush);
    } else {
        HDC hdc2= CreateCompatibleDC(hdc);
        SelectObject(hdc2, m_bitmap);
        SetStretchBltMode(hdc, HALFTONE); // slow but quality
        int w = rect.right-rect.left;
        int h = rect.bottom-rect.top;
        StretchBlt(hdc,0,0,w,h, hdc2, 0, 0, w,h,SRCCOPY);
        DeleteDC(hdc2);
    }

    // draw title
    int len = m_title.GetLength();
    if (len) {
        SelectObject(hdc, m_titleFont);
        rect.left = m_bSmall ? 4 : 8;
        rect.top = m_bSmall ? 4 : 8;
        rect.bottom =  rect.top + m_titleSize;
        DrawText(hdc, m_title, len, &rect, DT_LEFT|DT_SINGLELINE|DT_TOP);
    }

    // draw text strings
    rect.left = 10;
    for (int i=0; i<MAX_STRINGS; i++) {
        len = m_text[i].str.GetLength();
        if (len) {
            SelectObject(hdc, m_text[i].font);
            rect.top = m_text[i].top;
            rect.bottom = m_text[i].bottom;
            DrawText(hdc, m_text[i].str, len, &rect, DT_LEFT|DT_VCENTER|DT_SINGLELINE);
        }
    }
}
