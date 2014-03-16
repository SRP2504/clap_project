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
	public  Options options;
	public PXCMPoint3DF32 pos1, pos2, pos3, pos4;
	public uint i = 0;
	//public uint j = 0;
	//public PXCMGesture.GeoNode.Openness open;
	
    void Start() {
		pp=new PXCUPipeline();
		
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
						pixels[yy2+(handImage.width-1-x)]=new Color32(0,0,0,(byte)((pixel==labels[1] || pixel==labels[2])?120:0));
					}
				}
		        handImage.SetPixels32 (pixels, 0);
				handImage.Apply();
				
				pp.QueryGeoNode(PXCMGesture.GeoNode.Label.LABEL_BODY_HAND_SECONDARY,handData[0]);
				pp.QueryGeoNode(PXCMGesture.GeoNode.Label.LABEL_BODY_HAND_PRIMARY,handData[1]);
				
				pos1 = handData[0][1].positionImage;
				pos2 = handData[1][1].positionImage;
				//pos3 = handData[0][2].positionImage;
				//pos4 = handData[1][2].positionImage;
				
				if(pos1.x - pos2.x < 30 && pos1.x - pos2.x > -30 && (pos1.x!=0 && pos2.x!=0 )){
					audio.Play();
					//print ("clapped" + i++);
				}
				
				//if(pos3.x - pos4.x < 30 && pos3.x - pos4.x > -30 && (pos3.x!=0 && pos4.x!=0 )){
				//	audio.Play(
				
					//print ("clapped" + i++);
				//}
						
		
				//open = handData[0][1].opennessState;
				//for(int i = 0; zi < 2; i++){
				//	for(int j = 0; j < 9; j++){
				//if(handData[0][0].openness < 20 && handData[0][0].openness != 0)
				//	print ("clicked" + i++);
				//if(handData[1][0].openness < 20 && handData[1][0].openness != 0)
				//	print ("clicked" + i++);
						//print (handData[0][0].openness + " Left");
				print (handData[1][0].openness + " Right");
					//}
				//}
				pp.ReleaseFrame();
			}
		}
	}
	
	public Vector3 MapCoordinates(PXCMPoint3DF32 pos1) {
		Vector3 pos2=camera2.ViewportToWorldPoint(new Vector3((float)(handImage.width-1-pos1.x)/handImage.width,(float)(handImage.height-1-pos1.y)/handImage.height,0));
		pos2.z=0;
		return pos2;
	}
	
	private void ZeroImage(Texture2D image) {
		Color32[] pixels=image.GetPixels32(0);
		for (int x=0;x<image.width*image.height;x++) pixels[x]=new Color32(255,255,255,128);
	    image.SetPixels32(pixels, 0);
		image.Apply();
	}
}
