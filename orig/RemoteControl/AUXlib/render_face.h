/*******************************************************************************

INTEL CORPORATION PROPRIETARY INFORMATION
This software is supplied under the terms of a license agreement or nondisclosure
agreement with Intel Corporation and may not be copied or disclosed except in
accordance with the terms of that agreement
Copyright(c) 2013 Intel Corporation. All Rights Reserved.

*******************************************************************************/

#pragma once
#include <map>
#include "AUXStreamRender.h"
#include "pxcface.h"

class FaceRender: public CAUXStreamRender {
public:

    FaceRender(int posX, int posY, pxcCHAR *title);

    void ClearData(void);
    void SetDetectionData(PXCFaceAnalysis::Detection::Data *data);
    
protected:

    struct FaceData {
        PXCRectU32  location;
        pxcCHAR     name[64];
    }; 

    std::map<pxcUID,FaceData> m_faces;

    FaceData&    Insert(pxcUID fid);
    virtual void DrawMore(HDC hdc, double scale_x, double scale_y);
};

