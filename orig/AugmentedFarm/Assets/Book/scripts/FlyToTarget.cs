/*******************************************************************************

INTEL CORPORATION PROPRIETARY INFORMATION
This software is supplied under the terms of a license agreement or nondisclosure
agreement with Intel Corporation and may not be copied or disclosed except in
accordance with the terms of that agreement
Copyright(c) 2012-2013 Intel Corporation. All Rights Reserved.

*******************************************************************************/
using UnityEngine;
using System.Collections;

public class FlyToTarget : MonoBehaviour {
	
	public Vector3 target = new Vector3(0,0,0);	
	public float step = 0.01f;
	
	// Use this for initialization
	void Start () {	
		
	}
	
	// Update is called once per frame
	void Update () {
		Vector3 pos = transform.position;
		Vector3 dir =(target-pos).normalized;			
		transform.position = Vector3.MoveTowards(transform.position, target, step);						
		transform.eulerAngles = new Vector3(0 ,Mathf.Atan2(dir.x, dir.z)*Mathf.Rad2Deg , 0);	    						
	}
	
	
}
