/*******************************************************************************

INTEL CORPORATION PROPRIETARY INFORMATION
This software is supplied under the terms of a license agreement or nondisclosure
agreement with Intel Corporation and may not be copied or disclosed except in
accordance with the terms of that agreement
Copyright(c) 2013 Intel Corporation. All Rights Reserved.

*******************************************************************************/

#include "render_face.h"
#include <tchar.h>

FaceRender::FaceRender(int posX, int posY, pxcCHAR *title):CAUXStreamRender(posX, posY, title) {
}

FaceRender::FaceData& FaceRender::Insert(pxcUID fid) 
{
    std::map<pxcUID,FaceRender::FaceData>::iterator itr=m_faces.find(fid);
    if (itr!=m_faces.end()) return itr->second;
    FaceData data;
    memset(&data,0,sizeof(data));
    m_faces.insert(std::pair<pxcUID,FaceData>(fid,data));
    return m_faces.find(fid)->second;
}

void FaceRender::ClearData(void) {
    m_faces.clear();
}

void FaceRender::SetDetectionData(PXCFaceAnalysis::Detection::Data *data) {
    FaceRender::FaceData& itr=Insert(data->fid);
    itr.location=data->rectangle;
}

void FaceRender::DrawMore(HDC hdc, double sx, double sy) 
{
    for (std::map<pxcUID,FaceData>::iterator itr2=m_faces.begin();itr2!=m_faces.end();itr2++) {
        FaceData& itr=itr2->second;
 
        // draw face rectangle
        if (itr.location.w>0 && itr.location.h>0) {
            RECT rect={ (LONG)(itr.location.x*sx), (LONG)(itr.location.y*sy), (LONG)((itr.location.w+itr.location.x)*sx), (LONG)((itr.location.h+itr.location.y)*sy) };
            if (IsMirror()) {
                int left = m_sizeX - rect.right;
                rect.right = m_sizeX - rect.left;
                rect.left = left;
            }
            HBRUSH brush = CreateSolidBrush(RGB(255,0,0));
            FrameRect(hdc, &rect, brush); 
            DeleteObject(brush);
        }
    }
}

