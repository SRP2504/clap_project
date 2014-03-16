using UnityEngine;
using System.Collections;

public class controlling : MonoBehaviour {
	
	GameObject ExitText;
	public ShadowHand sh;
	bool exit = false;
	// Use this for initialization
	void Start () {
		ExitText = GameObject.Find("ExitText");
	}
	
	// Update is called once per frame
	void Update () {
		ExitText.SetActive(exit);
		if(sh.righttop)
			exit= true;
		
	}
}
