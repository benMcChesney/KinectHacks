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
#include "ofxVec2f.h" 
#include "Emitter.h"
#include "SequenceTrail.h"
#include "AbstractParticle.h" 
#include "CircleParticle.h"
using namespace TUIO;

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

		ofxKinect kinect;

		ofxCvGrayscaleImage	grayImage;
		ofxCvGrayscaleImage	depthImage;
		ofxCvColorImage		colorImage;
		ofxCvGrayscaleImage	redImage;
		
		ofxCvContourFinder 	contourFinder;
		TuioServer *tuioServer;

		int nearThreshold;
		int	farThreshold;
	
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
		ofSoundPlayer  melody;
		ofSoundPlayer  ambient;
		ofSoundPlayer  bass;
	//	float 			synthPosition;
	
		//Sound Part 3 - Sequence Trail
		int numFrames ; 
		//vector		<ofxVec2f>	sequence;
		vector		<SequenceTrail>	tracks;
		vector		<ofSoundPlayer>	sounds;
		
		int numTracks ; 
	
		int loopIncrement ;		//number of normalized loops to step up sequence
		int curLoop ;			//which track to start on
		int loopCount ;			//currentnumber of loops
};

#endif
