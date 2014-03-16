using UnityEngine;
using System.Collections;

public class picChange : MonoBehaviour {

	// Use this for initialization
	void Start () {
		gameObject.renderer.material.color = Color.green;
	}
	void change(){
		gameObject.renderer.material.color = Color.green;	
	}
	// Update is called once per frame
	void Update () {
	
	}
}
