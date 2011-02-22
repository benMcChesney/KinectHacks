/*
 *  KinectCursor.h
 *  TuioKinect
 *
 *  Created by Ben McChesney on 12/12/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef _KINECT_CURSOR
#define _KINECT_CURSOR


class KinectCursor : public ofBaseApp
{
	
	public:
	
	float x , y ; 
	int index ; 
	
	KinectCursor()
	{
		x = 0 ; 
		y = 0 ; 
		index = 0 ; 
	}
	
	KinectCursor( float _x , float _y , int _index ) 
	{
		x = _x ; 
		y = _y ; 
		index = _index ; 
	}

	
} ; 


#endif 
