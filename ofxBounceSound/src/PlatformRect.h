/*
 *  PlatformRect.h
 *  TuioKinect
 *
 *  Created by Ben McChesney on 12/29/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

/*
 *  CustomParticle.h
 *  TuioKinect
 *
 *  Created by Ben McChesney on 12/13/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _PLATFORM_RECT
#define _PLATFORM_RECT

#include "ofxBox2dCircle.h"

// ------------------------------------------------- a simple extended box2d circle
class PlatformRect : public ofxBox2dCircle
{
	
public:
	
	int value , w , h ; 
	ofColor color; 
	
	PlatformRect()
	{
		value = 0 ; 
		w = 50 ; 
		h = 100 ; 
	}
	
	PlatformRect( int _w , int _h) 
	{
		value = 0 ; 
		w = _w ; 
		h = _h ; 
	}
	
	void setColor ( ofColor _color ) 
	{
		color.r = _color.r ; 
		color.g = _color.g ; 
		color.b = _color.b ; 
	}
	
};


#endif 
