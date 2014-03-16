using UnityEngine;
using System.Collections;

public class lionController : MonoBehaviour {
	
	public GameObject rabbit;
	public GameObject lion;
	public GameObject gameOverText;
	public Material lionLTRMaterial;		//Material when lion is moving left to right
	public Material lionRTLMaterial;		//Material when lion is moving right to left
	
	//convenience variables to access transform.position.x and y values
	float lionPosX, lionPosY;		
	float rabbitPosX, rabbitPosY;
	
	bool didPlayerMissClap;			//Set to true if the player misses a clap.
	
	float lionMoveFactorDefault = 1.0f;			//Move lion by these many units by default
	float lionMoveFactorOnPenalty = 2.0f;		//Move lion by these many if player misses a clap, or has a penalty
	float lionMoveToX;							//position to which the lion should move to
	float lionMoveBy;							//move lion by these many steps in one go
	int lionMoveDirection = 1;				//+1 if left to right (position.X increases). -1 if right to left (X decreases).
	
	
	// Use this for initialization
	void Start () {
		
	}
	
	// Update is called once per frame
	void Update () {
		
		//Get positions of lion and rabbit
		lionPosX = transform.position.x;
		lionPosY = transform.position.y;
		rabbitPosX = rabbit.transform.position.x;
		rabbitPosY = rabbit.transform.position.y;
		
		//Set direction in which the lion should move
		lionMoveDirection = ((rabbitPosX <= lionPosX) ? (-1) : (1));
		
		//Update lion sprite rotation based on its direction
		lion.renderer.material = ((lionMoveDirection == 1) ? (lionLTRMaterial) : (lionRTLMaterial));
		
		//Set lion move amount based on clap status and rabbit's position
		lionMoveBy = ((didPlayerMissClap) ? (lionMoveFactorOnPenalty) : (lionMoveFactorDefault));
		
		//Calculate final position x for lion
		lionMoveToX = lionPosX + (lionMoveDirection * lionMoveBy);
		
		//And move
		iTween.MoveTo(lion, iTween.Hash("name", "lionMovement", "x", lionMoveToX, "easeType", "spring"));
		
	}
	
	void OnTriggerEnter (Collider other) {
		
		//Kill the rabbit for now
		//rabbit.SetActive(false);
		//Time.timeScale = 0;
		
		//Show game over text
		//gameOverText.SetActive(true);
		Debug.Log("COLLIDED");
		
		
	}
}
