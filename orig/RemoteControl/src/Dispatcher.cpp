/*******************************************************************************

INTEL CORPORATION PROPRIETARY INFORMATION
This software is supplied under the terms of a license agreement or nondisclosure
agreement with Intel Corporation and may not be copied or disclosed except in
accordance with the terms of that agreement
Copyright(c) 2013 Intel Corporation. All Rights Reserved.

*******************************************************************************/
    
#include "stdafx.h"
#include "Dispatcher.h"
#include "invoker.h"
#include "..\sensors\sensor_face_attribute\sensor_face_attribute.hpp"
#include "..\sensors\sensor_gesture_CI\sensor_gesture_CI.hpp"
#include "..\sensors\sensor_voice\sensor_voice.hpp"

#include <Psapi.h>
#include <Strsafe.h>
#include <msxml6.h>

extern CInvoker g_Invoker;
extern CDispatcher g_Dispatcher;
#include "..\AUXlib\AUXWidgetManager.h"
extern CAUXWidgetManager g_widgets;

void stActionDescriptor::GetSensorTitle(CString& str, BOOL bMultiString)
{
    CSensor* sens = g_Dispatcher.GetSensorById((CSensor::SENSOR_ID)sensorId);
    if (bMultiString) {
        if (sens) {
            if (isEnable) str.Append(_T("\n -enabled-\n Sensor = < " ));
            else          str.Append(_T("\n -disabled-\n Sensor = < " ));
            str.Append(sens->GetName());
            str.Append(L" >\n Event   = < "); 
            st_eventDescriptor descr;
            if (sens->GetEventbyId(eventId, &descr)) {
                str.Append(descr.name);
                str.Append(L" >"); 
            }
        } else {
            str.Append(L"\n no sensor ");
        }
    } else {
        if (sens) {
            str.Append(_T("< " ));
            str.Append(sens->GetName());
            str.Append(L" >  ::  < "); 
            st_eventDescriptor descr;
            if (sens->GetEventbyId(eventId, &descr)) {
                str.Append(descr.name);
                str.Append(L" >  :: "); 
            }
            if (isEnable) str.Append(_T("Enable" ));
            else          str.Append(_T("Disable" ));
        } else {
            str.Append(L"\n no sensor ");
        }
    }
}

void stActionDescriptor::GetTitle(CString& str, BOOL bName, BOOL bAction, BOOL bType)
{
    if (bName) {
        if (isGrouping) {
            str.Append(L"[");
            str.Append(windowName);
            str.Append(L"]");
        } else {
            str.Append(windowName);
        }
    }
    if (bName && (bAction||bType)) str.Append(_T(" :: "));
    switch (type) {
    case stActionDescriptor::MY_AT_PRINT:
        if (bAction) str.Append(_T("Print"));
        if (bAction && bType) str.Append(_T(" :: "));
        if (bType) {
            switch (system_type) {
            case stActionDescriptor::MY_PT_PRINT_EVENT:
                str.Append(_T("Event Name"));
                break;
            case stActionDescriptor::MY_PT_PRINT_EVENT_ENTER:
                str.Append(_T("Event Name + Return"));
                break;
            case stActionDescriptor::MY_PT_PRINT_NUMBER:
                str.Append(_T("Numbers"));
                break;
            }
        }
        break;
    case stActionDescriptor::MY_AT_NAVIGATION:
        if (bAction) str.Append(_T("Navigation"));
        if (bAction && bType) str.Append(_T(" :: "));
        if (bType) {
            switch (navigate_type) {
            case stActionDescriptor::MY_NT_TOP:
                str.Append(L"TOP");
                break;
            case stActionDescriptor::MY_NT_MAX:
                str.Append(L"MAX");
                break;
            case stActionDescriptor::MY_NT_MIN:
                str.Append(L"MIN");
                break;
            }
        }
        break;
    case stActionDescriptor::MY_AT_KEYBOARD:
        if (bAction) str.Append(_T("Keyboard"));
        if (bAction && bType) str.Append(_T(" :: "));
        if (bType) {
            switch (key_type) {
            case stActionDescriptor::MY_KT_ESC:
                str.Append(L"<ESCAPE>");
                break;
            case stActionDescriptor::MY_KT_DEL:
                str.Append(L"<DELETE>");
                break;
            case stActionDescriptor::MY_KT_PRESS:
                str.Append(L"< ");
                str.Append(key_code_name);
                str.Append(L" >");
                break;
            case stActionDescriptor::MY_KT_ENTER:
                str.Append(L"<ENTER>");
                break;
            case stActionDescriptor::MY_KT_SPACE:
                str.Append(L"<SPACE>");
                break;
            case stActionDescriptor::MY_KT_BKSPACE:
                str.Append(L"<BACKSPACE>");
                break;
            case stActionDescriptor::MY_KT_TAB:
                str.Append(L"<TAB>");
                break;
            case stActionDescriptor::MY_KT_WIN:
                str.Append(L"<WIN>");
                break;
            case stActionDescriptor::MY_KT_ALT_PRESS:
                str.Append(L"Down <ALT>");
                break;
            case stActionDescriptor::MY_KT_ALT_RELEASE:
                str.Append(L"Up <ALT>");
                break;
            }
        }
        break;
    case stActionDescriptor::MY_AT_BUTTON:
        if (bAction) str.Append(_T("Button"));
        if (bAction && bType) str.Append(_T(" :: "));
        if (bType) {
            str.Append(controlName);
        }
        break;
    }
};

