using UnityEngine;
using System.Collections;

public class palm : MonoBehaviour {
	public PXCMGesture.GeoNode.Label pam;
	public ShadowHand shadowHand;
	public Options options;
	public PXCMGesture.GeoNode.Side side;
	
	void Start() {
		StayAway();		
	}
	
	void FixedUpdate () {
		if (shadowHand.handData[(int)side-1][(int)pam].body>0) {
			float radius=(pam==PXCMGesture.GeoNode.Label.LABEL_HAND_LOWER)?1:shadowHand.handData[(int)side-1][(int)pam].radiusImage/18;
			transform.localScale=new Vector3(radius,radius,radius);
			
			rigidbody.MovePosition(shadowHand.MapCoordinates(shadowHand.handData[(int)side-1][(int)pam].positionImage));
			renderer.enabled=options.visible;
			rigidbody.detectCollisions=true;
		} else {
			StayAway();
		}
	}
			
	private void StayAway() {
		rigidbody.detectCollisions=false;
		renderer.enabled=false;
	}
}
