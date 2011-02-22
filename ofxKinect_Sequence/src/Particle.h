/*
 *  Particle.h
 *  TuioKinect
 *
 *  Created by Ben McChesney on 12/11/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _PARTICLE
#define _PARTICLE

class Particle 
{
	public : 
	
	int size , life ; 
	
	float xSpeed , ySpeed ; 
	float x,y ; 
	
	Particle()
	{
		
	} 
	
	Particle( float _x , float _y )
	{
		size = 5 ; 
		x = _x ; 
		y = _y ; 
		xSpeed = 0.0f ; 
		ySpeed = 0.0f ; 
		life = 250 ; 
		
	}
	
	Particle( int _size ,float _x , float _y, float _xSpeed , float _ySpeed )
	{
		size = _size ; 
		x = _x ; 
		y = _y ; 
		xSpeed = _xSpeed ; 
		ySpeed = _ySpeed ;  
		life = 250 ; 
	}
};

#endif