CDispatcher::CDispatcher(void)
{
    // fill sensors' list 
    for (int i=0; i<MAX_SENSORS; i++) m_sensorList[i] = 0;
    m_sensorList[0] = (CSensor*) new CSensorGestureCI;
    m_sensorList[1] = (CSensor*) new CSensorFaceAttribute;
    m_sensorList[2] = (CSensor*) new CSensorVoice;
    m_actionList.clear();
    m_taskList.clear();
    m_testConfigIsLoaded = false;
    m_tmpStr1 = SysAllocStringLen(NULL, MAX_PATH);
    m_tmpStr2 = SysAllocStringLen(NULL, MAX_PATH);
    m_processList = NULL;
    m_wasFileLoad = FALSE;
}

CDispatcher::~CDispatcher(void)
{
    Release();
    if (m_sensorList[0]) delete (CSensorGestureCI*)m_sensorList[0];
    if (m_sensorList[1]) delete (CSensorFaceAttribute*)m_sensorList[1];
    if (m_sensorList[2]) delete (CSensorVoice*)m_sensorList[2];
    for (int i=0; i<MAX_SENSORS; i++) m_sensorList[i] = 0;
    m_taskList.clear();
    SysFreeString(m_tmpStr1);
    SysFreeString(m_tmpStr2);
    if (m_processList) {
        m_processList->Release();
        m_processList = NULL;
    }
}

void CDispatcher::Release()
{
    for (std::map<int, stActionDescriptor*>::iterator itr = m_actionList.begin(); itr != m_actionList.end(); itr++) {
        if (itr->second) delete itr->second;
        itr->second = NULL;
    }
    m_taskList.clear();
    m_actionList.clear();
    m_testConfigIsLoaded = false;
}

void CDispatcher::StartFillTaskList()
{
    g_Invoker.Lock();
    m_taskList.clear();
}

int CDispatcher::StopFillTaskList()
{
    g_Invoker.UnLock();
    return (int)m_taskList.size();
}

int CDispatcher::AppendTaskToList(int id)
{
    std::map<int, stActionDescriptor*>::iterator iter = m_actionList.find(id);
    if (iter != m_actionList.end()) {
        m_taskList.push_back(iter->second);
    }
    return 1;
}

int CDispatcher::AddAction(const stActionDescriptor* descr)
{
    // return actionId 
    int id = (int)m_actionList.size();
    if (m_actionList[id]) {
        delete (stActionDescriptor*)(m_actionList[id]);
        m_actionList[id] = NULL;
    }
    stActionDescriptor* action = new stActionDescriptor;
    memcpy(action, descr, sizeof(stActionDescriptor));
    m_actionList[id] = action;
    return id;
}

stActionDescriptor* CDispatcher::GetAction(unsigned int num)
{
    if (num>=m_actionList.size()) return NULL;

    std::map<int, stActionDescriptor*>::iterator iter = m_actionList.begin();
    for (unsigned int i=0; i<num; i++) iter++;
    return iter->second;
}

CSensor* CDispatcher::GetSensorById(CSensor::SENSOR_ID id)
{
    for (int i=0; i<MAX_SENSORS; i++) {
        CSensor* sensor = GetSensor(i);
        if (sensor && sensor->GetId()==id) return sensor;
    }
    return NULL;
}

stActionDescriptor* CDispatcher::GetTask(unsigned int num)
{
    if (num>=m_taskList.size()) return NULL;
    return m_taskList[num];
}

static WCHAR BASED_CODE szFilter[] = L"Text (*.xml)|*.xml|All Files (*.*)|*.*||";
BOOL CDispatcher::LoadActionList(CString* name, BOOL reset)
{
    CString path;
    WCHAR app_path[MAX_PATH];
    WCHAR* startPath = NULL;
    if (name==NULL) {
        // select a file
        if (!m_wasFileLoad) {
            // first time start from the current dir
            WCHAR app_name[MAX_PATH];
            ::GetModuleFileName(0, app_name, MAX_PATH);
            _wfullpath(app_path, app_name, MAX_PATH);
            WCHAR drive[_MAX_DRIVE];
            WCHAR dir[_MAX_DIR];
            WCHAR fname[_MAX_FNAME];
            WCHAR ext[_MAX_EXT];
            _wsplitpath_s(app_path, drive, dir, fname, ext );
            _wmakepath_s(app_path, drive, dir, NULL, NULL );
            startPath = app_path;
        }
        CFileDialog dialog(1, L".xml", startPath, 0, szFilter);
        if (dialog.DoModal()!=IDOK) return false;
        path = dialog.GetPathName();
        name = &path;
    }
    if(name==NULL) return false;
    return LoadXML(name->GetBuffer(), reset);
}

