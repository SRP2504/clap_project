using UnityEngine;
using System.Collections;

public class rightcubechg : MonoBehaviour {
	
	// Use this for initialization
	void Start () {
		
	}
	
	// Update is called once per frame
	void Update () {
		
		if(ShadowHand.flag1)
			gameObject.renderer.material.color = Color.green;
		else
			gameObject.renderer.material.color = Color.red;
		
	/*	if(ShadowHand.flag && ShadowHand.flag1){
			transform.x = 2.8;
			transform.y = 2;
			transform.z = 0;
			Vector3 x1;
			x1.x = -1;
			x1.y = 0;
			x1.z = 0;
			while(ShadowHand.wave != true){
				if(transform.position.x > 2.7 && transform.position.x < 2.9)
					rigidbody.AddForce(x1 * 40);
				if(transform.position.x > -2 && transform.position.x < -1.8)
					rigidbody.AddForce(-x1 * 40);
			}
			gameObject.renderer.material.color = Color.green;
		}*/
	}
}
