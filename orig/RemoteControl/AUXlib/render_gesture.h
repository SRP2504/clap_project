/*******************************************************************************

INTEL CORPORATION PROPRIETARY INFORMATION
This software is supplied under the terms of a license agreement or nondisclosure
agreement with Intel Corporation and may not be copied or disclosed except in
accordance with the terms of that agreement
Copyright(c) 2013 Intel Corporation. All Rights Reserved.

*******************************************************************************/

#pragma once
#include <list>
#include <map>
#include "pxcgesture.h"
#include "AUXStreamRender.h"

class CAUXGestureRender: public CAUXStreamRender 
{
public:
    CAUXGestureRender(int posX, int posY, pxcCHAR *title):CAUXStreamRender(posX, posY, title) {}
    bool UpdateFrame(PXCImage *rgbImage, PXCGesture *detector, PXCGesture::Gesture *gdata);

protected:
    virtual void DrawMore(HDC hdc, double scale_x, double scale_y);

    struct Line {
        int x0, y0;
        int x1, y1;
    };
    std::list<Line>     m_lines;
};

