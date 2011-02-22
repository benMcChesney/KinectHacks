/*
 *  SquareRotateParticle.h
 *  TuioKinect
 *
 *  Created by Ben McChesney on 2/19/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _SQUARE_ROT_PARTICLE
#define _SQUARE_ROT_PARTICLE
#include "AbstractParticle.h"
#include "ofMain.h"

class SquareRotateParticle : public AbstractParticle
{
	
	public :
	/*
	 ofColor color; 
	 float _size ;
	 ofPoint position, velocity ; 
	 bool active ; 
	 */
	
	SquareRotateParticle() 
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
	
	SquareRotateParticle( float __size , ofColor _color )
	{
		_size = __size ; 
		color = _color; 
	}
	
	virtual void draw( float ratio , int index ) 
	{
		ofEnableAlphaBlending() ; 
		ofSetColor ( color.r , color.g, color.b ); 
		glPushMatrix();
		glTranslatef( position.x, position.y, 0);	
		ofRotateX(index * 10 ) ; 
		ofRect( 0 , 0 , ratio * _size  , ratio * _size  ) ; 
		ofRotateX(-index * 10 ) ; 
		glPopMatrix();
		ofDisableAlphaBlending() ; 
	}
	
	virtual void update()
	{
		//position += velocity ; 
	}
};

#endif