static HRESULT XMLGetStringValue(IXMLDOMNode* root, WCHAR* name, WCHAR* attr, WCHAR* dest, int len)
{ 
    HRESULT hr;
    CComPtr<IXMLDOMNode> pNode;
    VARIANT value;
    VariantInit(&value);
    VariantClear(&value);
    hr = root->selectSingleNode(name, &pNode); if (FAILED(hr)) return hr;
    if (pNode==NULL) return 0;
    CComPtr<IXMLDOMNamedNodeMap> pAttrList;
    hr = pNode->get_attributes(&pAttrList); if (FAILED(hr)) return hr;
    pNode.Release();
    hr = pAttrList->getNamedItem(attr, &pNode); if (FAILED(hr)) return hr;
    if (pNode==NULL) return 0;
    hr = pNode->get_nodeValue(&value); if (FAILED(hr)) return hr;
    wcscpy_s(dest, len, value.bstrVal);
    VariantClear(&value);
    return 1;
}

static HRESULT XMLGetIntegerValue(IXMLDOMNode* root, WCHAR* name, WCHAR* attr, int* dest)
{ 
    HRESULT hr;
    CComPtr<IXMLDOMNode> pNode;
    VARIANT value;
    VariantInit(&value);
    VariantClear(&value);
    hr = root->selectSingleNode(name, &pNode); if (FAILED(hr)) return hr;
    if (pNode==NULL) return 0;
    CComPtr<IXMLDOMNamedNodeMap> pAttrList;
    hr = pNode->get_attributes(&pAttrList); if (FAILED(hr)) return hr;
    pNode.Release();
    hr = pAttrList->getNamedItem(attr, &pNode); if (FAILED(hr)) return hr;
    if (pNode==NULL) return 0;
    hr = pNode->get_nodeValue(&value); if (FAILED(hr)) return hr;
    long val = 0;
    VarI4FromStr(value.bstrVal, 0, LOCALE_NOUSEROVERRIDE, &val); 
    *dest = (int)val;
    VariantClear(&value);
    return 1;
}

