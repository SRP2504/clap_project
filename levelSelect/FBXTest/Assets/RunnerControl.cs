using UnityEngine;
using System.Collections;

public class RunnerControl : MonoBehaviour {
	
	int moveSpeed = 5;
	float runShakeFactor = 0.2f;
	bool rotateClockwise = true;
	int rotationCounter = 0;
	int switchRotationAfter = 10;
	
	bool isRunning = true;		//Sets whether the character is running or no
	
	
	float startTime, journeyLength, distCovered, fracJourney;
	
	//Expose objects to allow setting positions by moving objects in the GUI
	public GameObject startMarker;
	public GameObject endMarker;
	public GameObject textTrigger;
	public GameObject perspectiveCamera;
	public GameObject raiseInstructionText;
	
	
	
	// Use this for initialization
	void Start () {
		startTime = Time.time;
		journeyLength = Vector3.Distance(startMarker.transform.position, endMarker.transform.position);
	}
	
	// Update is called once per frame
	void Update () {
		
		//transform.Translate(0, 0, Input.GetAxis ("Vertical") * moveSpeed * Time.deltaTime);
		distCovered = (Time.time - startTime) * moveSpeed;
		
		// Fraction of journey completed = current distance divided by total distance.
		fracJourney = distCovered / journeyLength;
		transform.position = Vector3.Lerp(startMarker.transform.position, endMarker.transform.position, fracJourney);
		
	
		//Set isRunning based on whether the camera is moving or not
		isRunning = ! transform.position.Equals(endMarker.transform.position);
			
		
		if(isRunning) {
			
			
			rotationCounter++;
			
			//Shake camera 	
			if(rotationCounter >= switchRotationAfter) {
				rotationCounter = 0;
				rotateClockwise = !rotateClockwise;
			}
			
			if(rotateClockwise == false) {
				transform.Rotate(Vector3.forward, runShakeFactor * Random.value);	
			} else {
				transform.Rotate(Vector3.forward, -1 * runShakeFactor * Random.value);
			}

				
		}
		
		//If camera has reached textMarker position, fade the 
		if(transform.position.z == textTrigger.transform.position.z) {
			//textTrigger.renderer.material.color.a -= 0.1;
		}
		
		
		//If camera has reached endMarker position, switch to secondary camera.
		if(transform.position.z == endMarker.transform.position.z) {
			Debug.Log("Reached end marker: CS: " + perspectiveCamera.camera.enabled);
			//camera.enabled = false;
			perspectiveCamera.camera.depth = 0;
			float ritPos = raiseInstructionText.transform.position.y;
			for(int i = 0; i < 45; i++) {
				ritPos += i;
				raiseInstructionText.transform.Translate(raiseInstructionText.transform.position.x, 
					-1 * Time.deltaTime, 
					raiseInstructionText.transform.position.z);
			}
			
			
			
		}
		
		

	}
}
