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
public class ABFarmScript : MonoBehaviour {
	// Use this for initialization
	public float xShift = 0.8f;
	public float yShift = 0.0f;
	public float zShift = 0.2f;
	void Start () {
#if DEBUG
		print ("###in start AR Farm script\n");
#endif

		GameObject SDKObj = GameObject.Find("SDKPipelineObject");
		SDKObj.GetComponent<SDKPipeline>().ARFound += delegate(object sender,string name, PXCMDCVTracker.TargetData tdata){
			if(name != ABStatics.Farm_PageName){
				return;	
			}
			
			//this.transform.localPosition = new Vector3((float)(-tdata.position.x/50f),(float)(-tdata.position.y/50f)-0.15f,(float)(tdata.position.z/50f));
			//this.transform.localRotation = new Quaternion((float)-tdata.orientation.x,(float)-tdata.orientation.y,(float)tdata.orientation.z,(float)tdata.orientation.r);
			//this.transform.Rotate(90f,90f,0f);
			//this.transform.Rotate(0f,90f+180f,0f);
			//this.transform.Rotate(0f,0f,90f+180f);
			//this.transform.Translate(xShift,yShift,zShift);
			
			this.transform.localPosition = new Vector3((float)(-tdata.position.x/50f),(float)(-tdata.position.y/50f),(float)(tdata.position.z/50f));
			this.transform.localRotation = new Quaternion((float)-tdata.orientation.x,(float)-tdata.orientation.y,(float)tdata.orientation.z,(float)tdata.orientation.r);
			this.transform.Rotate(90f,0f,0);
			this.transform.Rotate(0f,90f,0f);
			this.transform.Translate(xShift,yShift,zShift);
			
			SDKObj.GetComponent<SDKPipeline>().message=
				"Mary had a little land. There were fields for crops and grass for sheep.\n\n"+
				"Mary had a little land. She enjoyed playing with her butterfly and feeding her fish.\n";
		};
	}
}
