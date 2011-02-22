/*
 *  AbstractParticle.h
 *  TuioKinect
 *
 *  Created by Ben McChesney on 2/19/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */


#ifndef _ABSTRACT_PARTICLE
#define _ABSTRACT_PARTICLE
#include "ofMain.h" 
#include "ParticleInterface.h"

	
	class AbstractParticle 	{
		public :
			ofColor color; 
			ofColor secondary ; 
			float _size ;
			ofPoint position, velocity , rotation ; 
			bool active ; 
			int shapeType ; 
	
		AbstractParticle()
		{
			color = ofColor ( ) ; 
			color.r = 255 ; 
			color.g = 0 ; 
			color.b = 0 ; 
			_size = 4.0f ; 
			position = ofPoint () ; 
			velocity = ofPoint () ; 
			rotation = ofPoint ( 10 , 10 , 10 ); 
			active = true ; 
			shapeType = 0 ; 
			
	}	
	
	AbstractParticle( float __size , ofColor _color , ofColor _secondary, ofPoint _rotation , int _shapeType )
	{
		color = _color ; 
		secondary = _secondary ; 
		_size = __size ;  
		position = ofPoint () ; 
		velocity = ofPoint () ; 
		rotation = _rotation ; 
		shapeType = _shapeType ; 
		active = true ;
	}
	
	
	virtual void update() 
	{
		
	}
	
	virtual void draw( float ratio , float index, ofColor curColor ) 
	{ 
		ofEnableAlphaBlending() ; 
		ofSetColor ( curColor.r , curColor.g, curColor.b , (float)200 * ratio ); 
		glPushMatrix();
		glTranslatef( position.x, position.y, 0);	
		ofRotate( index ,  rotation.x , rotation.y, rotation.z) ; 
		
		float __size = _size ; 
		float _radiusSize = ratio * __size ;
		switch ( shapeType ) 
		{
			case 0:
				 
				ofRect( 0 , sin( ratio ) * __size * 10.0f + 5.0f , _radiusSize , _radiusSize ) ; 
				ofRect( 0 , cos( ratio ) * __size * 10.0f - 5.0f, _radiusSize , _radiusSize ) ; 
				break;
			case 1:
				ofSetLineWidth( 40 ) ;
				ofLine( 0 , 0 , ofRandom(-__size/2 ,__size/2) * ratio ,  ofRandom(-__size/2 , __size/2 ) * ratio ) ; 
				ofSetLineWidth( 1 ) ;
			case 2:	
				ofLine ( 0 , 0 , 0 , _radiusSize ) ;
				ofLine ( 0 , 0 , 0 , -_radiusSize ) ;
				break ;
			case 3 :
				ofCircle(0, 0, (1.0f-ratio) * __size ) ;
				break ; 
		}

		ofRotate( -index ,  rotation.x , rotation.y, rotation.z) ; 
		glPopMatrix();
		ofDisableAlphaBlending() ; 
	}
};

#endif
