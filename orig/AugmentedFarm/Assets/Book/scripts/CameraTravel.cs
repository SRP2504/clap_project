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
 * 	use this script on a camera
 * 
 * Description:
 * 	will move camera from origin to wanted point and back
 *********************************************************************************/


public class CameraTravel : MonoBehaviour {
	//The coordinate of the camera to be traveled to
	private Vector3 TravelVector;
	private Vector3 Origin;
	
	//Speed of camera travel [0,...,1]
	public float speed;
	public bool  isWorking = true;
	
	//Public 
	public void GOTOObject(string name){
		GameObject obj = GameObject.Find(name);
		TravelVector = (obj.transform.position - this.transform.position);
		TravelVector.Scale( new Vector3 (0.8f,0.8f,0.8f) ); 
		Origin = this.transform.position;
	}
	
	// Use this for initialization
	void Start () {
	}
	
	public bool auto = false;
	
	// Update is called once per frame
	void Update () {
		if(!isWorking){
			return;
		}
		
		if(auto){
			GOTOObject("Farmer_v02");
			auto = false;
		}

		Vector3 dir = TravelVector.normalized;
		Vector3 Delta = new Vector3(	Mathf.Max( new float[]{ Mathf.Abs(TravelVector.x) * speed * Time.deltaTime ,DELTA })*dir.x ,
										Mathf.Max( new float[]{ Mathf.Abs(TravelVector.y) * speed * Time.deltaTime ,DELTA })*dir.y ,
										Mathf.Max( new float[]{ Mathf.Abs(TravelVector.z) * speed * Time.deltaTime ,DELTA })*dir.z
									);
		print("asldjdjaklsjdkladasjdkl");
		this.transform.position += Delta ;
		TravelVector -= Delta;
		
		//End of the travel
		if( isPosSlitlyEquals( TravelVector , new Vector3(0f,0f,0f)) ){
			TravelVector = Origin - this.transform.position;
		}
		
		//end set to pos to (0,0,0)
		if(isPosSlitlyEquals( this.transform.localPosition , new Vector3(0f,0f,0f))){
			this.transform.localPosition = new Vector3(0f,0f,0f) ;
		}
	}
	
	
	
	float DELTA = 0.01f;
	bool isPosSlitlyEquals( Vector3 a , Vector3 b){
		return isFloatSlitlyEquals(a.x , b.x) && isFloatSlitlyEquals(a.y , b.y) && isFloatSlitlyEquals(a.z , b.z);
	}
	
	bool isFloatSlitlyEquals( float a, float b){
		return Mathf.Abs(a-b) < DELTA;
	}
}