BOOL CDispatcher::LoadXML(WCHAR* fileName, BOOL reset)
{
#define _CHK(func) hr=func; if (FAILED(hr)) {throw false;}
#define _CHK2(func,val) hr=func; if (FAILED(hr)||val==NULL) {throw false;}

    BOOL ret_code = TRUE;
    HRESULT hr;
    stActionDescriptor* descr = NULL;
    try {
        CComPtr<IXMLDOMDocument> pXMLDom;
        CComPtr<IXMLDOMNode> pRC;
        CComPtr<IXMLDOMNode> pTS;
        VARIANT_BOOL varBool;
        _CHK(CoInitialize(NULL));
        // Create and init DOM
        _CHK(CoCreateInstance(__uuidof(DOMDocument60), NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pXMLDom)));
        _CHK(pXMLDom->put_async(VARIANT_FALSE));
        _CHK(pXMLDom->put_validateOnParse(VARIANT_FALSE));
        _CHK(pXMLDom->put_resolveExternals(VARIANT_FALSE));
        _CHK(pXMLDom->load(_variant_t(fileName), &varBool));
        _CHK2(pXMLDom->selectSingleNode(L"RC", &pRC),pRC);
        _CHK2(pRC->selectSingleNode(L"TaskList", &pTS),pTS);
        CComPtr<IXMLDOMNodeList> pTaskList;
        pTS->selectNodes(L"Task", &pTaskList);
        long taskCount=0;
        _CHK(pTaskList->get_length(&taskCount));
        if (taskCount==0) throw false;
        if (reset) Release();
        for (long i=0; i<taskCount; i++) {
            CComPtr<IXMLDOMNode> pTask;
            CComPtr<IXMLDOMNode> pApp;
            CComPtr<IXMLDOMNode> pAction;
            CComPtr<IXMLDOMNode> pCamera;
            _CHK2(pTaskList->get_item(i, &pTask),pTask);
            _CHK2(pTask->selectSingleNode(L"Application", &pApp),pApp);
            _CHK2(pTask->selectSingleNode(L"Action", &pAction),pAction);
            _CHK(pTask->selectSingleNode(L"Camera", &pCamera));
            descr = new stActionDescriptor;
            _CHK(XMLGetIntegerValue(pTask, L"Application", L"enabled", &descr->isEnable));
            _CHK(XMLGetStringValue(pApp, L"Window", L"name", descr->windowName, MAX_PATH));
            if (hr==1) {
                _CHK(XMLGetStringValue(pApp, L"Class", L"name", descr->className, MAX_PATH));
            } else {
                _CHK(XMLGetStringValue(pApp, L"Group", L"name", descr->windowName, MAX_PATH));
                if (hr==1) descr->isGrouping = 1;
            }
            if (_wcsicmp(descr->windowName, L"_any_active_application_")==0) {
                descr->specific = stActionDescriptor::MY_ST_ANY_ACTIVE;
            } else {
                descr->specific = stActionDescriptor::MY_ST_NORMAL;
            }
            // load action
            _CHK(XMLGetIntegerValue(pAction, L"Common", L"type", (int*)&descr->type));
            switch(descr->type) {
            case stActionDescriptor::MY_AT_NAVIGATION:
                _CHK(XMLGetIntegerValue(pAction, L"Navigation", L"type", (int*)&descr->navigate_type));
                break;
            case stActionDescriptor::MY_AT_KEYBOARD:
                _CHK(XMLGetIntegerValue(pAction, L"Key", L"type", (int*)&descr->key_type));
                _CHK(XMLGetIntegerValue(pAction, L"Key", L"code", (int*)&descr->key_code));
                _CHK(XMLGetStringValue(pAction, L"Key", L"name", descr->key_code_name, 20));
                break;
            case stActionDescriptor::MY_AT_BUTTON:
                _CHK(XMLGetStringValue(pAction, L"Button", L"name", descr->controlName, MAX_PATH));
                _CHK(XMLGetIntegerValue(pAction, L"Button", L"auid", (int*)&descr->automationId));
                _CHK(XMLGetIntegerValue(pAction, L"Button", L"type", (int*)&descr->rtType));
                _CHK(XMLGetIntegerValue(pAction, L"Button", L"cnt", (int*)&descr->rtCounter));
                _CHK(XMLGetIntegerValue(pAction, L"Button", L"r0", (int*)&descr->runtimeId[0]));
                _CHK(XMLGetIntegerValue(pAction, L"Button", L"r1", (int*)&descr->runtimeId[1]));
                _CHK(XMLGetIntegerValue(pAction, L"Button", L"r2", (int*)&descr->runtimeId[2]));
                _CHK(XMLGetIntegerValue(pAction, L"Button", L"r3", (int*)&descr->runtimeId[3]));
                _CHK(XMLGetIntegerValue(pAction, L"Button", L"r4", (int*)&descr->runtimeId[4]));
                _CHK(XMLGetIntegerValue(pAction, L"Button", L"r5", (int*)&descr->runtimeId[5]));
                _CHK(XMLGetIntegerValue(pAction, L"Button", L"r6", (int*)&descr->runtimeId[6]));
                _CHK(XMLGetIntegerValue(pAction, L"Button", L"r7", (int*)&descr->runtimeId[7]));
                _CHK(XMLGetIntegerValue(pAction, L"Button", L"r8", (int*)&descr->runtimeId[8]));
                _CHK(XMLGetIntegerValue(pAction, L"Button", L"r9", (int*)&descr->runtimeId[9]));
                break;
            case stActionDescriptor::MY_AT_PRINT:
                _CHK(XMLGetIntegerValue(pAction, L"Print", L"type", (int*)&descr->system_type));
                break;
            }
            // load sensor
            if (pCamera) {
                _CHK(XMLGetIntegerValue(pCamera, L"Sensor", L"id", (int*)&descr->sensorId));
                if (descr->sensorId==CSensor::Sensor_Voice_recognition) {
                    WCHAR cmd[256];
                    _CHK(XMLGetStringValue(pCamera, L"Voice", L"cmd", cmd, 256));
                    CSensor* sensor = GetSensorById(CSensor::SENSOR_ID(descr->sensorId));
                    // check unique
                    for (int i=0; ; i++) {
                        descr->eventId = sensor->GetEventId(i);
                        if (descr->eventId<=0) {
                            descr->eventId = sensor->AddEvent(cmd);
                            break;
                        }
                        st_eventDescriptor event_descr;
                        sensor->GetEventbyId(descr->eventId, &event_descr);
                        if (_wcsicmp(event_descr.name, cmd)==0) break;
                    }
                } else {
                    _CHK(XMLGetIntegerValue(pCamera, L"Event", L"id", (int*)&descr->eventId));
                }
            }
            // invalidate descr
            descr->isActual = 1;
            // add descr to list
            int id = (int)m_actionList.size();
            m_actionList[id] = descr;
            descr = NULL;
        }
        // load helpers
        CComPtr<IXMLDOMNode> pHS;
        _CHK(pRC->selectSingleNode(L"HelperList", &pHS));
        if (pHS) {
            int gx = 0;
            int gy = 0;
            _CHK(XMLGetIntegerValue(pHS, L"Group", L"x", &gx));
            _CHK(XMLGetIntegerValue(pHS, L"Group", L"y", &gy));
            g_widgets.UpdateConfiguration(gx, gy, NULL,0,0,0,0);
            CComPtr<IXMLDOMNodeList> pHelperList;
            pHS->selectNodes(L"Helper", &pHelperList);
            long helperCount=0;
            _CHK(pHelperList->get_length(&helperCount));
            for (long i=0; i<helperCount; i++) {
                CComPtr<IXMLDOMNode> pHelper;
                CComPtr<IXMLDOMNode> pName;
                CComPtr<IXMLDOMNode> pPin;
                CComPtr<IXMLDOMNode> pActive;
                CComPtr<IXMLDOMNode> pX;
                CComPtr<IXMLDOMNode> pY;
                CComPtr<IXMLDOMNamedNodeMap> pAttr;
                VARIANT valName;
                VARIANT valPin;
                VARIANT valActive;
                VARIANT valX;
                VARIANT valY;
                _CHK(pHelperList->get_item(i, &pHelper));
                _CHK(pHelper->get_attributes(&pAttr));
                _CHK(pAttr->getNamedItem(L"name", &pName)); 
                _CHK(pAttr->getNamedItem(L"pin", &pPin)); 
                _CHK(pAttr->getNamedItem(L"active", &pActive)); 
                _CHK(pAttr->getNamedItem(L"x", &pX)); 
                _CHK(pAttr->getNamedItem(L"y", &pY)); 
                pName->get_nodeValue(&valName);
                pPin->get_nodeValue(&valPin);
                pActive->get_nodeValue(&valActive);
                pX->get_nodeValue(&valX);
                pY->get_nodeValue(&valY);
                long pin, active, x, y;
                VarI4FromStr(valPin.bstrVal, 0, LOCALE_NOUSEROVERRIDE, &pin); 
                VarI4FromStr(valActive.bstrVal, 0, LOCALE_NOUSEROVERRIDE, &active); 
                VarI4FromStr(valX.bstrVal, 0, LOCALE_NOUSEROVERRIDE, &x); 
                VarI4FromStr(valY.bstrVal, 0, LOCALE_NOUSEROVERRIDE, &y); 
                // update configurartion
                g_widgets.UpdateConfiguration(-1, -1, valName.bstrVal, pin, active, x, y);
                // apply to the widget manager
                VariantClear(&valName);
                VariantClear(&valPin);
                VariantClear(&valX);
                VariantClear(&valY);
            }
        }
    }
    catch(...) {
        if (descr) delete descr;
        descr = NULL;
        ret_code = FALSE;
    }
    CoUninitialize();
    m_wasFileLoad = TRUE;
    return ret_code; 
