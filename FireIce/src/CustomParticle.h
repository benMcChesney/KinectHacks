/*
 *  CustomParticle.h
 *  TuioKinect
 *
 *  Created by Ben McChesney on 12/13/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _CUSTOM_PARTICLE
#define _CUSTOM_PARTICLE

#include "ofxBox2dCircle.h"

// ------------------------------------------------- a simple extended box2d circle
class CustomParticle : public ofxBox2dCircle 
{
	
public:
	
	bool isFire ; 
	ofColor color ;
	ofColor innerColor ;
	ofColor outerColor ; 
	bool checkCollision ; 
	
	int life ; 
	int curParticle ;
	vector		<ofPoint>	trail;
	
	CustomParticle( ) 
	{
		life = 0 ; 
		curParticle = 0 ; 
	}
	
	void setColor ( ofColor _color ) 
	{
		life = 60 ;
		innerColor.r = _color.r ; 
		innerColor.g = _color.g ; 
		innerColor.b = _color.b ; 
		outerColor.r = _color.r ; 
		outerColor.g = _color.g ; 
		outerColor.b = _color.b ; 
		
	}
	
	void changeIsFire ( bool newIsFire ) 
	{
		life = 40 ; 
		innerColor = ofColor( ) ;
		outerColor = ofColor( ) ;
		isFire = newIsFire ;
		if ( isFire == true ) 
		{ 
			innerColor.r = 212 ; 
			innerColor.g = 0 ; 
			innerColor.b = 0 ; 
			outerColor.r = 255 ; 
			outerColor.g = 222 ; 
			outerColor.b = 0 ; 
		}
		else 
		{
			innerColor.r = 92 ; 
			innerColor.g = 205; 
			innerColor.b = 205 ; 
			outerColor.r = 230 ; 
			outerColor.g = 255 ; 
			outerColor.b = 255 ; 
		}
		
	}
	
	void draw( int frameCount ) 
	{
		if ( life > 0 ) 
			life-- ; 
		else 
			checkCollision = true ; 

		ofPoint pos = getPosition() ; 
		int trailLength = trail.size(); 
		
		if ( frameCount % 5 == 0  )
		{	
			trail.push_back( pos ) ; 
		}
		
		if ( trailLength > 25 ) 
		{
			trail.erase( trail.begin() ) ; 
		}
		
		float radius = getRadius();

		ofEnableSmoothing() ;
		ofSetColor(innerColor.r, innerColor.g, innerColor.b);
		ofFill();
		
		glPushMatrix();
		glTranslatef( pos.x, pos.y, 0);
		ofCircle(0, 0, radius);	
		ofSetColor(outerColor.r, outerColor.g, outerColor.b ) ; 
		
		glPopMatrix();
		
		
		
	
		
		int splitTrail = ( trailLength > 25 ) ? trailLength-25 : 0 ; 
		ofPoint& p = trail[splitTrail] ; 
		ofPoint last = trail[splitTrail] ; 
		
		ofEnableAlphaBlending() ; 
		
		for ( int i = splitTrail ; i < trailLength ; i++ )
		{
			p = trail[i] ;
			float ratio = ((float)(i-splitTrail) / 25.0f ) ; 
			ofSetColor(outerColor.r, outerColor.g, outerColor.b, 255 * ratio );

			glPushMatrix();
			glTranslatef( p.x, p.y, 0 );
			ofCircle(0, 0, (radius/2) * (ratio) );
			glPopMatrix();
			
						ofSetLineWidth( (radius*100) * ratio ) ;
			ofLine( last.x , last.y , p.x , p.y ) ; 
			last = p ; 
		}	
		
		ofDisableAlphaBlending() ; 
	}
};


#endif 
