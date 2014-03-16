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


public class ABObject : MonoBehaviour {
	// Use this for initialization
	
	void Start () {
#if DEBUG
			print ("###in start ARObject\n");
#endif
#if DEBUG
		print("assigning event to SDK");
#endif
		GameObject SDKObj = GameObject.Find("SDKPipelineObject");		
		SDKObj.GetComponent<SDKPipeline>().ARFound += delegate(object sender,string name, PXCMDCVTracker.TargetData tdata){
			
			this.transform.localPosition = new Vector3((float)(-tdata.position.x/50f),(float)(-tdata.position.y/50f),(float)(tdata.position.z/50f));
	
			this.transform.localRotation = new Quaternion((float)-tdata.orientation.x,(float)-tdata.orientation.y,(float)tdata.orientation.z,(float)tdata.orientation.r);
			this.transform.Rotate(90f,0f,0f);
			this.transform.Rotate(0f,90f,0f);
		};
	}
	
	void OnGUI() {
		
	}

	// Update is called once per frame
	void Update () {
	
	}
}
