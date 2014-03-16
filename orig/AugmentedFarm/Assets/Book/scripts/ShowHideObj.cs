/*******************************************************************************

INTEL CORPORATION PROPRIETARY INFORMATION
This software is supplied under the terms of a license agreement or nondisclosure
agreement with Intel Corporation and may not be copied or disclosed except in
accordance with the terms of that agreement
Copyright(c) 2012-2013 Intel Corporation. All Rights Reserved.

*******************************************************************************/
using UnityEngine;
using System.Collections;

public class ShowHideObj : MonoBehaviour
{
	bool state = true;
	// Use this for initialization
	void Start ()
	{
	}
	
	void Update ()
	{
		// Test
		if (Input.GetKeyDown (KeyCode.Z)) {
			
			state = !state;
		
			print("Clicked Z - in ShowHide script, state = " + state);
			
			ToggleVisibility (gameObject.transform, state);
			
		}
	}

	// Change visability of child
	static void ToggleVisibility (Transform obj, bool state)
	{
		for (int i = 0; i < obj.GetChildCount(); i++) {
			if (obj.GetChild (i).guiTexture != null)
				obj.GetChild (i).guiTexture.enabled = state;
			if (obj.GetChild (i).guiText != null)
				obj.GetChild (i).guiText.enabled = state;

			if (obj.GetChild (i).GetChildCount () > 0) {
				ToggleVisibility (obj.GetChild (i), state);
			}
		}
	}
}