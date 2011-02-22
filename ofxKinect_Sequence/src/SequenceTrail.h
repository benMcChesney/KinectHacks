/*
 *  SequenceTrail.h
 *  TuioKinect
 *
 *  Created by Ben McChesney on 2/7/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _SEQUENCE_TRAIL
#define _SEQUENCE_TRAIL

#include "ofMain.h" 
#include "AbstractParticle.h"

class SequenceTrail 
{
	public:
	
		//render info
		vector			<AbstractParticle>	sequence;
		ofColor			color ; 
		ofColor			secondary ; 
		ofPoint			rotation ; 
		float radius ; 
	
		//sound info
		int playRate ; 
		ofSoundPlayer	soundPlayer ; 
		int				playRateIndex ; 
		int				runningTotal ; //current total of prev sampleRate frames
		vector	<int>	playRates;
		Boolean			lockedX ; 
		float			startSpeed , range ; 
	
		//general info
		int				trailSize , numFrames, index ; 
		
		SequenceTrail() 
		{
			lockedX = true ; 
			playRateIndex = 0 ; 
			trailSize = 15 ; 
			radius = 5.0f ; 
			startSpeed = 0.75f ; 
			range = 0.5f ;
			playRate = 8 ; 
		}	

		SequenceTrail( int _numFrames , ofColor _color , ofColor _secondary ,  int _index , Boolean _lockedX , 
					   int _trailSize , float _radius , float _startSpeed , float _range ,
					   ofPoint _rotation , ofSoundPlayer _soundPlayer, vector <int> _playRates ) 
		{
			color = _color ; 
			index = _index ; 
			lockedX  = _lockedX ; 
			playRateIndex = 0 ; 
			trailSize = _trailSize ; 			
			radius = _radius ; 
			range = _range ; 
			startSpeed = _startSpeed ; 
			numFrames = _numFrames ; 
			rotation = _rotation ;
			soundPlayer = _soundPlayer ; 
			playRates= _playRates ;  
			runningTotal = 0 ; 
			secondary = _secondary ; 
			//float __size , ofColor _color
			for ( int i = 0 ; i < numFrames ; i++ ) 
			{
				sequence.push_back ( AbstractParticle( _radius , _color , secondary, _rotation , _index ) ) ; 
			}
			
			
		}
		
	void draw( int curFrame );
	void addTimingStep() ;
	void update() ; 	

};


#endif