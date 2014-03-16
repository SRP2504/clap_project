using UnityEngine;
using System.Collections;

public class cubeMove : MonoBehaviour {
	
	public int mul;
	public GameObject go;
	int cnt, flag1;
	//public shadowhandforcube shadow;
	// Use this for initialization
	void Start () {
		mul = 60;
		flag1 = 0;
		cnt = 0;
	}
	
	// Update is called once per frame
	void Update () {
		print(Time.time);
		Vector3 flag;
		flag.x = -1;
		flag.y = 0;
		flag.z = 0;
		Vector3 pos;
		pos.x = 0.5f;
		pos.y = 2f;
		pos.z = 0f;
		//print (shadowhandforcube.wave);
		if(transform.position.x >= 4.9 && (!shadowhandforcube.wave)){
			rigidbody.AddForce(flag * mul);
		}
		if(transform.position.x <= -4.9 && (!shadowhandforcube.wave)){
			rigidbody.AddForce(-flag * mul);
		}
		if(shadowhandforcube.wave){
			//rigidbody.Sleep();
			go = GameObject.Find("Cube");
			go.rigidbody.Sleep();
			flag1 = 1;
			//rigidbody.MovePosition(pos);
		}
		if(flag1 == 1){
			cnt++;
			if(cnt > 65)
				Application.LoadLevel("level3");
		}
	}
}
