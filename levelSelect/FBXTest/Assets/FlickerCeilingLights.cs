using UnityEngine;
using System.Collections;

public class FlickerCeilingLights : MonoBehaviour {

	// Use this for initialization
	void Start () {
	
	}
	
	// Update is called once per frame
	void Update () {
		//if ((Random.value * 10) % 2 == 0) {
			light.range = 6 + (Random.value * 10);	
		//}
		
	}
}
