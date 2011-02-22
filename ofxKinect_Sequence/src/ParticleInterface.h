/*
 *  ParticleInterface.h
 *  TuioKinect
 *
 *  Created by Ben McChesney on 2/20/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef PARATICLE_INTERFACE
#define PARATICLE_INTERFACE
/////////////////////////////////////////////////////////////
#include "ofMain.h"


	
	struct ParticleInterface {
		virtual void update() = 0;
		virtual void draw( float ratio , float index, ofColor curColor ) = 0;
		virtual ~ParticleInterface() {}
	};
	/*
	namespace ES2 {
		ScRenderEngineInterface* CreateRenderingEngine(ProgramManagerPtr &programManager); 
	}
	*/

#endif  
