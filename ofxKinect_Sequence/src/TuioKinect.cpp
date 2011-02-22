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


#include "TuioKinect.h"


ofxCvKalman *tuioPointSmoothed[32];


TuioPoint updateKalman(int id, TuioPoint tp) {
	if (id>=16) return NULL;
	if(tuioPointSmoothed[id*2] == NULL) {
		tuioPointSmoothed[id*2] = new ofxCvKalman(tp.getX());
		tuioPointSmoothed[id*2+1] = new ofxCvKalman(tp.getY());
	} else {
		tp.update(tuioPointSmoothed[id*2]->correct(tp.getX()),tuioPointSmoothed[id*2+1]->correct(tp.getY()));
	}
	
	return tp;
}

void clearKalman(int id) {
	if (id>=16) return;
	if(tuioPointSmoothed[id*2]) {
		delete tuioPointSmoothed[id*2];
		tuioPointSmoothed[id*2] = NULL;
		delete tuioPointSmoothed[id*2+1];
		tuioPointSmoothed[id*2+1] = NULL;
	}
}

ofColor userColors[8] ; 


//--------------------------------------------------------------
void TuioKinect::setup()
{
	ofSetWindowTitle("TuioKinect");
	
	kinect.init();
	kinect.setVerbose(true);
	kinect.open();
	
	depthImage.allocate(kinect.width, kinect.height);
	grayImage.allocate(kinect.width, kinect.height);
	colorImage.allocate(kinect.width, kinect.height);
	redImage.allocate(kinect.width, kinect.height);

	nearThreshold = 134;
	farThreshold  = 175;
	//farThreshold  = 100;

	TuioTime::initSession();	
	tuioServer = new TuioServer();
	tuioServer->setSourceName("TuioKinect");
	tuioServer->enableObjectProfile(false);
	tuioServer->enableBlobProfile(false);
	
	for (int i=0;i<32;i++)
		tuioPointSmoothed[i] = NULL;

	
	//Audio setup
	sampleRate 			= 44100;
	phase 				= 0;
	phaseAdder 			= 0.0f;
	phaseAdderTarget 	= 0.0f;
	volume				= 0.5f;
	bNoise 				= false;
	lAudio = new float[256];
	rAudio = new float[256];
	//uncomment this if you want to hear the sound wave
	//ofSoundStreamSetup(2,0,this, sampleRate,256, 4);

	//soundSample
	bass.loadSound("sounds/bass.wav" , true );
	bass.setMultiPlay(true) ; 
	
	melody.loadSound("sounds/melody.wav" , true );
	melody.setMultiPlay(true) ; 
	
	beats.loadSound("sounds/beat.wav", true );
	beats.setMultiPlay(true) ; 
	
	ambient.loadSound("sounds/ambient.wav" , true ) ; 
	ambient.setMultiPlay(true) ; 

	bass.setVolume(0.35f);
	melody.setVolume(0.25f);
	beats.setVolume(0.65f);
	ambient.setVolume(0.5f);
	
	sounds.push_back ( beats ) ; 
	sounds.push_back ( melody ) ; 
	sounds.push_back ( bass ) ; 
	sounds.push_back ( ambient ) ; 

	ofSetFrameRate(30);
	numFrames = 120 ;

	//
	ofColor color ; 
	color.r = 255 ; 
	color.g = 210 ; 
	color.b = 25 ;
	userColors[0] = color ;
	color.r = 120 ; 
	color.g = 178 ; 
	color.b = 25 ;
	userColors[1] = color ;
	
	//
	color.r = 246; 
	color.g = 10; 
	color.b = 32;
	userColors[2] = color ;
	color.r = 65 ; 
	color.g = 65 ; 
	color.b = 65 ; 
	userColors[3] = color ;
	
	//
	color.r = 0 ; 
	color.g = 196 ; 
	color.b = 217 ;
	userColors[4] = color ;
	color.r = 193 ; 
	color.g = 255 ; 
	color.b = 239 ;
	userColors[5] = color ;
	
	//
	color.r = 0; 
	color.g = 102; 
	color.b = 189;
	userColors[6] = color ;
	color.r = 102 ; 
	color.g = 181 ; 
	color.b = 204 ; 
	userColors[7] = color ;
	
	numTracks = 4 ; 
	
	loopIncrement = 2 ;	//number of normalized loops to step up sequence
	curLoop  = 0 ;			//which track to start on
	loopCount = 0 ; 	
	
	
	vector <int> rates0 ; 
	rates0.push_back ( 1 ) ;  
	rates0.push_back ( numFrames / 2 ) ;  
	vector <int> rates1 ; 
	//rates1.push_back ( 1 ) ; 
	rates1.push_back ( numFrames / 4 ) ; 
	rates1.push_back ( numFrames / 16  ) ; 
	rates1.push_back ( numFrames / 8 ) ;
	rates1.push_back ( numFrames / 16  ) ;
	 
	vector <int> rates2 ;
	//rates2.push_back ( 1 ) ;
	rates2.push_back ( numFrames / 16 ) ;
	rates2.push_back ( numFrames / 16 ) ;
	rates2.push_back ( numFrames / 8 ) ;
	rates2.push_back ( numFrames / 16 ) ;
	rates2.push_back ( numFrames / 16 ) ;
	rates2.push_back ( numFrames / 8 ) ;
	//
	vector <int> rates3 ; 
	//rates3.push_back( 1 ) ;
	rates3.push_back ( numFrames / 32 ) ;  
	rates3.push_back ( numFrames / 16 ) ; 
	rates3.push_back ( numFrames / 4 ) ; 
	rates3.push_back ( numFrames / 32 ) ;
	rates3.push_back ( numFrames / 8 ) ;
	 
	/*
	 int _numFrames , ofColor _color , ofColor _secondary ,  int _index , Boolean _lockedX , 
	 int _trailSize , float _radius , float _startSpeed , float _range ,
	 ofPoint _rotation , ofSoundPlayer _soundPlayer, vector <int> _playRates
	*/
	tracks.push_back( SequenceTrail( numFrames , userColors[0] , userColors[1] , 0 , true , 15 , 18 , 1.0f , 0.2f , ofPoint( 0 , 1.0f , 1080.0f ) , sounds[0] , rates0 ) ) ;  
	tracks.push_back( SequenceTrail( numFrames , userColors[2] , userColors[3] , 1 , false , 12 , 24 , .750f , 0.35f , ofPoint( 0 , 0 , -15.0f ) , sounds[1] , rates1 ) ) ;  
	tracks.push_back( SequenceTrail( numFrames , userColors[4] , userColors[5] , 2 , true , 30 , 35 , 0.85f , 0.45f , ofPoint( 0, 4 , 1080 ) , sounds[2] , rates2 ) ) ;  
	tracks.push_back( SequenceTrail( numFrames , userColors[6] , userColors[7] , 3 , false , 18 , 80 , .70f , 0.35f , ofPoint( 0 , 0 , 0 ) , sounds[3] , rates3 ) ) ;  
}



