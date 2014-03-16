using UnityEngine;
using System.Collections;

public class eyesBlink : MonoBehaviour {
	
	float eyeIntensity;
	
	// Use this for initialization
	void Start () {
		eyeIntensity = 8.0f;
	}
	
	// Update is called once per frame
	void Update () {
		
		eyeIntensity = Random.value * 80;
		//intensity = Mathf.Abs(intensity);
		light.intensity = eyeIntensity;
	}
}
