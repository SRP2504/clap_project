/*******************************************************************************

INTEL CORPORATION PROPRIETARY INFORMATION
This software is supplied under the terms of a license agreement or nondisclosure
agreement with Intel Corporation and may not be copied or disclosed except in
accordance with the terms of that agreement
Copyright(c) 2012-2013 Intel Corporation. All Rights Reserved.

*******************************************************************************/
#define DEBUG
using UnityEngine;
using System;
using System.Runtime.InteropServices;
using System.Collections.Generic;

/*****************************************************************************
 * Assign SDK script to an empty GameObject 
 * 
 * Usage Tracker example:
 * 		GameObject SDKObj = GameObject.Find("Object");
 *		SDKObj.GetComponent<SDKPipeline>().ARFound += (sender, tdata) => {.....};
 *******************************************************************************/
public class SDKPipeline : MonoBehaviour
{
	/***************************************************************************
	 * Events
	 ***************************************************************************/
	//EVENTS AR
	//will be called when the SDK will fine A Tracker object in the camera
	public delegate void ARObjectFoundEventHandler (object sender,string name,PXCMDCVTracker.TargetData tdata);

	public event ARObjectFoundEventHandler ARFound;
	
	//EVENTS Hand	
	public delegate void PalmFoundEventHandler (object sender,
												int imgWidth,
												int imgHeight,
												bool isleftHand,
												PXCMGesture.GeoNode palm);
	public event PalmFoundEventHandler PalmFound;
	
	//EVENTS Accelerometer
	//accelData[0] = x
	//accelData[1] = y
	//accelData[2] = z
	public delegate void AccelChangedEventHandler (object sender,float[] accelData);
	public event AccelChangedEventHandler OnAccelChanged;
	
	/***************************************************************************
	 * Public
	 ***************************************************************************/
	//The Camera will be randered on the GameObject
	public GameObject ObjectToDrawOn;
	public bool isTracking = true;
	public bool isGestureOn = true;
	public bool isHandRecognitionOn = true;
	public bool isAccelerometerOn = true;
	
	/***************************************************************************
	 * Privates
	 ***************************************************************************/
	private int rgb_width = 0;
	private int rgb_height = 0;
	private int uvm_width = 0;
	private int uvm_height = 0;
	private Texture2D m_Texture;
	private PXCUPipelineOT.Mode mode = PXCUPipelineOT.Mode.GESTURE | PXCUPipelineOT.Mode.OBJECT_TRACKING;
	private PXCUPipelineOT PipeLine;
	private float[] AccelerometerReading = new float[3];
	private float[] _m_uvmap;
	
	void Start ()
	{	
		for (;;) {
			PipeLine = new PXCUPipelineOT ();
			if (!PipeLine.Init (mode)) {
				message="No Creative* camera? Please plugin the camera; Press ESC to exit and restart the demo application.";
				break;
			}
		
			int[] size = new int[2];
			if (!PipeLine.QueryRGBSize (size)) break;
			rgb_width = size [0];
			rgb_height = size [1]; 
		
			if (!PipeLine.QueryUVMapSize (size)) break;
			uvm_width = size [0];
			uvm_height = size [1]; 
		
			_m_uvmap = new float[uvm_width * 2 * uvm_height];
			m_Texture = new Texture2D (rgb_width, rgb_height, TextureFormat.ARGB32, false);
			ObjectToDrawOn.renderer.material.mainTexture = m_Texture;
			
			if (!PipeLine.InitTracker (Application.dataPath + @"/Plugins/tracker/tracker.xml", rgb_width, rgb_height))
				break;
			
			return;
		}
		PipeLine.Close ();
		PipeLine=null;
	}
    
	void OnDisable ()
	{
		if (PipeLine==null) return;
		PipeLine.Dispose ();
		PipeLine=null;
	}
	
	void Update ()
	{
		////////////////
		// Camera 
		////////////////
		if (PipeLine==null) return;
		if (!PipeLine.AcquireFrame (false)) return;
		
		if (!PipeLine.IsDisconnected()) {
			if (PipeLine.QueryRGB (m_Texture)) m_Texture.Apply ();
			PipeLine.QueryUVMap (_m_uvmap);
			
			////////////////
			// ACCELEROMETER 
			////////////////
			if (isAccelerometerOn) 
				UpdateAccelerometer ();
			
			////////////////
			// Gesture
			////////////////
			if (isGestureOn) 
				UpdateGesture ();
			
			////////////////
			// AR Tracker 
			////////////////
			if (isTracking)
				UpdateTracker ();
			
			if (message.Contains("disconnected")) 
				message=instructions;
		} else {
			message="Camera disconnected? Please replugin the camera!";
		}	
		
		//END
		PipeLine.ReleaseFrame ();
	}
	
