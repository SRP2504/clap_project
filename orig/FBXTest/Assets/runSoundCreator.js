#pragma strict

var StepSound : AudioClip;

function Start() {
	

	
	audio.clip = StepSound;
	//audio.Stop();
	
} 
   
function Update() {

	//Debug.Log(Input.GetAxis("Vertical"));
 
	if (! Input.GetAxis ("Vertical")) { 
	
		//Debug.Log("SV");
	
		audio.clip = StepSound;
		audio.loop = true;
		audio.Play();
		
   } 
   
 
} 