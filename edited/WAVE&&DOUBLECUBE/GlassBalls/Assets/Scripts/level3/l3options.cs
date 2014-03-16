/*******************************************************************************

INTEL CORPORATION PROPRIETARY INFORMATION
This software is supplied under the terms of a license agreement or nondisclosure
agreement with Intel Corporation and may not be copied or disclosed except in
accordance with the terms of that agreement
Copyright(c) 2012 Intel Corporation. All Rights Reserved.

*******************************************************************************/
using UnityEngine;
using System.Collections;

public class l3options: MonoBehaviour {
	public 	bool 	visible;
	private string	msg;
	public static int cnt;
	
	void Awake() {
		Application.targetFrameRate=30;
		cnt = 0;
	}
	
	void OnGUI() {
		if (GUI.Button (new Rect (10,10,120,30), visible?"Hide Fingertips":"Show Fingertips")) {
			visible=!visible;
		}
		if (GUI.Button (new Rect (10,40,120,30), "Restart")) {
			Application.LoadLevel(0);
		}
		
		//GUI.Label(new Rect(600, 10, 230, 400), 
		//	msg!=null?msg:" " + cnt + " ");
		
	}
	public void SetMessage(string msg2) {
		msg=msg2;
	}
	
	public void updatecounter(int c){
		GUI.Label(new Rect(600, 10, 230, 400), 
			msg!=null?msg:" " + c + " ");	
	}
}
