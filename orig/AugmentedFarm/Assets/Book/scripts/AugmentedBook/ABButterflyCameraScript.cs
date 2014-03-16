/*******************************************************************************

INTEL CORPORATION PROPRIETARY INFORMATION
This software is supplied under the terms of a license agreement or nondisclosure
agreement with Intel Corporation and may not be copied or disclosed except in
accordance with the terms of that agreement
Copyright(c) 2012-2013 Intel Corporation. All Rights Reserved.

*******************************************************************************/
#define DEBUG
using UnityEngine;
using System.Collections.Generic;
using System;


/********************************************************************************
 * Usage:
 * 	use this script on the object that going to move according to the tracked data
 *********************************************************************************/


public class ABButterflyCameraScript : MonoBehaviour {
	// Use this for initialization
	
	private int MaxWaitingTime = 100000; //in ms
	private DateTime lastSeen;		
	
	void Start () {
#if DEBUG
			print ("###in start ABButterflyCameraScript\n");
#endif

		GameObject SDKObj = GameObject.Find("SDKPipelineObject");
		
		//Disable the camera
		GameObject.Find("AugmentedButterflyCamera").camera.enabled = false;
		SDKObj.GetComponent<SDKPipeline>().ARFound += delegate(object sender,string name, PXCMDCVTracker.TargetData tdata){
			lastSeen = DateTime.Now;
			
			// Turn On/Off virtual AR Butterfly Camera when recognizing Butterfly Page			
			if( (name == ABStatics.Butterfly_PageName) ){
				GameObject.Find("AugmentedButterflyCamera").camera.enabled = true;	
			}else{
				GameObject.Find("AugmentedButterflyCamera").camera.enabled = false;
			}
		};
	}
	
	void OnGUI() {
		
	}

	// Update is called once per frame
	void Update () {
		if(lastSeen < DateTime.Now.Subtract( new TimeSpan(0,0,0,0,MaxWaitingTime)) ){
			GameObject.Find("AugmentedButterflyCamera").camera.enabled = false;
		}
	}
}