#undef _CHK 
}

static HRESULT format( IXMLDOMDocument* pDoc, IXMLDOMElement* pElem, int tab) 
{
    CComPtr<IXMLDOMText> pLineFeed;
    CString str = L"\n";
    for (int i=0; i<tab; i++) {
        str.Append(L"   ");
    }
    HRESULT hr = pDoc->createTextNode(str.GetBuffer(), &pLineFeed);
    if (SUCCEEDED(hr)) pElem->appendChild(pLineFeed, NULL);
    return hr;
}

BOOL CDispatcher::SaveXML(WCHAR* fileName)
{
#define _CHK(func) if (FAILED(func)) {throw false;}

    BOOL ret_code = TRUE;
    try {
        CComPtr<IXMLDOMDocument> pXMLDom;
        CComPtr<IXMLDOMElement> pRC;
        CComPtr<IXMLDOMElement> pTS;
        CComPtr<IXMLDOMElement> pHS;
        _CHK(CoInitialize(NULL));
        // Create and init DOM
        _CHK(CoCreateInstance(__uuidof(DOMDocument60), NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pXMLDom)));
        _CHK(pXMLDom->put_async(VARIANT_FALSE));
        _CHK(pXMLDom->put_validateOnParse(VARIANT_FALSE));
        _CHK(pXMLDom->put_resolveExternals(VARIANT_FALSE));
        _CHK(pXMLDom->put_preserveWhiteSpace(VARIANT_TRUE));
        // add info
        CComPtr<IXMLDOMProcessingInstruction> pPI;
        _CHK(pXMLDom->createProcessingInstruction(L"xml", L"version='1.0'", &pPI));
        _CHK(pXMLDom->appendChild(pPI, NULL));
        // add comment
        CComPtr<IXMLDOMComment> pComment;
        _CHK(pXMLDom->createComment(L"RemoteControl", &pComment));
        _CHK(pXMLDom->appendChild(pComment, NULL));
        // Create the root element.
        _CHK(pXMLDom->createElement(L"RC", &pRC));
        _CHK(pXMLDom->appendChild(pRC, NULL)); 
        _CHK(pXMLDom->createElement(L"TaskList", &pTS));
        _CHK(format(pXMLDom, pRC, 0));
        _CHK(pRC->appendChild(pTS, NULL));
        for (int i=0; i<(int)m_taskList.size(); i++) {
            CComPtr<IXMLDOMElement> pTask;
            CComPtr<IXMLDOMElement> pApp;
            CComPtr<IXMLDOMElement> pAction;
            _CHK(pXMLDom->createElement(L"Task", &pTask));
            _CHK(format(pXMLDom, pTS, 1));
            _CHK(pTS->appendChild(pTask, NULL));
            _CHK(pXMLDom->createElement(L"Application", &pApp));
            _CHK(pApp->setAttribute(L"enabled", _variant_t(m_taskList[i]->isEnable)));
            _CHK(format(pXMLDom, pTask, 2));
            _CHK(pTask->appendChild(pApp, NULL));
            _CHK(pXMLDom->createElement(L"Action", &pAction));
            _CHK(format(pXMLDom, pTask, 2));
            _CHK(pTask->appendChild(pAction, NULL));
            // save application
            CComPtr<IXMLDOMElement> pWindow;
            CComPtr<IXMLDOMElement> pClass;
            _CHK(format(pXMLDom, pApp, 3));
            if (m_taskList[i]->isGrouping) {
                _CHK(pXMLDom->createElement(L"Group", &pWindow));
                _CHK(pWindow->setAttribute(L"name", _variant_t(m_taskList[i]->windowName)));
                _CHK(pApp->appendChild(pWindow, NULL));
            } else {
                _CHK(pXMLDom->createElement(L"Window", &pWindow));
                _CHK(pWindow->setAttribute(L"name", _variant_t(m_taskList[i]->windowName)));
                _CHK(pApp->appendChild(pWindow, NULL));
                _CHK(format(pXMLDom, pApp, 3));
                _CHK(pXMLDom->createElement(L"Class", &pClass));
                _CHK(pClass->setAttribute(L"name", _variant_t(m_taskList[i]->className)));
                _CHK(pApp->appendChild(pClass, NULL));
            }
            _CHK(format(pXMLDom, pApp, 2));
            // save Sensor
            if (m_taskList[i]->sensorId) {
                CComPtr<IXMLDOMElement> pCamera;
                CComPtr<IXMLDOMElement> pSensor;
                CComPtr<IXMLDOMElement> pEvent;
                _CHK(pXMLDom->createElement(L"Camera", &pCamera));
                _CHK(format(pXMLDom, pTask, 2));
                _CHK(pTask->appendChild(pCamera, NULL));
                _CHK(format(pXMLDom, pTask, 1));
                _CHK(pXMLDom->createElement(L"Sensor", &pSensor));
                _CHK(pSensor->setAttribute(L"id", _variant_t(m_taskList[i]->sensorId)));
                CSensor* sensor = GetSensorById(CSensor::SENSOR_ID(m_taskList[i]->sensorId));
                _CHK(pSensor->setAttribute(L"title", _variant_t(sensor->GetName())));
                _CHK(format(pXMLDom, pCamera, 3));
                _CHK(pCamera->appendChild(pSensor, NULL));
                st_eventDescriptor event_descr;
                if (sensor->GetEventbyId(m_taskList[i]->eventId, &event_descr)) {
                    if (m_taskList[i]->sensorId==CSensor::Sensor_Voice_recognition) {
                        _CHK(pXMLDom->createElement(L"Voice", &pEvent));
                        _CHK(pEvent->setAttribute(L"cmd", _variant_t(event_descr.name)));
                    } else {
                        _CHK(pXMLDom->createElement(L"Event", &pEvent));
                        _CHK(pEvent->setAttribute(L"id", _variant_t(m_taskList[i]->eventId)));
                        _CHK(pEvent->setAttribute(L"title", _variant_t(event_descr.name)));
                    }
                }
                _CHK(format(pXMLDom, pCamera, 3));
                _CHK(pCamera->appendChild(pEvent, NULL));
                _CHK(format(pXMLDom, pCamera, 2));
            }
            // save Action
            if (m_taskList[i]->type != stActionDescriptor::MY_AT_NONE) {
                CComPtr<IXMLDOMElement> pType;
                _CHK(pXMLDom->createElement(L"Common", &pType));
                _CHK(pType->setAttribute(L"type", _variant_t(m_taskList[i]->type)));
                _CHK(format(pXMLDom, pAction, 3));
                _CHK(pAction->appendChild(pType, NULL));
                pType.Release();
                switch(m_taskList[i]->type) {
                case stActionDescriptor::MY_AT_NAVIGATION:
                    _CHK(pXMLDom->createElement(L"Navigation", &pType));
                    _CHK(pType->setAttribute(L"type", _variant_t(m_taskList[i]->navigate_type)));
                    break;
                case stActionDescriptor::MY_AT_KEYBOARD:
                    _CHK(pXMLDom->createElement(L"Key", &pType));
                    _CHK(pType->setAttribute(L"type", _variant_t(m_taskList[i]->key_type)));
                    if (m_taskList[i]->key_type == stActionDescriptor::MY_KT_PRESS) {
                        _CHK(pType->setAttribute(L"name", _variant_t(m_taskList[i]->key_code_name)));
                        _CHK(pType->setAttribute(L"code", _variant_t(m_taskList[i]->key_code)));
                    }
                    break;
                case stActionDescriptor::MY_AT_BUTTON:
                    _CHK(pXMLDom->createElement(L"Button", &pType));
                    _CHK(pType->setAttribute(L"name", _variant_t(m_taskList[i]->controlName)));
                    if (m_taskList[i]->rtType) {
                        _CHK(pType->setAttribute(L"auid", _variant_t(m_taskList[i]->automationId)));
                    } else {
                        _CHK(pType->setAttribute(L"type", _variant_t(m_taskList[i]->rtType)));
                        _CHK(pType->setAttribute(L"cnt", _variant_t(m_taskList[i]->rtCounter)));
                        _CHK(pType->setAttribute(L"r0", _variant_t(m_taskList[i]->runtimeId[0])));
                        _CHK(pType->setAttribute(L"r1", _variant_t(m_taskList[i]->runtimeId[1])));
                        _CHK(pType->setAttribute(L"r2", _variant_t(m_taskList[i]->runtimeId[2])));
                        _CHK(pType->setAttribute(L"r3", _variant_t(m_taskList[i]->runtimeId[3])));
                        _CHK(pType->setAttribute(L"r4", _variant_t(m_taskList[i]->runtimeId[4])));
                        _CHK(pType->setAttribute(L"r5", _variant_t(m_taskList[i]->runtimeId[5])));
                        _CHK(pType->setAttribute(L"r6", _variant_t(m_taskList[i]->runtimeId[6])));
                        _CHK(pType->setAttribute(L"r7", _variant_t(m_taskList[i]->runtimeId[7])));
                        _CHK(pType->setAttribute(L"r8", _variant_t(m_taskList[i]->runtimeId[8])));
                        _CHK(pType->setAttribute(L"r9", _variant_t(m_taskList[i]->runtimeId[9])));
                    }
                    break;
                case stActionDescriptor::MY_AT_PRINT:
                    _CHK(pXMLDom->createElement(L"Print", &pType));
                    _CHK(pType->setAttribute(L"type", _variant_t(m_taskList[i]->system_type)));
                    break;
                default:
                    throw false; 
                }
                _CHK(format(pXMLDom, pAction, 3));
                _CHK(pAction->appendChild(pType, NULL));
                _CHK(format(pXMLDom, pAction, 2));
            }
        }
        // add helpers
        for (int i=0; i<(int)g_widgets.m_widgetList.size(); i++) {
            if (g_widgets.m_widgetList[i].title[0]) {
                if (pHS==NULL) {
                    _CHK(pXMLDom->createElement(L"HelperList", &pHS));
                    _CHK(format(pXMLDom, pRC, 0));
                    _CHK(pRC->appendChild(pHS, NULL));
                    CComPtr<IXMLDOMElement> pGroup;
                    _CHK(format(pXMLDom, pHS, 1));
                    _CHK(pXMLDom->createElement(L"Group", &pGroup));
                    _CHK(pGroup->setAttribute(L"x", _variant_t(g_widgets.m_lastWinInfo.rcWindow.left)));
                    _CHK(pGroup->setAttribute(L"y", _variant_t(g_widgets.m_lastWinInfo.rcWindow.top)));
                    _CHK(pHS->appendChild(pGroup, NULL));
                }
                CComPtr<IXMLDOMElement> pHelper;
                _CHK(format(pXMLDom, pHS, 1));
                _CHK(pXMLDom->createElement(L"Helper", &pHelper));
                _CHK(pHelper->setAttribute(L"name", _variant_t(g_widgets.m_widgetList[i].title)));
                _CHK(pHelper->setAttribute(L"x", _variant_t(g_widgets.m_widgetList[i].rect.left)));
                _CHK(pHelper->setAttribute(L"y", _variant_t(g_widgets.m_widgetList[i].rect.top)));
                _CHK(pHelper->setAttribute(L"pin", _variant_t(g_widgets.m_widgetList[i].pin)));
                _CHK(pHelper->setAttribute(L"active", _variant_t(g_widgets.m_widgetList[i].active)));
                _CHK(pHS->appendChild(pHelper, NULL));
            }
        }
        if (pHS) {_CHK(format(pXMLDom, pHS, 0));}
        _CHK(format(pXMLDom, pTS, 0));
        _CHK(format(pXMLDom, pRC, 0));
        _CHK(pXMLDom->save(_variant_t(fileName)));
    }
    catch(...) {
        ret_code = FALSE;
    }
    CoUninitialize();
    m_wasFileLoad = TRUE;
    return ret_code;
