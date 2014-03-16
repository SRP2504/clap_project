using UnityEngine;
using System.Collections;

public class leftcubechg : MonoBehaviour {
	//public ShadowHand sh;
	//PXCMPoint3DF32 pos1, pos2;
	// Use this for initialization
	void Start () {
		//gameObject.renderer.material.color = Color.green;
	
	}
	
	// Update is called once per frame
	void Update () {
		//pos1 = sh.handData[0][9].positionImage;
		//pos2 = sh.handData[1][9].positionImage;
		if(ShadowHand.flag)
			gameObject.renderer.material.color = Color.green;
		else
			gameObject.renderer.material.color = Color.red;
		
		if(ShadowHand.flag && ShadowHand.flag1){
			Application.LoadLevel("handwave");
		}
	}
}
