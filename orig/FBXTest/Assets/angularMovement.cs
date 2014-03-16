using UnityEngine;
using System.Collections;

public class angularMovement : MonoBehaviour {

	// Use this for initialization
	void Start () {
	
	}
	
	// Update is called once per frame
	void Update () {
		
		
		transform.Translate(Time.deltaTime, Time.deltaTime, 0);	
		transform.Translate(-1 * Time.deltaTime, -1 * Time.deltaTime, 0);	
		
		
	}
}
