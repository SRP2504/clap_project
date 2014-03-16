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

public class ABButterflyInteraction : MonoBehaviour
{
	// Use this for initialization
	
	/// <summary>
	/// The time of holding the flame after the last gesture recognized.
	/// the time is in mSec
	/// </summary>
	public int TimeOfHolding = 1000;
	private Vector3 targetPos;	
	int handCounter = 0;
	
	// Controling butterfly size
	float butterflyScaleOnPaper = 2.0f;	
	float butterflyScaleOnHand  = 0.5f;
	
	float step = 0.5f;						// speed step
	
	float scale;				// Changing scale of butterfly
	
	void Start ()
	{
		
		scale = butterflyScaleOnPaper;	// Changing scale of butterfly - start from paper scene
		
		targetPos = new Vector3 (115, 0, 0);		
		
#if DEBUG
		print ("###in start ABButterflyInteraction\n");
#endif
#if DEBUG
		print ("assigning event to SDK");
#endif
		GameObject SDKObj = GameObject.Find ("SDKPipelineObject");
		SDKObj.GetComponent<SDKPipeline> ().ARFound += delegate(object sender,string name, PXCMDCVTracker.TargetData tdata){
			if (name != ABStatics.Butterfly_PageName) {
				return;	
			}
			
			SDKObj.GetComponent<SDKPipeline>().message=
					"Mary's butterfly is beautiful. She got red colored wings with dots along the edges.\n\n"+
					"She was not afraid to land on Mary's hand and enjoyed that.";

			if ( handCounter == 0) {
			    scale = Mathf.MoveTowards(scale, butterflyScaleOnPaper, 0.1f);
				this.transform.localScale = new Vector3(scale, scale, scale);
				Vector3 pos = new Vector3 ((float)(-tdata.position.x / scale), (float)(-tdata.position.y / scale), (float)(tdata.position.z / scale));	
			
				this.transform.localPosition = Vector3.MoveTowards (this.transform.localPosition , pos, 2f);				
				this.transform.localRotation = new Quaternion ((float)-tdata.orientation.x, (float)-tdata.orientation.y, (float)tdata.orientation.z, (float)tdata.orientation.r);
				this.transform.Rotate (0f, 90f, 0f);
				this.transform.Rotate (180f, 0f, 0f);
			}
		};
		
		SDKObj.GetComponent<SDKPipeline> ().PalmFound += delegate(object sender, int imgWidth, int imgHeight, bool isleftHand, PXCMGesture.GeoNode palm) {
			UpdateHandPosition (imgHeight, imgWidth, isleftHand, palm);
		};
	}
	
/************************************************************************
 * Privates
 **************************************************************************/
	private void MapPixPosToScenePos (int imgHeight, int imgWidth, float x, float y, out float Px, out float Py)
	{
		// Map from pixel cordinates to 3D scene cordinates
		Px = +2f - x * 4 / imgWidth;
		Py = +1.5f - y * 3 / imgHeight;
	}
				
	void UpdateHandPosition (int imgHeight, int imgWidth, bool LeftHand, PXCMGesture.GeoNode ndata_palm)
	{			
		float Px;
		float Py;
		MapPixPosToScenePos (imgHeight, imgWidth, ndata_palm.positionImage.x, ndata_palm.positionImage.y, out Px, out Py);				
		targetPos = new Vector3 (115 + Px * 2.5f, Py * 2.5f, ndata_palm.positionImage.z);
		print( ndata_palm.positionImage.z);
		if(ndata_palm.opennessState == PXCMGesture.GeoNode.Openness.LABEL_OPEN)
	    	handCounter = 20;									
	}
	
	// Update is called once per frame
	void Update ()
	{
		if(handCounter>0)
			handCounter--;		
		
		// Move butterfly
		if (handCounter>0) {
			this.transform.position = Vector3.MoveTowards (this.transform.position, targetPos, step);	
			scale = Mathf.MoveTowards(scale, butterflyScaleOnHand, 0.1f);
			this.transform.localScale = new Vector3 (scale, scale, scale);					
		}
		
	}
}