	//UV to RGB
	void getRGBIndex (float[] uvmap, float uvRow, float uvCol, int rgbW, int rgbH, out float rgbRow, out float rgbCol)
	{
		// Get U,V values from map
		float u_Val = uvmap [(int)uvRow * (uvm_width * 2) + (int)uvCol * 2];
		float v_Val = uvmap [(int)uvRow * (uvm_width * 2) + (int)uvCol * 2 + 1];
	                                
		// output row and col for RGB image                       
		rgbRow = (v_Val * rgbH);
		rgbCol = (u_Val * rgbW);
	}

	////////////////
	// ACCELEROMETER 
	////////////////
	private float[] _AccelSumVector = new float[3];
	//Defins that the average is on the last accelerometerAverage accelerometer readings
	private	int _AccelerometerAverage = 10;
	private int _AccelerometerCounter = 0;
	
	private void UpdateAccelerometer ()
	{
		float[] tempAccelerometerReading = new float[3];
		if (PipeLine.QueryDeviceProperty (PXCMCapture.Device.Property.PROPERTY_ACCELEROMETER_READING, tempAccelerometerReading)) {
			//average of _AccelerometerMovingAverage last accel Readings
			for (int i = 0; i < 3; i++) {
				_AccelSumVector [i] += tempAccelerometerReading [i];
			}
			_AccelerometerCounter++;
			//Average
			if (_AccelerometerCounter == _AccelerometerAverage) {
				_AccelerometerCounter = 0;
				for (int i = 0; i < 3; i++) {
					AccelerometerReading [i] = _AccelSumVector [i] / (float)_AccelerometerAverage;
					_AccelSumVector [i] = 0;
				}
				//Call the Event
				if (OnAccelChanged != null) {
					OnAccelChanged (this, AccelerometerReading);
				}
			}
		}
	}
	
	////////////////
	// Tracker Part
	////////////////
	void UpdateTracker ()
	{
		PXCMDCVTracker.TargetData tdata;
		for (int i = 0; i < 10; i++) {
			if (PipeLine.QueryTargetData (i, out tdata)) {
				if (tdata.status == PXCMDCVTracker.TrackingStatus.STATUS_TRACKING) {
					if (ARFound != null) {
						string name;
						PipeLine.QueryTargetName (i, out name);
						ARFound (this, name, tdata);
						print("ARFound: "+name);
					}
				}
			}
		}
	}
	
	/////////////////////
	// Gesture ,Hand Part
	/////////////////////
	void UpdateGesture ()
	{
		PXCMGesture.GeoNode ndata_palm;
		
		PXCMGesture.GeoNode.Label handSide;	// Left or Right
		bool leftHandFlag = true;
		for (int i=0; i<2; i++) {
			if (i == 0) {				
				leftHandFlag = true;
				handSide = PXCMGesture.GeoNode.Label.LABEL_BODY_HAND_LEFT;
			} else {
				leftHandFlag = false;
				handSide = PXCMGesture.GeoNode.Label.LABEL_BODY_HAND_RIGHT;
			}
			//////////////////////////////
			// Get Palm poition
			//////////////////////////////
			if (PipeLine.QueryGeoNode (handSide, out ndata_palm)) {
				//Convert to RGB index
				getRGBIndex (_m_uvmap, ndata_palm.positionImage.y, ndata_palm.positionImage.x, rgb_width, rgb_height, out ndata_palm.positionImage.y, out ndata_palm.positionImage.x);
				//CALL EVENT
				if (PalmFound != null && !float.IsInfinity (ndata_palm.positionImage.y) && !float.IsInfinity (ndata_palm.positionImage.x)) {
					PalmFound (this, rgb_width, rgb_height, leftHandFlag, ndata_palm);
				}
			}		
		}
	}
	
	private string instructions="Instructions:\n 1. Print the farm cover page or the butterfly page.\n 2. Show the page to the camera and see the farm going alive\n\nPress ESC to quit.";
	public string message;
	public GUIStyle style;
	
	void Awake() {
		Application.targetFrameRate=30;
		message=instructions;
	}
	
	void OnGUI() {
		GUI.Label(new Rect(10, 10, 230, 400), message, style);
	}
}