//--------------------------------------------------------------
void TuioKinect::update()
{

	ofBackground(10, 10, 10);
	
	//Kinect + Feeds updating
	kinect.update();

	grayImage.setFromPixels(kinect.getDepthPixels(), kinect.width, kinect.height);
	grayImage.mirror(false, true);
	
	unsigned char * pix = grayImage.getPixels();
	int numPixels = grayImage.getWidth() * grayImage.getHeight()-1;
	
	depthImage.setFromPixels(pix, kinect.width, kinect.height);
	depthImage.flagImageChanged();
	
	//colorImage.setFromPixels(kinect.getPixels(), kinect.width, kinect.height);
	//colorImage.mirror(false, true);
	//colorImage.convertToGrayscalePlanarImage(redImage, 0);


	for(int i = numPixels; i > 0 ; i--){
		if( pix[i] > nearThreshold && pix[i] < farThreshold ){
			pix[i] = 255;
		}else{
			pix[i] = 0;
		}
	}
	
	//update the cv image
	grayImage.flagImageChanged();
	
	unsigned char * red = redImage.getPixels();
	numPixels = redImage.getWidth() * redImage.getHeight();
	//
	contourFinder.findContours(grayImage, 900, (kinect.width*kinect.height)/8, 20, false);
	
	TuioTime frameTime = TuioTime::getSessionTime();
	tuioServer->initFrame(frameTime);
	
	//detect all blobs
	std::vector<ofxCvBlob>::iterator blob;
	for (blob=contourFinder.blobs.begin(); blob!= contourFinder.blobs.end(); blob++) 
	{
		float xpos = (*blob).centroid.x;
		float ypos = (*blob).centroid.y;
				
		TuioPoint tp(xpos/kinect.width,ypos/kinect.height);
		
		//if ((tp.getY() > 0.8) && (tp.getX()>0.25) && (tp.getX()<0.75)) continue;
		
		TuioCursor *tcur = tuioServer->getClosestTuioCursor(tp.getX(),tp.getY());
		
		//Adjust here to change minumum distance for new points
		if ((tcur==NULL) || (tcur->getDistance(&tp)>0.2)) { 
			tcur = tuioServer->addTuioCursor(tp.getX(), tp.getY());
			updateKalman(tcur->getCursorID(),tcur);
		} else {
			TuioPoint kp = updateKalman(tcur->getCursorID(),tp);
			tuioServer->updateTuioCursor(tcur, kp.getX(), kp.getY());
		}
	}

	//remove non moving cursors
	tuioServer->stopUntouchedMovingCursors();
	
	std::list<TuioCursor*> dead_cursor_list = tuioServer->getUntouchedCursors();
	std::list<TuioCursor*>::iterator dead_cursor;
	for (dead_cursor=dead_cursor_list.begin(); dead_cursor!= dead_cursor_list.end(); dead_cursor++) 
	{
		clearKalman((*dead_cursor)->getCursorID());
	}
	
	tuioServer->removeUntouchedStoppedCursors();
	tuioServer->commitFrame();
	
	//Play sounds
	ofSoundUpdate();
}

