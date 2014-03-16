/*******************************************************************************

INTEL CORPORATION PROPRIETARY INFORMATION
This software is supplied under the terms of a license agreement or nondisclosure
agreement with Intel Corporation and may not be copied or disclosed except in
accordance with the terms of that agreement
Copyright(c) 2012-2013 Intel Corporation. All Rights Reserved.

*******************************************************************************/
using UnityEngine;
using System.Collections;

/********************************************************************************
 * Usage:
 * 	use this script on the parent Augmented book Object
 *********************************************************************************/


public class ABObjectScript : MonoBehaviour {

	// Use this for initialization
	void Start () {
		GameObject SDKObj = GameObject.Find("SDKPipelineObject");
		SDKObj.GetComponent<SDKPipeline>().OnAccelChanged += delegate(object sender, System.Single[] accelData) {
			this.transform.localRotation = Quaternion.LookRotation( new Vector3(0*accelData[0],-accelData[2],-accelData[1]) );
		};
	}

	// Update is called once per frame
	void Update () {
	
	}
}
