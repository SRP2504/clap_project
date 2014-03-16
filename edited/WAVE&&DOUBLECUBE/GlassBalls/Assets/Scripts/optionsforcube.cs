/*******************************************************************************

INTEL CORPORATION PROPRIETARY INFORMATION
This software is supplied under the terms of a license agreement or nondisclosure
agreement with Intel Corporation and may not be copied or disclosed except in
accordance with the terms of that agreement
Copyright(c) 2012 Intel Corporation. All Rights Reserved.

*******************************************************************************/
using UnityEngine;
using System.Collections;

public class optionsforcube: MonoBehaviour {
	public 	bool 	visible;
	private string	msg;
	
	void Awake() {
		Application.targetFrameRate=30;
	}
	
	void OnGUI() {
		if (GUI.Button (new Rect (10,10,120,30), visible?"Hide Fingertips":"Show Fingertips")) {
			visible=!visible;
		}
		if (GUI.Button (new Rect (10,40,120,30), "Restart")) {
			Application.LoadLevel(0);
		}
		
		GUI.Label(new Rect(200, 50, 230, 500), 
			msg!=null?msg:"Wave your hand to say HIII !!.");
		
	}
	public void SetMessage(string msg2) {
		msg=msg2;
	}
}
