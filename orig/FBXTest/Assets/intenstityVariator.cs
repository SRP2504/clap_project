using UnityEngine;
using System.Collections;

public class intenstityVariator : MonoBehaviour {
	
	float lightIntensity = 0.0f;
	float rVal = 0.0f;
	// Use this for initialization
	void Start () {
	
	}
	
	// Update is called once per frame
	void Update () {
		
		//flicker the light
		rVal = Random.value;
		
		lightIntensity += ((rVal > 0.5f) ? (rVal * -1) : (rVal));
		
		if(lightIntensity <=0 || lightIntensity >= 1) {
			lightIntensity = 0.5f;
		}
		light.intensity = lightIntensity * 20;
		
		
	}
}
