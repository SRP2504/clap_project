using UnityEngine;
using System.Collections;


//@TODO: Move character in a curved path while translating


public class rabbitMover : MonoBehaviour {
	
	public GameObject rightEar;
	public GameObject leftEar;
	public GameObject rabbitBody;
	public GameObject timeScaleText;
	public GameObject backgroundColorizer;
	public GameObject lion;
	
	//Rabbit sprites
	public Material rabbitSpriteRHandHigh;
	public Material rabbitSpriteLHandHigh;
	public Material rabbitSpriteBothHandsHigh;
	public Material rabbitSpriteDefault;
	
	private Vector3[] jumpPath;
	private Vector3 tempJumpComponent;
	float percentsPerSecond = 0.02f;
	float currentPathPercent = 0.0f;
	
	float currX;
	float currY;
	float finX, finY;
	
	private float maxJumpValue = 7.5f;		//Range for RNG to move the character
	private int jumpFreq = 20;
	private float changeColorBy; 
	
	int rabbitMoveDirection = 1;				//+1 if left to right (position.X increases). -1 if right to left (X decreases).
	
	bool didPlayerMissClap;						//Set to true if the player misses a clap.
	
	float rabbitMoveFactorDefault = 2.0f;		//Move by these many units by default
	float rabbitMoveFactorOnPenalty = 1.0f;		//Move by these many on penalty
	float rabbitMoveBy;
	const float rabbitMaxRScreen = 12.5f;		//Position of rabbit beyond which it goes out of camera view on right side
	const float rabbitMaxLScreen = -12.5f;		//and on the left side
	
	bool shouldRabbitJumpOverLion = false;		//Set to true if rabbit reaches end of screen, and must jump over the lion. //Reet to false after use.
	
	float rabbitJumpHeightDefault = 2.0f;		//How much to jump - by default
	float rabbitJumpHeightWithPower = 8.0f;		//on having power
	
	int combinationHolder;
	
	
	
	
	// Use this for initialization
	void Start () {
		jumpPath = new Vector3[3];
	}
	
	// Update is called once per frame
	void Update () {
		
		//Modify time scale to speed up as the level progresses
		//Time.timeScale = 1.0f + (int)(Time.frameCount / 300);
		Time.fixedDeltaTime = 0.02f * Time.timeScale;
		
		
		//Every nth frame, change the background color
		timeScaleText.guiText.text = backgroundColorizer.renderer.material.color.r.ToString();
		if((Time.frameCount % 300) == 0) {
			
			if(backgroundColorizer.renderer.material.color.r >= 1.0f) {
				changeColorBy = 0.0f;
			} else {
				changeColorBy = 0.05f;
			}
			iTween.ColorTo(backgroundColorizer, 
				iTween.Hash("r", backgroundColorizer.renderer.material.color.r + changeColorBy, 
				"g", backgroundColorizer.renderer.material.color.g - changeColorBy, 
				"b", backgroundColorizer.renderer.material.color.b - changeColorBy, 
				"easetype", "linear"));
		}
		
		
	
		
		//Raise hand to clap on every nth frame
		if(Time.frameCount % 20 == 0) {
		
			currX = transform.position.x;
			currY = transform.position.y;
			
			//Generate a random number between 0 and 3, to decide which hand combination to raise
			combinationHolder = (int)(Random.value * 4);
			Debug.Log(combinationHolder);
			switch (combinationHolder) {
			
			case 0 : 
				//no hands
				rabbitBody.renderer.material = rabbitSpriteDefault;
				break;
				
			case 1: 
				//left hand high, right hand low
				rabbitBody.renderer.material = rabbitSpriteLHandHigh;
				break;
				
			case 2:
				//right hand high, left hand low
				rabbitBody.renderer.material = rabbitSpriteRHandHigh;
				break;
				
			case 3: 
				//both hands high
				rabbitBody.renderer.material = rabbitSpriteBothHandsHigh;
				break;
				
			}
			
			
			//Move to a random new location
			//finX = -7 + (Random.value * 15);
			//finY = currY;
			
			//Move to a new location
			if(lion.transform.position.x > transform.position.x) {
				//Lion's to the left of the rabbit, move rabbit to the right
				rabbitMoveDirection = -1;
			} else {
				//Lion's to the right, move rabbit to the left
				rabbitMoveDirection = 1;
			}
			rabbitMoveBy = (didPlayerMissClap ? rabbitMoveFactorOnPenalty : rabbitMoveFactorDefault);
			finX = transform.position.x + (rabbitMoveDirection * rabbitMoveBy);
			if ((finX >= rabbitMaxRScreen) || (finX <= rabbitMaxLScreen)) {	 	//flip the direction
				rabbitMoveDirection *= -1;
				//Jump over the lion to holf of the screen
				finX = 0.0f + (rabbitMoveDirection * 5.0f);
				shouldRabbitJumpOverLion = true;
			}
			
			//Rabbit should not land on the lion, move it ahead if it will
			if(finX == lion.transform.position.x) {
				finX += (rabbitMoveDirection * 3.0f);
			}
			
			
			finY = currY;
			
			Debug.Log(finX + ":" + finY);
			
			
			//Create the path using 3 points (current position, a middle position, end position)
			//Current position
			tempJumpComponent.x = transform.position.x; 
			tempJumpComponent.y = transform.position.y; 
			tempJumpComponent.z = transform.position.z; 
			jumpPath[0] = tempJumpComponent;			
			//Middle position
			tempJumpComponent.x = Mathf.Abs(currX - finX) / 2 + Mathf.Min(currX, finX);
			tempJumpComponent.y = (currY + ((shouldRabbitJumpOverLion) ? (rabbitJumpHeightWithPower):(rabbitJumpHeightDefault)));
			tempJumpComponent.z = transform.position.z;
			jumpPath[1] = tempJumpComponent;
			shouldRabbitJumpOverLion = false;
			//End position
			tempJumpComponent.x = finX; 
			tempJumpComponent.y = finY; 
			tempJumpComponent.z = transform.position.z;
			jumpPath[2] = tempJumpComponent;
			
			
			//Now move on the path
			iTween.MoveTo(rabbitBody, iTween.Hash("path", jumpPath, "easetype", "easeOutQuint"));
			
		}
		
	}
	
	
}
