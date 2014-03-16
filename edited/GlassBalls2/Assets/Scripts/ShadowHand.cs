/*******************************************************************************

INTEL CORPORATION PROPRIETARY INFORMATION
This software is supplied under the terms of a license agreement or nondisclosure
agreement with Intel Corporation and may not be copied or disclosed except in
accordance with the terms of that agreement
Copyright(c) 2012 Intel Corporation. All Rights Reserved.

*******************************************************************************/
using UnityEngine;
using System;
using System.Runtime.InteropServices;

public class ShadowHand: MonoBehaviour {
	private PXCUPipeline pp;
	private Texture2D handImage;
	private byte[] labelmap;
	
	public  Camera camera2;
	public  PXCMGesture.GeoNode[][] handData;
	public PXCMGesture.GeoNode.Openness a;
	public  Options options;
	
    void Start() {
		pp=new PXCUPipeline();
		print ("started");
		int[] size=new int[2]{320,240};
		if (pp.Init(PXCUPipeline.Mode.GESTURE)) {
	        pp.QueryLabelMapSize(size);
	        print("LabelMap: width=" + size[0] + ", height=" + size[1]);
			labelmap=new byte[size[0]*size[1]];
		} else {
			options.SetMessage("Failed to detect the Creative* camera. Please plugin the camera and click restart.");
		}
		handImage=new Texture2D(size[0],size[1],TextureFormat.ARGB32,false);
		ZeroImage(handImage);
		handData=new PXCMGesture.GeoNode[2][];
		handData[0]=new PXCMGesture.GeoNode[10];
		handData[1]=new PXCMGesture.GeoNode[10];
    }
    
    void OnDisable() {
		if (pp==null) return;
		pp.Dispose();
		pp=null;
    }

    void OnGUI () {
		GUI.DrawTexture(new Rect(0,0,Screen.width,Screen.height), handImage, ScaleMode.StretchToFill);
    }
	
	void Update() {
		if (pp!=null) {
			if (pp.AcquireFrame(false)) {
				options.SetMessage(pp.IsDisconnected()?"Camera unplugged? Please replugin the camera to the same USB port. Thanks.":null);
				
				int[] labels=new int[3]{0,256,256};
				pp.QueryLabelMap(labelmap,labels);
			    Color32[] pixels=handImage.GetPixels32(0);
				for (int y=0, yy1=0, yy2=(handImage.height-1)*handImage.width;y<handImage.height;y++,yy1+=handImage.width,yy2-=handImage.width) {
					for (int x=0;x<handImage.width;x++) {
						int pixel=labelmap[yy1+x];
						pixels[yy2+(handImage.width-1-x)]=new Color32(0,0,0,(byte)((pixel==labels[1] || pixel==labels[2])?160:0));
					}
				}
		        handImage.SetPixels32 (pixels, 0);
				handImage.Apply();
				PXCMPoint3DF32 pos1;
				PXCMPoint3DF32 pos2;
				pp.QueryGeoNode(PXCMGesture.GeoNode.Label.LABEL_BODY_HAND_SECONDARY,handData[0]);
				pp.QueryGeoNode(PXCMGesture.GeoNode.Label.LABEL_BODY_HAND_PRIMARY,handData[1]);
					
					pos1 = handData[0][1].positionImage;
					//for(int i = 1; i < 10; i++){
				//		a = handData[0][i].opennessState;
			//		print(a);
		//		}
					pos2 = handData[0][2].positionImage;
					print("pos1 " + pos1.x + " " + pos1.y);
					print("pos2 " + pos2.x + " " + pos2.y);
					/*if(pos2.x - pos1.x < 20 && pos2.x - pos1.x > -20 && (pos1.x != 0 && pos2.x!=0))
						print ("1");
					else
						print ("0");
				*/
				pp.ReleaseFrame();
			}
		}
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
	public Vector3 MapCoordinates(PXCMPoint3DF32 pos1) {
		Vector3 pos2=camera2.ViewportToWorldPoint(new Vector3((float)(handImage.width-1-pos1.x)/handImage.width,(float)(handImage.height-1-pos1.y)/handImage.height,0));
		pos2.z=0;
		//print(pos1.x + " " + pos1.y);
		
		return pos2;
	}
	
	private void ZeroImage(Texture2D image) {
		Color32[] pixels=image.GetPixels32(0);
		for (int x=0;x<image.width*image.height;x++) pixels[x]=new Color32(255,255,255,128);
	    image.SetPixels32(pixels, 0);
		image.Apply();
	}
}
