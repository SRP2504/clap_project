/*******************************************************************************

INTEL CORPORATION PROPRIETARY INFORMATION
This software is supplied under the terms of a license agreement or nondisclosure
agreement with Intel Corporation and may not be copied or disclosed except in
accordance with the terms of that agreement
Copyright(c) 2013 Intel Corporation. All Rights Reserved.

*******************************************************************************/

#include <tchar.h>
#include <math.h>
#include "AUXStreamRender.h"
#include "pxcsmartptr.h"

CAUXStreamRender::CAUXStreamRender(int posX, int posY, pxcCHAR *title) : CAUXWidget(posX, posY, title)
{
    m_bitmap=0;
    memset(&m_info,0,sizeof(m_info));
    m_buffer=0;
    SetMirror(TRUE);
}

CAUXStreamRender::~CAUXStreamRender(void) 
{
    if (m_bitmap) DeleteObject(m_bitmap);
    if (m_buffer) delete [] m_buffer;
}

void CAUXStreamRender::SetSize(pxcU32 width, pxcU32 height) 
{
    if (m_buffer) delete [] m_buffer;
    m_buffer=new pxcU32[width*height];

    m_info.bmiHeader.biWidth=width;
    m_info.bmiHeader.biHeight= - (int)height;
    m_info.bmiHeader.biBitCount=32;
    m_info.bmiHeader.biPlanes=1;
    m_info.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
    m_info.bmiHeader.biCompression=BI_RGB;

    GetWindowInfo(m_hWnd, &this->m_lastWinInfo);
    SetWindowPos( m_hWnd, NULL, m_lastWinInfo.rcWindow.left, m_lastWinInfo.rcWindow.top, 
        m_lastWinInfo.rcClient.right-m_lastWinInfo.rcClient.left, m_lastWinInfo.rcClient.bottom-m_lastWinInfo.rcClient.top, 0);//SWP_SHOWWINDOW );
}

void CAUXStreamRender::CopyImage(unsigned char* dst, unsigned char* src, int width, int height, int pitch, bool inverseBlue) 
{
    if (inverseBlue) {
        for(int i=0; i<height; i++) {
            unsigned char* _src = src;
            for(int j=0; j<width; j++) {
                if (_src[0]>200) {
                    dst[0]=dst[1]=dst[2]=dst[3]=255;
                } else if (_src[0]<100) {
                    dst[0] = 232;
                    dst[1] = 162;
                    dst[2] = 0;
                    dst[3] = 255;
                } else {
                    dst[0] = 29;
                    dst[1] = 230;
                    dst[2] = 181;
                    dst[3] = 255;
                }
                dst += 4;
                _src += 4;
            }
            src += pitch;
        }
    } else {
        for(int i=0; i<height; i++) {
            memcpy_s(dst+i*width*4, width*4, src+i*pitch, width*4);
        }
    }
}

bool CAUXStreamRender::UpdateFrame(PXCImage *image, bool inverseBlue) 
{
    if (!image) return false;

    if (m_bitmap) {
        if (!IsWindowVisible(m_hWnd) || m_bPause) {
            DoMessageLoop();
            return true;
        }
        DeleteObject(m_bitmap);
    } else {
        if (!m_info.bmiHeader.biWidth || !m_info.bmiHeader.biHeight) {
            PXCImage::ImageInfo info;
            image->QueryInfo(&info);
            SetSize(info.width, info.height);
        }
    }

    if (m_buffer) {
        PXCImage::ImageInfo info;
        image->QueryInfo(&info);
        PXCImage::ImageData data;
        pxcStatus sts=image->AcquireAccess(PXCImage::ACCESS_READ, PXCImage::COLOR_FORMAT_RGB32, &data);
        if (sts>=PXC_STATUS_NO_ERROR) {
            CopyImage((unsigned char*)m_buffer, (unsigned char*)data.planes[0], m_info.bmiHeader.biWidth, -m_info.bmiHeader.biHeight, data.pitches[0], inverseBlue);
            sts=image->ReleaseAccess(&data);
        }

        HDC dc=GetDC(m_hWnd);
        m_bitmap=CreateDIBitmap(dc, &m_info.bmiHeader, CBM_INIT, m_buffer, &m_info, DIB_RGB_COLORS);
        ReleaseDC(m_hWnd, dc);
        InvalidateRect(m_hWnd, 0, FALSE);
    }

    DoMessageLoop();
    return true;
}

void CAUXStreamRender::OnPaint(HDC hdc)
{
    if (m_bitmap) {
        HDC hdc2= CreateCompatibleDC(hdc);

        SelectObject(hdc2, m_bitmap);
        RECT rect;
        GetClientRect(m_hWnd, &rect);
        SetStretchBltMode(hdc, HALFTONE); // slow but quality
        if (IsMirror()) {
            StretchBlt(hdc,0,0,rect.right-rect.left, rect.bottom-rect.top, hdc2, m_info.bmiHeader.biWidth, 0, -m_info.bmiHeader.biWidth,-m_info.bmiHeader.biHeight, SRCCOPY);
        } else {
            StretchBlt(hdc,0,0,rect.right-rect.left, rect.bottom-rect.top, hdc2, 0, 0, m_info.bmiHeader.biWidth,-m_info.bmiHeader.biHeight, SRCCOPY);
        }
        DeleteDC(hdc2);
        // draw contour
        HBRUSH frame = CreateSolidBrush(RGB(0,162,232));
        FrameRect(hdc, &rect, frame); 
        DeleteObject(frame);
        // draw other
        DrawMore(hdc,(double)(rect.right-rect.left)/m_info.bmiHeader.biWidth,(double)(rect.bottom-rect.top)/(-m_info.bmiHeader.biHeight));
    }
}
