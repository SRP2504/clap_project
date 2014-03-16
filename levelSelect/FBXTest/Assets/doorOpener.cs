using UnityEngine;
using System.Collections;

public class doorOpener : MonoBehaviour {

	// Use this for initialization
	void Start () {
	
	}
	
	// Update is called once per frame
	void Update () {
		
		//If o key is pressed, open the door
		if(Input.GetKeyDown("o")) {
			
			//Vector3.Lerp(transform.position, transform.eulerAngles.y + 45.0f, 0.5f);
			transform.Rotate(0.0f, 45.0f, 0.0f);
			
			//newRotation = new Quaternion.LookRotation(Camera.main.transform.position - transform.position).eulerAngles;
			
		    
		    
		}
	}
}
