/*******************************************************************************

INTEL CORPORATION PROPRIETARY INFORMATION
This software is supplied under the terms of a license agreement or nondisclosure
agreement with Intel Corporation and may not be copied or disclosed except in
accordance with the terms of that agreement
Copyright(c) 2013 Intel Corporation. All Rights Reserved.

*******************************************************************************/

#include <windows.h>
#include "resource.h"
#include "render_blob.h"

bool CAUXBlobRender::UpdateFrame(PXCImage *rgbImage, PXCGesture *detector, PXCGesture::Gesture *gdata) 
{
    bool res = false;
    if (detector) {
        PXCImage* blobImage = NULL;
        detector->QueryBlobImage(PXCGesture::Blob::LABEL_SCENE,0,&blobImage);
        if (blobImage) {
            res = CAUXStreamRender::UpdateFrame(blobImage, true);
            blobImage->Release();
        }

    }
    return res;
}

void CAUXBlobRender::DrawMore(HDC hdc, double sx, double sy) 
{
    return;
}