#undef _CHK 
}

BOOL CDispatcher::SaveActionList(CString* name)
{
    // select a file
    CString path;
    WCHAR* fileName = NULL;
    if (name==NULL) {
        CFileDialog dialog(0, L".xml", NULL, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter);
        if (dialog.DoModal()!=IDOK) return false;
        path = dialog.GetFolderPath();
        path.AppendChar('\\');
        path.Append(dialog.GetFileName());
        fileName = path.GetBuffer();
    } else {
        fileName = name->GetBuffer();
    }
    if (!fileName) return FALSE;
    WCHAR fName[_MAX_FNAME];
    WCHAR fExt[_MAX_EXT];
    _wsplitpath_s(fileName, NULL,0, NULL,0, fName,_MAX_FNAME, fExt,_MAX_EXT);
    return SaveXML(fileName);
}

BOOL CDispatcher::NewActionList(void)
{
    Release();
    return true;
}

HWND CDispatcher::GetWindow(LPCWSTR className, LPCWSTR windowName)
{
    HWND hWnd = NULL;
    hWnd = ::FindWindow( className, windowName);
    if (!IsWindow(hWnd)) return NULL;

    return hWnd;
}

BOOL CDispatcher::UpdateProcessList() 
{
    BOOL ret = TRUE;
    IUIAutomation* automation = g_Invoker.GetAutomation();
    if (!automation) return FALSE;

    if(m_processList) m_processList->Release();
    m_processList = NULL;

    IUIAutomationElement* root;
    if (FAILED(automation->GetRootElement(&root) || !root)) {
        return FALSE;
    }

    IUIAutomationCondition* cond;
    automation->CreateTrueCondition(&cond);
    if (cond) {
        root->FindAll(TreeScope_Children, cond, &m_processList);
        cond->Release();
    }
    root->Release();

    // update hwnd map
    int count = (int)m_taskList.size();
    for (int i=0; i<count; i++) m_taskList[i]->hWnd = (HWND)1;
    for (int i=0; i<count; i++) {
        if (m_taskList[i]->hWnd != (HWND)1) continue;
        HWND _hWnd = NULL;
        if (m_taskList[i]->isGrouping)  _hWnd = GetInstance(m_taskList[i]->windowName);
        else _hWnd = GetWindow( m_taskList[i]->className, m_taskList[i]->windowName);
        if (!IsWindowVisible(_hWnd)) _hWnd = NULL;
        m_taskList[i]->hWnd = _hWnd;
        // find the same app in the list
        for (int j=i+1; j<count; j++) {
            if (m_taskList[j]->hWnd != (HWND)1) continue;
            if (m_taskList[j]->isGrouping) {
                if (_wcsicmp(m_taskList[i]->windowName, m_taskList[j]->windowName)==0) m_taskList[j]->hWnd = _hWnd;
            } else if ( (_wcsicmp(m_taskList[i]->windowName, m_taskList[j]->windowName)==0)
                        && (_wcsicmp(m_taskList[i]->className, m_taskList[j]->className)==0)) m_taskList[j]->hWnd = _hWnd;
        }
    }

    return m_processList != NULL;
}

