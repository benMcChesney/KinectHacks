#ifndef _TEST_APP
#define _TEST_APP

#include "ofMain.h"

#include "ofxOpenCv.h"
#include "ofxKinect.h"

class testApp : public ofBaseApp
{

	public:

		void setup();
		void update();
		void draw();
		void exit();
	
		void drawPointCloud();

		void keyPressed  (int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void audioReceived 	(float * input, int bufferSize, int nChannels); 
		void drawSoundWave() ;
	
		bool inside(float px, float py, ofRectangle rect ) ; 

		float * left;
		float * right;

		ofPoint offset; 
		ofxKinect kinect;

		ofxCvColorImage		colorImg;

		unsigned char *			alphaPixels;
		unsigned char *			colorPixels;
	
		ofImage					alphaImg ; 
		ofTexture 	maskedImage;
		ofxCvGrayscaleImage 	grayImage;
		ofxCvGrayscaleImage 	grayThresh;
		ofxCvGrayscaleImage 	grayThreshFar;

		ofxCvContourFinder 	contourFinder;
		
		bool				bThreshWithOpenCV;

		int 				nearThreshold;
		int					farThreshold;

		int					angle;
	
		ofVideoPlayer 		movie;
		ofImage background;
		ofImage mask;
		ofImage masked;
	
		ofRectangle lastRect ;
		ofRectangle	boundsRect ; 
};

#endif
