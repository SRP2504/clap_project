/*******************************************************************************

INTEL CORPORATION PROPRIETARY INFORMATION
This software is supplied under the terms of a license agreement or nondisclosure
agreement with Intel Corporation and may not be copied or disclosed except in
accordance with the terms of that agreement
Copyright(c) 2013 Intel Corporation. All Rights Reserved.

*******************************************************************************/

#include <windows.h>
#include <resource.h>
#include "render_gesture.h"

#define COLOR_RED        RGB(255,0,0)
#define COLOR_GREEN      RGB(0,255,0)
#define COLOR_BLUE       RGB(0,0,255)
#define COLOR_YELLOW     RGB(255,255,0)

static struct {
    PXCGesture::GeoNode::Label  from;
    PXCGesture::GeoNode::Label  to;
} skeletonLines[] = 
{
    { PXCGesture::GeoNode::LABEL_BODY_HAND_LEFT, PXCGesture::GeoNode::LABEL_BODY_HAND_LEFT|PXCGesture::GeoNode::LABEL_FINGER_THUMB },
    { PXCGesture::GeoNode::LABEL_BODY_HAND_LEFT, PXCGesture::GeoNode::LABEL_BODY_HAND_LEFT|PXCGesture::GeoNode::LABEL_FINGER_INDEX },
    { PXCGesture::GeoNode::LABEL_BODY_HAND_LEFT, PXCGesture::GeoNode::LABEL_BODY_HAND_LEFT|PXCGesture::GeoNode::LABEL_FINGER_MIDDLE },
    { PXCGesture::GeoNode::LABEL_BODY_HAND_LEFT, PXCGesture::GeoNode::LABEL_BODY_HAND_LEFT|PXCGesture::GeoNode::LABEL_FINGER_RING },
    { PXCGesture::GeoNode::LABEL_BODY_HAND_LEFT, PXCGesture::GeoNode::LABEL_BODY_HAND_LEFT|PXCGesture::GeoNode::LABEL_FINGER_PINKY },
    { PXCGesture::GeoNode::LABEL_BODY_HAND_RIGHT, PXCGesture::GeoNode::LABEL_BODY_HAND_RIGHT|PXCGesture::GeoNode::LABEL_FINGER_THUMB },
    { PXCGesture::GeoNode::LABEL_BODY_HAND_RIGHT, PXCGesture::GeoNode::LABEL_BODY_HAND_RIGHT|PXCGesture::GeoNode::LABEL_FINGER_INDEX },
    { PXCGesture::GeoNode::LABEL_BODY_HAND_RIGHT, PXCGesture::GeoNode::LABEL_BODY_HAND_RIGHT|PXCGesture::GeoNode::LABEL_FINGER_MIDDLE },
    { PXCGesture::GeoNode::LABEL_BODY_HAND_RIGHT, PXCGesture::GeoNode::LABEL_BODY_HAND_RIGHT|PXCGesture::GeoNode::LABEL_FINGER_RING },
    { PXCGesture::GeoNode::LABEL_BODY_HAND_RIGHT, PXCGesture::GeoNode::LABEL_BODY_HAND_RIGHT|PXCGesture::GeoNode::LABEL_FINGER_PINKY },
};

bool CAUXGestureRender::UpdateFrame(PXCImage *rgbImage, PXCGesture *detector, PXCGesture::Gesture *gdata) 
{
    m_lines.clear();
    if (detector) {
        PXCImage::ImageInfo rgbInfo={0,0,0};
        rgbImage->QueryInfo(&rgbInfo);

        for (int u=0;;u++) {
            pxcUID user=0;
            pxcStatus sts= detector->QueryUser(u,&user);
            if (sts<PXC_STATUS_NO_ERROR) break;

            for (int i = 0; i < sizeof(skeletonLines)/sizeof(skeletonLines[0]); i++) {
                PXCGesture::GeoNode dataFrom;
                if (detector->QueryNodeData(user,skeletonLines[i].from,&dataFrom)<PXC_STATUS_NO_ERROR) continue;
                PXCGesture::GeoNode dataTo;
                if (detector->QueryNodeData(user,skeletonLines[i].to,&dataTo)<PXC_STATUS_NO_ERROR) continue;
                
                float x0=dataFrom.positionImage.x;
                float y0=dataFrom.positionImage.y;
                float x1=dataTo.positionImage.x;
                float y1=dataTo.positionImage.y;
                if (x0 > 0 && x0 < rgbInfo.width && y0 > 0 && y0 < rgbInfo.height &&
                    x1 > 0 && x1 < rgbInfo.width && y1 > 0 && y1 < rgbInfo.height) {
                    Line line={(int)x0,(int)y0,(int)x1,(int)y1};
                    m_lines.push_back(line);
                }
            }
        }
    }

    return CAUXStreamRender::UpdateFrame(rgbImage);
}

void CAUXGestureRender::DrawMore(HDC hdc, double sx, double sy) 
{
    HPEN red=CreatePen(PS_SOLID, 4, COLOR_RED);
    SelectObject(hdc, red);
    for (std::list<Line>::iterator itrl=m_lines.begin(); itrl!=m_lines.end(); itrl++) {
        if (IsMirror()) {
            MoveToEx(hdc, (int)(m_sizeX-itrl->x0*sx), (int)(itrl->y0*sy), NULL); 
            LineTo(hdc, (int)(m_sizeX-itrl->x1*sx), (int)(itrl->y1*sy)); 
        } else {
            MoveToEx(hdc, (int)(itrl->x0*sx), (int)(itrl->y0*sy), NULL); 
            LineTo(hdc, (int)(itrl->x1*sx), (int)(itrl->y1*sy)); 
        }
    }

    DeleteObject(red);
}
