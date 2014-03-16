/* /////////////////////////////////////////////////////////////////////////////
//
//                  INTEL CORPORATION PROPRIETARY INFORMATION
//     This software is supplied under the terms of a license agreement or
//     nondisclosure agreement with Intel Corporation and may not be copied
//     or disclosed except in accordance with the terms of that agreement.
//          Copyright(c) 2011 Intel Corporation. All Rights Reserved.
//
*/
#pragma once

class Thread {
public:

    Thread(void) { thread=0; stop=false; video=true; }

    virtual void Start(BOOL videoControl);
    virtual void Stop(void);
    HANDLE  GetHandle(void) { return thread; }
    bool    stop;
	bool	video;

protected:
    HANDLE  thread;

    virtual void RunThread(void)=0;
    static DWORD WINAPI ThreadCallback(LPVOID hdl);
};

