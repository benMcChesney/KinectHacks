/*
 TuioKinect - A simple TUIO hand tracker for the Kinect 
 Copyright (c) 2010 Martin Kaltenbrunner <martin@tuio.org>
 
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef _TUIO_KINECT
#define _TUIO_KINECT

#include "ofMain.h"

#include "ofxOpenCv.h"
#include "ofxKinect.h"
#include "ofxCvKalman.h"
#include "TuioServer.h"
#include "ofxVectorMath.h"

#include "ofxBox2d.h"
#include "KinectCursor.h" 

#define PARTICLE_COUNT = 100 ; 
#define MAX_HOLD = 60 ;


using namespace TUIO;

// ------------------------------------------------- a simple extended box2d circle
class CustomParticle : public ofxBox2dCircle 
{
	
public:
	
	CustomParticle( ) 
	{
		
	}
	
	ofColor color;
	void draw() 
	{
		float radius = getRadius();
		
		glPushMatrix();
		glTranslatef(getPosition().x, getPosition().y, 0);
		
		ofSetColor(color.r, color.g, color.b);
		ofFill();
		//img.draw(-32,-32); 
		ofCircle(0, 0, radius);	
		
		glPopMatrix();
	}
};


class TuioKinect : public ofBaseApp
{

	public:

		void setup();
		void update();
		void draw();
        void exit();

		void keyPressed  (int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void drawTuioPath( std::list<TuioPoint> acPath );

		ofxKinect kinect;

		ofxCvGrayscaleImage	grayImage;
		ofxCvGrayscaleImage	depthImage;
		ofxCvColorImage		colorImage;
		ofxCvGrayscaleImage	redImage;
		
		ofxCvContourFinder 	contourFinder;
		TuioServer *tuioServer;

		int nearThreshold;
		int	farThreshold;

		//AUDIO stuff
		void audioRequested 	(float * input, int bufferSize, int nChannels);
		void checkTUIOHit(float x , float y) ; 
	
		float 	pan;
		int		sampleRate;
		bool 	bNoise;
		float 	volume;
	
		float 	* lAudio;
		float   * rAudio;
	
		//Sound Part 1 sound wave synthesis
		float 	targetFrequency;
		float 	phase;
		float 	phaseAdder;
		float 	phaseAdderTarget;
	
		//Sound Part 2 - Samples
		ofSoundPlayer  beats;
		ofSoundPlayer  synth;
		ofSoundPlayer  synth1;
		float 		synthPosition;
	
		//box2D
		bool							bDrawLines;
		bool							bMouseForce;
	
		ofxBox2d						box2d;			  //	the box2d world
		vector		<CustomParticle>	customParticles;  //	this is a custom particle the extends a cirlce
	
		int holdingLength ;  //how long as two hands been up, ( increases strength of pull ) 
		int curToReplace ;	//index of keeping within the bounds
	
		
};

#endif
