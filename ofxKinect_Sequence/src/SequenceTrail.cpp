/*
 *  SequenceTrail.cpp
 *  TuioKinect
 *
 *  Created by Ben McChesney on 2/7/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "SequenceTrail.h"


void SequenceTrail::update() 
{
	
}

void SequenceTrail::draw( int curFrame ) 
{
	
	AbstractParticle& _drawMe = sequence[0]  ; 
	ofPoint& p = _drawMe.position ; 
	ofEnableAlphaBlending() ; 
	
	ofSetColor ( color.r ,color.g , color.b  , 50 ) ; 
	ofRect((ofGetWidth() / 4.0f) * index, ofGetHeight()-50, ofGetWidth() / 4.0f , 50 ) ; 
	
	ofSetColor ( color.r ,color.g , color.b  , 75 ) ; 
	ofFill();
	int trailLength = trailSize ;  ; 
	
	int prevIndex = curFrame ;  
	if ( curFrame == 0 ) 
		runningTotal = 0 ; 

	for ( int i = 0 ; i < trailLength ; i++ ) 
	{
		
		if ( curFrame > 0 ) 
			prevIndex = curFrame - i ; 
		else {
			prevIndex = curFrame; 
		}
		
		if ( prevIndex < 0 ) 
			prevIndex += numFrames-1 ; 
		
		if ( prevIndex < numFrames ) 
		{
			_drawMe = sequence[prevIndex] ; 
			p = _drawMe.position ;
			
			if ( _drawMe.active == true && p.x > 0 && p.y > 0 ) 
			{
				//float ratio = 1.0f +- ( (float)i / (float)trailLength ) ; 
				float ratio = ( (float)i / (float)trailLength ) ; 
				
				ofColor curColor = ofColor() ; 
				curColor.r = ofLerp( color.r , secondary.r , ratio ) ; 
				curColor.g = ofLerp( color.g , secondary.g , ratio ) ; 
				curColor.b = ofLerp( color.b , secondary.b , ratio ) ;  
				ratio = 1.0f +- ( (float)i / (float)trailLength ) ; 
				_drawMe.draw( ratio, (float)i , curColor );
				
				//if ( curFrame +- (playRates[playRateIndex]+runningTotal) == 0 ) 
				if ( (curFrame-runningTotal) == playRates[playRateIndex] ) 
				{
					ofSetColor ( color.r , color.g , color.b  , 125 ) ; 
					ofRect( (ofGetWidth() / 4.0f) * index, ofGetHeight()-50, ofGetWidth() / 4.0f , 50 ) ; 
					
					if (soundPlayer.bLoadedOk == true )
					{
						float ratio = (1.0f)-((p.y) / (float)ofGetHeight() ) ; 
						//8 steps
						//Round to the nearest 8th step.
						int _step = ratio * 8.0f ; 
						float ratioStep = (float)_step / 8.0f ; 
						
						//int _step = (int)(ratio*100.0f) | 0 ; 
						//ratio = (float)_step / 8.0f ; 
						float newSpeed = startSpeed + range * ratioStep; 
						soundPlayer.setSpeed( newSpeed );
						soundPlayer.play();	
						runningTotal += playRates[playRateIndex] ; 
			
						playRateIndex++ ; 
						if ( playRateIndex > playRates.size()-1 ) 
						{
							playRateIndex = 0 ; 
						}
						
					}
					
					
				}
			}
		}
	}
	ofDisableAlphaBlending() ; 
	
}