HWND CDispatcher::GetInstance(LPCWSTR appName)
{
    HWND hWnd = NULL;

    if (!m_processList) return hWnd;

    int in = 0;
    m_processList->get_Length(&in);
    for (int i=0; i<in; i++) {
        IUIAutomationElement* el = NULL;
        if (SUCCEEDED( m_processList->GetElement(i, &el) ) && el ) {
            el->get_CurrentName(&m_tmpStr1);
            if (SysStringLen(m_tmpStr1)) {
                stActionDescriptor* app = NULL;
                HANDLE hProcess = NULL;
                int pid;
                el->get_CurrentProcessId(&pid);
                hProcess = OpenProcess( PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid );
                if (hProcess) {
                    if (GetProcessImageFileName(hProcess, m_tmpStr2, MAX_PATH)) {
                        WCHAR filename[_MAX_FNAME];
                        _wsplitpath_s(m_tmpStr2, NULL,0, NULL,0, filename,_MAX_FNAME, NULL,0);

                        if (wcscmp(filename, appName)==0) {
                            el->get_CurrentNativeWindowHandle((UIA_HWND*)&hWnd);
                            hWnd = FindWindow(NULL, m_tmpStr1);
                            if (IsWindow(hWnd)) break;
                        }
                    }
                    CloseHandle(hProcess);
                }
            }
        }
    }
    return hWnd;
}