//--------------------------------------------------------------
void TuioKinect::draw()
{
	float height = (float)ofGetHeight();
	float width = (float)ofGetWidth() ; 
	
	ofSetColor(0,0,0,200) ; 
	ofEnableSmoothing();
	
	ofSetColor(255, 255, 255 );
	//depthImage.draw(15, 15, 128, 96);
	//redImage.draw(425, 15, 400, 300);
	//grayImage.draw(5, 5, 180, 120);
	contourFinder.draw(15, 15, 128, 96);
	ofSetColor(255, 255, 255);
 	ofFill() ; 

	ofSetColor(255, 255, 255);
	std::list<TuioCursor*> alive_cursor_list = tuioServer->getTuioCursors();
	std::list<TuioCursor*>::iterator alive_cursor;

	int count = 0 ; 
	

	int normalizedFrame = ofGetFrameNum() % numFrames ; 
	
	if ( normalizedFrame == 0 ) 
	{
		loopCount++ ;
		if ( loopCount == loopIncrement+1) 
		{
			curLoop++ ; 
			loopCount = 1 ; 
			if ( curLoop > numTracks ) 
				curLoop = 1 ; 
		}
	}
	
	count += curLoop ; 
	

	for (alive_cursor=alive_cursor_list.begin(); alive_cursor!= alive_cursor_list.end(); alive_cursor++) 
	{
		if ( count > numTracks-1 ) 
			count = 0 ; 
		TuioCursor *ac = (*alive_cursor);
		
		pan = ac->getX();
		float heightPct = ac->getY() ; 
		float absXSpeed = ac->getXSpeed() ;
		float absYSpeed = ac->getYSpeed() ;
	
		absXSpeed = ( absXSpeed < 0 ) ? absXSpeed * -1 : absXSpeed ; 
		absYSpeed = ( absYSpeed < 0 ) ? absYSpeed * -1 : absYSpeed ; 
		if ( absXSpeed > 5.0f || absYSpeed > 5.0f ) 
		{
			
		}
		
		targetFrequency = 1500.0f * heightPct;
		phaseAdderTarget = (targetFrequency / (float) sampleRate) * TWO_PI;
		
		if ( tracks[count].lockedX == true ) {
			float newX =  normalizedFrame * (ofGetWidth() / numFrames) ; 
			tracks[count].sequence[normalizedFrame].position = ofPoint ( newX ,  heightPct * ofGetHeight() );
		}
		else {
			tracks[count].sequence[normalizedFrame].position = ofPoint (  ac->getX() * ofGetWidth() , heightPct * ofGetHeight() );	
		}

		count++ ;
	}
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	ofEnableAlphaBlending() ; 
	ofSetColor ( 255 , 255 , 255 ) ; 
	//SequenceTrail &sq = tracks[0] ; 
	
	for ( int i = 0 ; i < numTracks ; i++ )
	{
		tracks[i].draw( normalizedFrame ) ; 
	}
	
	ofSetColor ( 255 ,0 , 0 , 175 ) ; 
	
	//Quiet that annpying whine
	if ( alive_cursor_list.begin() == alive_cursor_list.end() ) 
	{
		volume = 0.15f ; 
	}

	ofEnableAlphaBlending() ; 
	
	ofSetColor(255, 255, 255,15) ; 
	ofRect( normalizedFrame * (ofGetWidth() / numFrames ) - 8, 0, 16, height );
	//Debug Text
	
	ofSetColor(255, 255, 255);
	char reportStr[1024];
	//	if ( loopCount == loopIncrement	curLoop++ ; 
	sprintf(reportStr, "set near threshold %i (press: + -)\nset far threshold %i (press: < >)  \n loop: %i / %i  @index : %i  ", nearThreshold, farThreshold , loopCount ,loopIncrement  ,  curLoop );
	ofDrawBitmapString(reportStr, 20, 575);
	

}

//--------------------------------------------------------------
void TuioKinect::exit(){
	kinect.close();
}

//--------------------------------------------------------------
void TuioKinect::keyPressed (int key)
{
	switch (key)
	{	
		case '>':
		case '.':
			farThreshold ++;
			if (farThreshold > 255) farThreshold = 255;
			break;
		case '<':		
		case ',':		
			farThreshold --;
			if (farThreshold < 0) farThreshold = 0;
			break;
			
		case '+':
		case '=':
			nearThreshold ++;
			if (nearThreshold > 255) nearThreshold = 255;
			break;
		case '-':		
			nearThreshold --;
			if (nearThreshold < 0) nearThreshold = 0;
			break;
		case 'w':
			kinect.enableDepthNearValueWhite(!kinect.isDepthNearValueWhite());
			break;
	}
}

//--------------------------------------------------------------
void TuioKinect::mouseMoved(int x, int y)
{}

//--------------------------------------------------------------
void TuioKinect::mouseDragged(int x, int y, int button)
{}

//--------------------------------------------------------------
void TuioKinect::mousePressed(int x, int y, int button)
{}

//--------------------------------------------------------------
void TuioKinect::mouseReleased(int x, int y, int button)
{}

//--------------------------------------------------------------
void TuioKinect::windowResized(int w, int h)
{}

