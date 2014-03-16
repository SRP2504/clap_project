/*******************************************************************************

INTEL CORPORATION PROPRIETARY INFORMATION
This software is supplied under the terms of a license agreement or nondisclosure
agreement with Intel Corporation and may not be copied or disclosed except in
accordance with the terms of that agreement
Copyright(c) 2012 Intel Corporation. All Rights Reserved.

*******************************************************************************/
using UnityEngine;
using System.Collections;

public class Fingertip : MonoBehaviour {
	public PXCMGesture.GeoNode.Label fingertip;
	public ShadowHand shadowHand;
	public Options options;
	public PXCMGesture.GeoNode.Side side;
	
	void Start() {
		StayAway();		
	}
	
	void FixedUpdate () {
		if (shadowHand.handData[(int)side-1][(int)fingertip].body>0) {
			float radius=(fingertip==PXCMGesture.GeoNode.Label.LABEL_ANY)?1:shadowHand.handData[(int)side-1][(int)fingertip].radiusImage/18;
			transform.localScale=new Vector3(radius,radius,radius);
			
			rigidbody.MovePosition(shadowHand.MapCoordinates(shadowHand.handData[(int)side-1][(int)fingertip].positionImage));
			renderer.enabled=options.visible;
			rigidbody.detectCollisions=true;
		} else {
			StayAway();
		}
	}
			
	private void StayAway() {
		rigidbody.detectCollisions=false;
		renderer.enabled=false;
	}
}
