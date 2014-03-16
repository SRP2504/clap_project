using UnityEngine;
using System.Collections;



public class lightFlicker : MonoBehaviour {
	
	int executeCounter = 0;
	
	// Use this for initialization
	void Start () {
	
	}
	
	// Update is called once per frame
	void Update () {
		
		if(executeCounter++ > 3) {
			executeCounter = 0;
			if(Random.value > 0.8) {
				light.enabled = ! light.enabled;
			}
			
		}		
		
	}
}
