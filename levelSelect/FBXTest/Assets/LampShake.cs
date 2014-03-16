using UnityEngine;
using System.Collections;

public class LampShake : MonoBehaviour {
	
	float moveSpeed = 1.0f;
	int rotationCounter = 0;
	int switchRotationAfter = 3;
	bool moveRight = true;
	
	float lampIntensity = 0.5f;
	float randVal = 0.1f;
	
	// Use this for initialization
	void Start () {
		
	}
	
	// Update is called once per frame
	void Update () {
		
		rotationCounter++;
		
		if(rotationCounter >= switchRotationAfter) {
			randVal = Random.value;
			
			
			if(randVal > 0.9 ) {
				
				rotationCounter = 0;
				//moveRight = !moveRight;
				moveSpeed *= -1;
				//switchRotationAfter = (int)(Random.value * 20);
				lampIntensity = Random.value;
				
				light.intensity = lampIntensity;
				
			}
			
		
		}
		
		//transform.Translate(Vector3.right * moveSpeed * Time.deltaTime);
		
		/*if(moveRight == false) {
			transform.Translate(Vector3.right * moveSpeed);
		} else {
			transform.Translate(Vector3.left * moveSpeed);
		}*/
		
	}
}
