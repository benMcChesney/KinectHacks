/*
 *  Emitter.h
 *  TuioKinect
 *
 *  Created by Ben McChesney on 12/11/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */



#ifndef _EMITTER
#define _EMITTER

#include "ofMain.h"
#include "Particle.h"
#define size 500

class Emitter 
{

	public : 
		int x,y ; 
		int cur ; 
		int maxSize ; 
		Particle* p[size];
	
		Emitter()
		{
			x = 0 ; 
			y = 0 ; 
			cur = 0 ; 
			maxSize = 100 ; 
			setup() ; 
		}
		Emitter(int _x , int _y , int _maxSize )
		{
			x = _x ; 
			y = _y ; 
			maxSize = _maxSize ; 
			setup() ; 
		}
	
		//std::list<Particle> particles ; 
		
		void setup() ; 
		void update() ; 
		void draw() ; 
	
};

#endif
