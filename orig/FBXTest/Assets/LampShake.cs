using UnityEngine;
using System.Collections;

public class LampShake : MonoBehaviour {
	
	//float moveSpeed = 1.0f;
	int rotationCounter = 0;
	int switchRotationAfter = 3;
	//bool moveRight = true;
	
	float lampIntensity = 0.5f;
	float randVal = 0.1f;
	GameObject doorlight;
	
	float i;
	float j;
	Vector3 f, t;
	
	// Use this for initialization
	void Start () {
		doorlight = GameObject.Find("doorKnobHighlighter");
		
		i = 0;
		j = 2;
		f.x = 0;
		f.y = 0;
		f.z = 0;
		t.x = 70;
		t.y = 0;
		t.z = 0;
	}
	
	// Update is called once per frame
	void Update () {
		
		rotationCounter++;
		
		if(rotationCounter >= switchRotationAfter) {
			randVal = Random.value;
			
			//Debug.Log(randVal);
			
			if(randVal > 0.9 ) {
				
				rotationCounter = 0;
				//moveRight = !moveRight;
				//moveSpeed *= -1;
				//switchRotationAfter = (int)(Random.value * 20);
				lampIntensity = Random.value;
				
				light.intensity = lampIntensity;
			
				//Debug.Log(lampIntensity);	
			}
			
			doorlight.light.intensity = 8;
			doorlight.light.spotAngle = i;
			/*if(Vector3.Lerp(t , f, Time.time).x >= 70 || Vector3.Lerp(t , f, Time.time).x <= 1){
				Vector3 temp = t;
				t = f;
				f = temp;
			}*/
			//print(Vector3.Lerp(f , t, Time.deltaTime).x);
			//print(Vector3.Lerp(f , t, Time.deltaTime).y);
			//print(Vector3.Lerp(f , t, Time.deltaTime).z);
				i = i + j;
				if(i >=60)
					j = -1f;
				if(i <= 20)
					j = 1f;
		}
		
		//transform.Translate(Vector3.right * moveSpeed * Time.deltaTime);
		
		/*if(moveRight == false) {
			transform.Translate(Vector3.right * moveSpeed);
		} else {
			transform.Translate(Vector3.left * moveSpeed);
		}*/
		
	}
}
