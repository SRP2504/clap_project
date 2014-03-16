using UnityEngine;
using System.Collections;

public class rotateTree : MonoBehaviour {
	
	//Expose in the inspector
	public float minAngle = 0.0f;
	public float maxAngle = 2.0f;
	public float rotationFrequencySeed = 0.95f;
	
	// Use this for initialization
	void Start () {
	
	}
	
	// Update is called once per frame
	void Update () {
		
		//If a random value between 0 and 100 is divisible by rotationFreqSeed, rotate the tree
		//rotationFreqSeed should ideally be a prime number, so that rotations are not frequent
		
		if(Random.value >= rotationFrequencySeed) {
			
			Debug.Log("Moving now");
			
	        Quaternion target = Quaternion.Euler(2.0f * maxAngle, 0, 0);
			
	        transform.rotation = Quaternion.Slerp(transform.rotation, target, Time.deltaTime * 2.0f);
			
		}
		
	}
}
