using UnityEngine;
using System.Collections;

public class cloudMovement : MonoBehaviour {
	
	public float speed;
	
	// Use this for initialization
	void Start () {
		
		
	}
	
	// Update is called once per frame
	void Update () {
		//GUI.depth = -5;
		transform.Translate(Time.deltaTime * speed, 0, 0);
		if(transform.position.x > 1.0f) {
			transform.Translate(-2.0f, 0, 0);
		}
	}
	
	void OnGUI() {
		
		//GUI.depth = -5;
	}
	
}