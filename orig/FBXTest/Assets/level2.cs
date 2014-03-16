using UnityEngine;
using System.Collections;

public class level2 : MonoBehaviour {
	float start;
	// Use this for initialization
	void Start () {
		start = Time.time;
	}
	
	// Update is called once per frame
	void Update () {
		start = Time.time;
		if(start == 2.0f)
			print ("print");
	}
}
