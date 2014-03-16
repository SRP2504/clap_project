/*******************************************************************************

INTEL CORPORATION PROPRIETARY INFORMATION
This software is supplied under the terms of a license agreement or nondisclosure
agreement with Intel Corporation and may not be copied or disclosed except in
accordance with the terms of that agreement
Copyright(c) 2012 Intel Corporation. All Rights Reserved.

*******************************************************************************/
using UnityEngine;
using System.Collections;

public class Hand : MonoBehaviour {
	public PXCMGesture.GeoNode.Label hand;
	public ShadowHand shadowHand;
	public Options options;
	public PXCMGesture.GeoNode.Side side;
	
	void Start() {
		StayAway();		
	}
	void OnCollisionEnter(Collision collision){
		audio.Play();
		Debug.Log("collision enter");
	}
	void OnCollisionStay(Collision collision){
		Debug.Log("collision stay");
	}
	void OnCollisionExit(Collision collision){
		audio.Stop();
		Debug.Log("collision exit");
	}
	void FixedUpdate () {
		if (shadowHand.handData[(int)side-1][(int)hand].body>0) {
			float radius=(hand==PXCMGesture.GeoNode.Label.LABEL_HAND_LOWER)?1:shadowHand.handData[(int)side-1][(int)hand].radiusImage/18;
			transform.localScale=new Vector3(radius,radius,radius);
			
			rigidbody.MovePosition(shadowHand.MapCoordinates(shadowHand.handData[(int)side-1][(int)hand].positionImage));
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
