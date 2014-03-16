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
#include "AUXWidget.h"

class CAUXStreamRender : public CAUXWidget
{
public:
    CAUXStreamRender(int posX, int posY, pxcCHAR *title );
    virtual ~CAUXStreamRender(void);

    void SetSize(pxcU32 width, pxcU32 height);
    bool UpdateFrame(PXCImage *image, bool inverseBlue=false);
    void CopyImage(unsigned char* dst, unsigned char* src, int width, int height, int pitch, bool inverseBlue);
    void Release() { delete this; };

protected:
    PXCImage*   m_image;
    HBITMAP     m_bitmap;
    BITMAPINFO  m_info;
    pxcU32*     m_buffer;

    virtual void OnPaint(HDC hdc);
    virtual void DrawMore(HDC hdc, double scale_x, double scale_y) {}
};

