using UnityEngine;
using System.Collections;
using System.IO;

public class RunnerControl : MonoBehaviour {
	float moveSpeed = 1.5f;
	float moveSpeed2 = 0.9f;
	public ShadowHand sh;
	float doorOpenSmoothness = 0.3f;
	float runShakeFactor = 0.05f;
	bool rotateClockwise = true, againMainCamera = false;
	int rotationCounter = 0;
	int switchRotationAfter = 10;
	public bool secondcameraactive = false, game = false, boolforfirstanim = true, boolforsecanim = false, boolforthird = false, dooropen = false;
	bool superForOpen = false;
	public GameObject door, mainCamera, doorRotator;
	public GameObject secondCamera;
	GameObject GameTitle;
	
	bool isRunning = true;		//Sets whether the character is running or no
	//float doorPosition = 10;		//Position of the door, to stop the character from running

	// Use this for initialization
	void Start () {
		doorRotator = GameObject.Find("doorRotator");
		secondCamera = GameObject.Find("secondaryCamera");
		door = GameObject.Find("door");
		mainCamera = GameObject.Find("Main Camera");
		GameTitle = GameObject.Find("GameTitle");
		//handCamera = GameObject.Find("handCamera");
		//secondCamera.gameObject.SetActive(false);
	}
	
	// Update is called once per frame
	void Update () {
		secondCamera.SetActive(secondcameraactive);
		GameTitle.SetActive(game);
		//handCamera.SetActive(handcameraactive);
		if(isRunning) {
			rotationCounter++;
			if(rotationCounter >= switchRotationAfter) {
				rotationCounter = 0;
				rotateClockwise = !rotateClockwise;
			}
			
			if(rotateClockwise == false) {
				//transform.renderer.material.color = Color.blue;
				
				mainCamera.transform.Rotate(Vector3.forward, runShakeFactor);	
			} else{
				
				mainCamera.transform.Rotate(Vector3.back, runShakeFactor);
			}
			
			
			//transform.Translate(Vector3.forward * moveSpeed * Time.deltaTime);
				
		}
		
		
		/*if(camera.transform.localPosition.z >= doorPosition || camera.transform.localRotation.z <= 0) {
			isRunning = false;
		}*/
		//Debug.Log("dt " + Input.GetAxis("Horizontal"));
		//print("f" + boolforfirstanim + " s" + boolforsecanim + " t" + boolforthird);
	
			Vector3 cameraPosition = mainCamera.transform.localPosition;
			//print(cameraPosition.z);
		
			if(cameraPosition.z <= -11.0f && boolforfirstanim){
				mainCamera.transform.Translate(0, 0, moveSpeed * Time.deltaTime);
			}
			else if(boolforfirstanim) {
				//play first anim
				//animation.Play("bear_8.gif");
				//print ("delay1");
				boolforfirstanim = false;
				boolforsecanim = true;
			}
			
			if(cameraPosition.z <= -7.0 && boolforsecanim){
				mainCamera.transform.Translate(0, 0, moveSpeed * Time.deltaTime);
			}
			else if(boolforsecanim && !boolforfirstanim){
				//play second anim
				//print ("delay2");
				boolforsecanim = false;	
				boolforthird = true;
			}
			
			
			if(cameraPosition.z <= 1f && boolforthird){
				mainCamera.transform.Translate(0, 0, moveSpeed * Time.deltaTime);
			}else if(boolforthird && !boolforfirstanim && !boolforsecanim){
				
				secondCamera.SetActive(true);
				mainCamera.SetActive(false);
				secondcameraactive = true;
				boolforthird = false;
				dooropen = true;
				//handcameraactive = true;
				//print("setup");
			}
		
		if(dooropen){
			//againMainCamera = true;
			PXCMPoint3DF32 pos, pos0;
			pos = sh.handData[1][1].positionImage;
			pos0 = sh.handData[0][1].positionImage;
			print (pos0.x + " " + pos0.y + " " + pos0.z);
			if(superForOpen || (pos.x > 215 && pos.x < 240 && pos.y > 95 && pos.y < 115)){
			//if(true){
				Quaternion e = Quaternion.Euler(0, 90, 0);
				doorRotator.transform.localRotation = Quaternion.Slerp(doorRotator.transform.localRotation, e, doorOpenSmoothness * Time.deltaTime);
				superForOpen = true;
				if(doorRotator.transform.rotation.y >= 0.65f){
					Application.LoadLevel("gameName");
					againMainCamera = true;	
					dooropen = false;
					secondcameraactive = false;
					//print("Active");
					//secondCamera.SetActive(false);
					//mainCamera.SetActive(true);
					superForOpen = false;
					//GameTitle.SetActive(true);
					//game = true;
				}
			}
		}
		/*if(Input.GetKeyDown(KeyCode.W)) {
			transform.Translate(Vector3.forward * moveSpeed * Time.deltaTime);	
			
		}
		if(Input.GetKeyDown(KeyCode.S)) {
			transform.Translate(Vector3.back * moveSpeed * Time.deltaTime);	
		}*/
		/*if(Input.GetMouseButtonDown(0) == true) {
			//camera.transform.Rotate(0, 0, 0);
			moveSpeed = moveSpeed * -1;
			isRunning = true;
		}*/
		
		if(againMainCamera){
			if(cameraPosition.z <= 8.0f && againMainCamera){
				mainCamera.transform.Translate(0, 0, moveSpeed * Time.deltaTime);
				//GameTitle.transform.Translate(0,0,moveSpeed2 * Time.deltaTime);
				//game = true;
				//mainCamera.camera.depth = 5;
			}
			else{
				//againMainCamera = false;
				Application.LoadLevel("gameName");
				//againMainCamera = false;		
			}
		}
	}

}
