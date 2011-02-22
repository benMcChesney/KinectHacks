/*
 *  Emitter.cpp
 *  TuioKinect
 *
 *  Created by Ben McChesney on 12/11/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */
#include "Emitter.h"

Particle particles[150] ; 
void Emitter::setup() 
{
	float halfX = ofGetWidth() / 2; 
	float halfY = ofGetHeight() / 2; 

	for (int i=0; i<size; i++) p[i] = new Particle();
	//for ( int i = 0 ; i < 150 ; i++ )
	//{
//		Particle p = Particle ( ofRandom( 10 , 15 ) , halfX , halfY , ofRandom(-5.0f , 5.0f ) , ofRandom(-5.0f , 5.0f ) ) ; /
//		particles[ i ] = p ; 
//	}
}

void Emitter::update() 
{
	for ( int i = 0 ; i < 150 ; i++ ) 
	{
		Particle p = particles [ i ] ; 
		p.x = p.x + p.xSpeed ; 
		p.y += p.y + p.ySpeed ;
		p.life-- ;
		
	}
	//std::list<Particle>::iterator curParticle ; 
	/*
	for (curParticle=particles.begin() ; curParticle != particles.end() ; curParticle++) 
	{
		Particle p = (*curParticle);
		p.x += p.xSpeed ; 
		p.y += p.ySpeed ;
		p.life-- ;
	}*/
	
}

void Emitter::draw()
{
	//ofTranslate(x, y, 0) ; 
	for ( int i = 0 ; i < 150 ; i++ ) 
	{
		Particle p = particles [ i ] ;
		ofSetColor(255, 0, 0) ;
		ofCircle(p.x, p.y , 15 ) ;
	}
	//ofTranslate(x, y, 0) ; 
	//std::list<Particle>::iterator curParticle ; 
	
}