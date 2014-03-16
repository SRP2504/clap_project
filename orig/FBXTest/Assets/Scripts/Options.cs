/*******************************************************************************

INTEL CORPORATION PROPRIETARY INFORMATION
This software is supplied under the terms of a license agreement or nondisclosure
agreement with Intel Corporation and may not be copied or disclosed except in
accordance with the terms of that agreement
Copyright(c) 2012 Intel Corporation. All Rights Reserved.

*******************************************************************************/
using UnityEngine;
using System.Collections;

public class Options: MonoBehaviour {
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
		
		/*GUI.Label(new Rect(10, 100, 230, 400), 
			msg!=null?msg:"Instructions: Show your hand(s) to the camera, "+
			"and use your fingertips to interact with the glass balls.\n\n"+
			"Challenge: Can you nudge, flick or hold the glass balls?\n\n" +
			"Tips: If all glass balls disappear (out of screen). Click restart to resume.");*/
		
	}
	public void SetMessage(string msg2) {
		msg=msg2;
	}
}
