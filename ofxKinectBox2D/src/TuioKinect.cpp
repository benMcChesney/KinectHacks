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

KinectCursor kCursors[ 2 ] ; 

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

	nearThreshold = 50;
	farThreshold  = 150;
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
	volume				= 0.1f;
	bNoise 				= false;
	lAudio = new float[256];
	rAudio = new float[256];
	//uncomment this if you want to hear the sound wave
	//ofSoundStreamSetup(2,0,this, sampleRate,256, 4);
	
	//soundSamples
	synth.loadSound("sounds/synth.wav");
	synth1.loadSound("sounds/synth1.wav");
	beats.loadSound("sounds/1085.mp3");
	synth.setVolume(0.75f);
	synth1.setVolume(0.75f);
	beats.setVolume(0.5f);
	beats.setMultiPlay(false);
	
	//box2D setup
	bDrawLines  = false;
	bMouseForce = false;
	
	box2d.init();
	box2d.setGravity(0, 10);
	box2d.doSleep = true ; 
	box2d.bCheckBounds = true ; 
	box2d.createFloor();
	box2d.checkBounds(true);
	box2d.createBounds(0, 0, ofGetWidth(), ofGetHeight() ) ; 
	box2d.setFPS(30.0);
	
	holdingLength = 0 ; 
	curToReplace = 0 ; 

	ofSetFrameRate(30);
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
	
	colorImage.setFromPixels(kinect.getPixels(), kinect.width, kinect.height);
	colorImage.mirror(false, true);
	colorImage.convertToGrayscalePlanarImage(redImage, 0);


	
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
	
	int numBlobs = contourFinder.blobs.size() ;
	int count = 0 ; 
	
	if ( numBlobs == 0 ) 
		holdingLength = 0 ;
	
	float sumX ; 
	float sumY ; 
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
		
		xpos = tp.getX() * ofGetWidth() ; 
		ypos = tp.getY() * ofGetHeight() ; 
		
		if ( numBlobs < 2 )
		{
			kCursors[count].x = xpos ; 
			kCursors[count].y = ypos ; 
			holdingLength = 0 ;
		}

		sumX += xpos ; 
		sumY += ypos ;
		
		count++ ;
	}
	
	//Divide the sums of all x/y to get the centroid
	sumX = sumX / (float)numBlobs ; 
	sumY = sumY / (float)numBlobs ; 
	
	if ( numBlobs == 2 ) 
	{
		holdingLength++ ;
		float strength = 10.0f;
		float damping  = 0.5f;
		float minDis   = 200.0f;
		float cursorDist = ofDist( kCursors[0].x , kCursors[0].y , kCursors[1].x , kCursors[1].y ) ;
		float maxDistance = 200.0f ; 
		
		for(int i=0; i< customParticles.size() ; i++) 
		{	
			//customParticles[i].addAttractionPoint(sumX, sumY, (cursorDist/maxDistance)*strength, minDis);
			customParticles[i].addAttractionPoint(sumX, sumY, (float)holdingLength*1.1 * strength, minDis);
			customParticles[i].addDamping(damping, damping);
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
	//ofSoundUpdate();
	
	
	//updateBox2D()
	box2d.update() ; 
}


//--------------------------------------------------------------
void TuioKinect::draw()
{
	
	ofSetColor(255, 255, 255);
	char reportStr[1024];
	sprintf(reportStr, "holding Length %i", holdingLength );
	ofDrawBitmapString(reportStr, 150, 15);
	
	float height = (float)ofGetHeight();
	float width = (float)ofGetWidth() ; 
	
	ofSetColor(0,0,0,200) ; 
	
	
	//Additive blend mode
	glBlendFunc(GL_SRC_COLOR, GL_ONE);
	
	
	ofSetColor(255, 255, 255 ) ; 
	redImage.draw(15, 15, 128, 96);
	ofSetColor(255, 255, 255 , 125 );
	depthImage.draw(15, 15, 128, 96);
	ofSetColor(255, 255, 255 ) ; 
	//grayImage.draw(5, 5, 180, 120);
	contourFinder.draw(15, 15, 128, 96);
	ofFill() ;
	
	ofSetColor(255, 255, 255) ; 
	ofEnableSmoothing();
	
	for(int i=0; i< customParticles.size(); i++) 
	{
		customParticles[i].draw();
	}
	
	box2d.draw();
	box2d.drawGround() ;

	ofSetColor(255, 255, 255);
	std::list<TuioCursor*> alive_cursor_list = tuioServer->getTuioCursors();
	std::list<TuioCursor*>::iterator alive_cursor;
	
	ofEnableAlphaBlending() ; 
	
	for (alive_cursor=alive_cursor_list.begin(); alive_cursor!= alive_cursor_list.end(); alive_cursor++) 
	{
		TuioCursor *ac = (*alive_cursor);
		
		pan = ac->getX();
		float heightPct = ac->getY() ; 
		float absXSpeed = ac->getXSpeed() ;
		float absYSpeed = ac->getYSpeed() ;
		float xpos = pan * (float)ofGetWidth() ; 
		float ypos = heightPct * (float)ofGetHeight() ; 
		
		absXSpeed = ( absXSpeed < 0 ) ? absXSpeed * -1 : absXSpeed ; 
		absYSpeed = ( absYSpeed < 0 ) ? absYSpeed * -1 : absYSpeed ; 
			
		if ( absXSpeed > .30 || absYSpeed > .30 ) 
		{
			CustomParticle p;
			float r = ofRandom(.25f, 1.0f); //normalized diff
			p.setPhysics(15.0 * r, .2 * r, .65 * r );
			p.setup(box2d.getWorld(), xpos, ypos, (r*15) );
			p.setVelocity( ac->getXSpeed()*10 , ac->getYSpeed()*10 ) ; 
			p.color.r = ofRandom(10, 250);
			p.color.g = ofRandom(10, 250);
			p.color.b = ofRandom(10, 250);
			
			if ( customParticles.size() < 100 ) 
			{
				customParticles.push_back(p);
			}
			else
			{
				CustomParticle toBeReplaced = customParticles[ curToReplace ] ; 
				b2Body* b = toBeReplaced.body ;
				
				if ( b != NULL ) 
				{
					b2Body* b = toBeReplaced.body ; 
					box2d.world -> DestroyBody( toBeReplaced.body ) ;
					ofxBox2dBaseShape* base = (ofxBox2dBaseShape*)b->GetUserData();
					if(base) 
					{
						base->dead = true;
						box2d.world->DestroyBody(toBeReplaced.body );
					}	
				}
				
				customParticles[curToReplace] = p ; 
				curToReplace++ ; 
				if ( curToReplace > 99 ) 
					curToReplace = 0 ; 
			}
		}
		
		/*
		targetFrequency = 1500.0f * heightPct;
		phaseAdderTarget = (targetFrequency / (float) sampleRate) * TWO_PI;

		//Minimum movement otherwise things sound a bit jarring
		if ( absXSpeed > .10 && absYSpeed > .10 ) 
			checkTUIOHit( ac->getX() * ofGetWidth() , ac->getY() * ofGetHeight() ) ;
		 */
		//Draw that path!
		drawTuioPath( ac->getPath() ) ; 
	}
	
	//Quiet that annpying whine
	if ( alive_cursor_list.begin() == alive_cursor_list.end() ) 
	{
		volume = 0.05f ; 
	}
	
	//Debug Text
	/*
	ofSetColor(255, 255, 255);
	char reportStr[1024];
	sprintf(reportStr, "set near threshold %i (press: + -)\nset far threshold %i (press: < >)\nnum blobs found %i, fps: %i", nearThreshold, farThreshold, (int)contourFinder.blobs.size(), (int)ofGetFrameRate());
	ofDrawBitmapString(reportStr, 20, 650);
	 */
	ofDisableAlphaBlending() ; 
}

void TuioKinect::drawTuioPath( std::list<TuioPoint> acPath )
{
	int count = -1 ; 
	std::list<TuioPoint>::iterator curPoint;
	
	for ( curPoint = acPath.end() ; curPoint != acPath.begin() ; curPoint-- ) 
	{
		TuioPoint ac = (*curPoint) ; 
		float ratio = (float)(20-count)/20.0f ;
		float xpos = ac.getX()*ofGetWidth() ; 
		float ypos = ac.getY()*ofGetHeight() + ( count * 3 ) ; 
		float lastX , lastY ; 
		float midX , midY ; 
		int fadedValue = 255*(ratio) ; 
		if ( count < 25 ) 
		{
			ofSetColor(255,fadedValue,fadedValue ,fadedValue) ;
			ofCircle(xpos,ypos ,15*ratio);
		}
		else
			break ; 
		
		count++ ; 
	}
}
void TuioKinect::checkTUIOHit(float x , float y)
{
	float widthStep = ofGetWidth() / 3.0f;
	float height = ofGetHeight() ; 
	ofEnableAlphaBlending() ; 
	if (x < widthStep)
	{
		ofSetColor(255, 0, 0,35) ; 
		ofRect(0, 0, widthStep, height );
		float pct = x / widthStep;
		synth.play();
		synth.setSpeed( 0.1f + ((float)(ofGetHeight() - y) / (float)ofGetHeight())*10);
		synth.setPan(pct);
	}
	else if (x >= widthStep && x < widthStep*2)
	{
		ofSetColor(0, 255, 0,15) ; 
		ofRect( widthStep, 0, widthStep, height );
		beats.play();
	} 
	else 
	{
		ofSetColor(0, 0, 255,35) ; 
		ofRect( widthStep*2, 0, widthStep, height );
		float pct = x / widthStep;
		synth1.play();
		synth1.setSpeed( 0.1f + (y / (float)ofGetHeight())*5);
		synth1.setPan(pct);
	}
	ofDisableAlphaBlending() ; 
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

void TuioKinect::audioRequested 	(float * output, int bufferSize, int nChannels)
{
	//pan = 0.5f;
	volume = 0.25f ; 
	float leftScale = 1 - pan;
	float rightScale = pan;
	
	// sin (n) seems to have trouble when n is very large, so we
	// keep phase in the range of 0-TWO_PI like this:
	while (phase > TWO_PI){
		phase -= TWO_PI;
	}
	
	if ( bNoise == true){
		// ---------------------- noise --------------
		for (int i = 0; i < bufferSize; i++){
			lAudio[i] = output[i*nChannels    ] = ofRandomf() * volume * leftScale;
			rAudio[i] = output[i*nChannels + 1] = ofRandomf() * volume * rightScale;
		}
	} else {
		phaseAdder = 0.95f * phaseAdder + 0.05f * phaseAdderTarget;
		for (int i = 0; i < bufferSize; i++){
			phase += phaseAdder;
			float sample = sin(phase);
			lAudio[i] = output[i*nChannels    ] = sample * volume * leftScale;
			rAudio[i] = output[i*nChannels + 1] = sample * volume * rightScale;
		}
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

