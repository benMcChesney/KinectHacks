/*
 *  CircleParticle.h
 *  TuioKinect
 *
 *  Created by Ben McChesney on 2/19/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _CIRCLE_PARTICLE
#define _CIRCLE_PARTICLE
#include "AbstractParticle.h"
#include "ofMain.h"

class CircleParticle : public AbstractParticle
{

	public :
	
	/*
	 ofColor color; 
	 float _size ;
	 ofPoint position, velocity ; 
	 bool active ; 
	 */
	
		CircleParticle() 
		{
			color = ofColor ( ) ; 
			color.r = 255 ; 
			color.g = 0 ; 
			color.b = 0 ; 
			_size = 4.0f ; 
			position = ofPoint () ; 
			velocity = ofPoint () ; 
			active = true ; 
		} 
	
		CircleParticle( float __size ) 
		{
			color = ofColor ( ) ; 
			color.r = 255 ; 
			color.g = 0 ; 
			color.b = 0 ; 
			_size = 4.0f ; 
			position = ofPoint () ; 
			velocity = ofPoint () ; 
			active = true ; 
		}	
	
		virtual void draw( float ratio , int index ) 
		{

		}
	
		virtual void update()
		{
			//position += velocity ; 
		}
};

#endif