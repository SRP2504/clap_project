using UnityEngine;
using System.Collections;

public class cubehandle : MonoBehaviour {
	public GameObject cam, left, right;
	public float start;
	public bool orig, rightb, bothb, leftd, rightd, bothd, leftdt, rightdt, bothdt;
	public float scale, temp;
	public Vector3 campos;
	public Vector3 rightpos;
	public Vector3 leftpos;
	public l3options op;
	public l3shadowhand shadow;
	int cnt ;
	// Use this for initialization
	void Start () {
		cnt = 0;
		cam = GameObject.Find("Main Camera");
		left = GameObject.Find("leftCube");
		right = GameObject.Find("rightCube");
		start = Time.time;
		scale = 3.0f;
		orig = true;
		rightb = true;
		bothb = true;
		//op.updatecounter(0);
		//rightahead();
		//leftahead();
		left.gameObject.renderer.material.color = Color.red;
		right.gameObject.renderer.material.color = Color.red;
		leftahead();
		//rightahead();
		leftd = true;
		rightd = false;
		bothd = false;
	//	original();
	}
	
	// Update is called once per frame
	void Update () {
		print(Time.time);
		if((Time.time >= start + scale && orig) || (leftdt) || (rightdt) || (bothdt)){
			original();
			scale = Time.time + 5.0f;	
			orig = false;
			leftd = false;
			rightd = false;
			bothd = false;
			leftdt = false;
			rightdt = false;
			bothdt = false;
		}
		
		if(Time.time >= (start + 5.0f) && rightb) {
			rightahead();
			left.gameObject.renderer.material.color = Color.red;
			right.gameObject.renderer.material.color = Color.red;
			rightb = false;
			orig = true;
			rightd = true;
		}
		
		if(Time.time >= (start + 8.0f) && bothb){
			bothahead();
			left.gameObject.renderer.material.color = Color.red;
			right.gameObject.renderer.material.color = Color.red;
			bothb = false;
			orig = true;
		
		}
		
		if(leftd){
				if(l3shadowhand.pos1.x >= 200 && l3shadowhand.pos1.x <= 218 && l3shadowhand.pos1.y >= 80 && l3shadowhand.pos1.y <= 100){
					leftdt = true;
					cnt++;
					left.gameObject.renderer.material.color = Color.green;
				}
		}
		
		if(rightd){
				if(l3shadowhand.pos1.x >= 107 && l3shadowhand.pos1.x <= 130 && l3shadowhand.pos1.y >= 85 && l3shadowhand.pos1.y <= 107){
					rightdt = true;
					cnt++;
					right.gameObject.renderer.material.color = Color.green;
				}
		}
		
		/*if(leftd){
				if(l3shadowhand.pos1.x >= 200 && l3shadowhand.pos1.x <= 218 && l3shadowhand.pos1.y >= 80 && l3shadowhand.pos1.y <= 100){
					leftdt = true;
					left.gameObject.renderer.material.color = Color.green;
				}
		}*/
	}
	
	void rightahead(){
		campos.x = cam.transform.position.x + (1.5f);
		campos.y = cam.transform.position.y;
		campos.z = cam.transform.position.z;
		cam.rigidbody.MovePosition(campos);
	
		rightpos.x = right.transform.position.x ;
		rightpos.y = right.transform.position.y;
		rightpos.z = -4;
		right.rigidbody.MovePosition(rightpos);
	}
	
	void leftahead(){
		campos.x = cam.transform.position.x - (1.5f);
		campos.y = cam.transform.position.y;
		campos.z = cam.transform.position.z;
		cam.rigidbody.MovePosition(campos);
	
		leftpos.x = left.transform.position.x ;
		leftpos.y = left.transform.position.y;
		leftpos.z = -4;
		left.rigidbody.MovePosition(leftpos);
	}
	
	void bothahead(){
		rightpos.x = 2.5f ;
		rightpos.y = right.transform.position.y;
		rightpos.z = -4;
		right.rigidbody.MovePosition(rightpos);
	
		leftpos.x = -2.5f;
		leftpos.y = left.transform.position.y;
		leftpos.z = -4f;
		left.rigidbody.MovePosition(leftpos);
	}
	
	void original(){
		campos.x = 0;
		campos.y = cam.transform.position.y;
		campos.z = cam.transform.position.z;
		cam.rigidbody.MovePosition(campos);
	
		leftpos.x = -4f;
		leftpos.y = left.transform.position.y;
		leftpos.z = 0;
		left.rigidbody.MovePosition(leftpos);
		
		rightpos.x = 4f;
		rightpos.y = right.transform.position.y;
		rightpos.z = 0;
		right.rigidbody.MovePosition(rightpos);
	}
}
