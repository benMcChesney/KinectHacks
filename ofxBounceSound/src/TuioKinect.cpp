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
#include "ofxVec2f.h"

ofPoint kCursors[ 2 ] ; 

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

	nearThreshold = 180;
	farThreshold  = 230;
	//farThreshold  = 100;

	TuioTime::initSession();	
	tuioServer = new TuioServer();
	tuioServer->setSourceName("TuioKinect");
	tuioServer->enableObjectProfile(false);
	tuioServer->enableBlobProfile(false);
	
	for (int i=0;i<32;i++)
		tuioPointSmoothed[i] = NULL;
	
	kCursors[0] = ofPoint() ; 
	kCursors[1] = ofPoint() ; 
	
	//soundSamples
	synth.loadSound("sounds/synth.wav");
	synth.setVolume(0.75f);
	synth.setMultiPlay(true);
	
	//box2D setup
	box2d.init();
	box2d.setGravity(0, 10);
	box2d.doSleep = true ; 
	box2d.bCheckBounds = true ; 
	box2d.createFloor();
	box2d.checkBounds(true);
	box2d.createBounds(0, 0, ofGetWidth(), ofGetHeight() ) ; 
	box2d.getWorld()->SetContactListener( &contacts );		// register contact class.
	box2d.setFPS(30.0);
	holdingLength = 0 ; 
	curToReplace = 0 ; 
	ofSetFrameRate(30);
	
	birthDelay = 0 ; 
	
	for ( int i = 0 ; i < 6 ; i++ ) 
	{
		//platforms
		int w = ofRandom(25,75) ; 
		int h =  ofRandom( 100 , 150 ) ; 
		PlatformRect rect = PlatformRect( w , h ) ; 
		//rect.setPhysics(20.0, .1 , 0.0 ) ;
		rect.setup(box2d.getWorld(), ofRandom(200,800), ofRandom(200,600 ), w , true ) ; 
		platforms.push_back(rect);
		
	}
	

}



//--------------------------------------------------------------
void TuioKinect::update()
{
	birthDelay++ ; 
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
	
	//detect all blobs
	std::vector<ofxCvBlob>::iterator blob;
	for (blob=contourFinder.blobs.begin(); blob!= contourFinder.blobs.end(); blob++) 
	{
		float xpos = (*blob).centroid.x;
		float ypos = (*blob).centroid.y;
		
		TuioPoint tp(xpos/kinect.width,ypos/kinect.height);
		TuioCursor *tcur = tuioServer->getClosestTuioCursor(tp.getX(),tp.getY());
		
		//Adjust here to change minumum distance for new points
		if ((tcur==NULL) || (tcur->getDistance(&tp)>0.1)) 
		{ 
			tcur = tuioServer->addTuioCursor(tp.getX(), tp.getY());
			updateKalman(tcur->getCursorID(),tcur);
		} 
		else
		{
			TuioPoint kp = updateKalman(tcur->getCursorID(),tp);
			tuioServer->updateTuioCursor(tcur, kp.getX(), kp.getY());
		}
		
		xpos = tp.getX() * ofGetWidth() ; 
		ypos = tp.getY() * ofGetHeight() ; 
		
		if ( numBlobs < 3 )
		{
			kCursors[count].x = xpos ; 
			kCursors[count].y = ypos ; 
		}
		count++ ;
	}
	
	float strength = 5.0f;
	float damping  = 0.5f;
	
	if ( numBlobs == 2 ) 
	{
		if ( holdingLength < 40 ) 
			holdingLength++ ;

		float holdingRatio = 0.05f + (float)holdingLength/40.0f ;
		float minDis   = 100.0f;
		
		ofPoint c1 = kCursors[1] ; 
		ofPoint c0 = kCursors[0] ; 
		
		float newStrength = (float)holdingLength*1.05 * strength ; 
		
		for(int i=0; i< customParticles.size() ; i++) 
		{	
			CustomParticle& p = customParticles[i] ; 
			if ( i % 2 == 0 ) {
				p.addAttractionPoint( c0.x, c0.y, newStrength , minDis);
			}
			else {
				p.addAttractionPoint( c1.x, c1.y, newStrength , minDis);
			}
			p.addDamping(damping, damping);
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

	box2d.update() ; 
}


//--------------------------------------------------------------
void TuioKinect::draw()
{
	float height = (float)ofGetHeight();
	float width = (float)ofGetWidth() ; 
	
	ofSetColor(0,0,0,200) ; 
	
	//Additive blend mode
	glBlendFunc(GL_SRC_COLOR, GL_ONE);
	
	ofSetColor(255, 255, 255) ; 
	ofEnableSmoothing();
	
	for(int i=0; i< platforms.size(); i++) 
	{
		platforms[i].draw();
	}
	
	for(int i=0; i< customParticles.size(); i++) 
	{
		customParticles[i].draw(0);
	}
	
	box2d.draw();
	box2d.drawGround() ;

	ofSetColor(255, 255, 255);
	std::list<TuioCursor*> alive_cursor_list = tuioServer->getTuioCursors();
	std::list<TuioCursor*>::iterator alive_cursor;
	
	for (alive_cursor=alive_cursor_list.begin(); alive_cursor!= alive_cursor_list.end(); alive_cursor++) 
	{
		TuioCursor *ac = (*alive_cursor);
		
		float absXSpeed = ac->getXSpeed() ;
		float absYSpeed = ac->getYSpeed() ;
		float xpos = ac->getX() * (float)ofGetWidth() ; 
		float ypos = ac->getY() * (float)ofGetHeight() ; 
		
		absXSpeed = ( absXSpeed < 0 ) ? absXSpeed * -1 : absXSpeed ; 
		absYSpeed = ( absYSpeed < 0 ) ? absYSpeed * -1 : absYSpeed ; 
			
		if ( absXSpeed > .30 || absYSpeed > .30 ) 
		{	
			int _size  = customParticles.size() ;
			if ( _size < 20 ) 
			{
				CustomParticle p;
				float r = ofRandom(.25f, 1.0f); //normalized diff
				p.setPhysics(5.0 * r, .2 * r, .45 * r );
				p.setup(box2d.getWorld(), xpos, ypos, (r*30) );
				p.setVelocity( ac->getXSpeed()*10 , ac->getYSpeed()*10 ) ; 
				customParticles.push_back(p);
			}
		}
		
		//Draw that path!
		drawTuioPath( ac->getPath() ) ; 
	}
	
	//Debug Text
	ofSetColor(255, 255, 255);
	char reportStr[1024];
	sprintf(reportStr, "set near threshold %i (press: + -)\nset far threshold %i (press: < >)\nnum blobs found %i, fps: %i", nearThreshold, farThreshold, (int)contourFinder.blobs.size(), (int)ofGetFrameRate());
	ofDrawBitmapString(reportStr, 20, 650);
	 
	ofEnableAlphaBlending() ; 

	ofSetColor ( 10 , 10 , 10 ); 
	ofFill() ; 
	ofSetLineWidth( 2 ) ;
	ofRect(0, 0, 40, 40 ) ; 
		
	ofSetColor(255, 255, 255 ) ; 
	ofFill() ;
	
	grayImage.draw(5, 5, 144, 108 );
	contourFinder.draw(5, 5, 144, 108 );
	ofFill() ; 
	
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
void TuioKinect :: box2dContactEventHandler ( const b2ContactPoint* point )
{
	bool isFirstHit ; 
	CustomParticle& firstHit = customParticles[0] ; 
	for( int i=0; i<customParticles.size(); i++ )
	{
		CustomParticle& p = customParticles[ i ];
		if ( p.checkCollision == true && p.body != NULL ) 
		{
			for( b2Shape* s=p.body->GetShapeList(); s; s=s->GetNext() )
			{
				b2Body* b = p.body ; 
				if ( b != NULL ) 
				{
					ofxBox2dBaseShape* base = (ofxBox2dBaseShape*)b->GetUserData();
					if ( base != NULL )
					{
						base->dead = true;
						box2d.world -> DestroyBody( b ) ; 
						customParticles.erase( customParticles.begin() + i  ) ; 
					}
				}
				
				if( point->shape1 == s || point->shape2 == s )
				{	
					if ( isFirstHit == false ) 
					{
						isFirstHit = true ; 
						firstHit = p ; 
					}
					else
					{
						float synthSpeed = (firstHit.body->GetLinearVelocity().x + firstHit.body->GetLinearVelocity().y ) ; 
						float synthPan = (p.body->GetLinearVelocity().x + p.body->GetLinearVelocity().y ) ;
						if ( synthSpeed < 0.5f && synthSpeed < 0.8f ) 
						{
							ofColor c = ofColor() ; 
							c.r = 35 ; 
							c.g = 34 ; 
							c.b = 36 ; 
							p.setColor(c);
							p.checkCollision = false ; 
							synth.play();
							synth.setSpeed( synthSpeed );
							synth.setPan( synthPan );
						}
					}
				}
				else 
				{
					float synthSpeed = (firstHit.body->GetLinearVelocity().x + firstHit.body->GetLinearVelocity().y ) ; 
					float synthPan = (p.body->GetLinearVelocity().x + p.body->GetLinearVelocity().y ) ;
					if ( synthSpeed > 5.5f || synthPan > 4.5f ) 
					{
						synthSpeed /= 10 ; 
						synthPan /= 10 ; 
						ofColor c = ofColor() ; 
						c.r = ofRandom(15,255); 
						c.g = ofRandom(15,255); 
						c.b = ofRandom(15,255) ;
						p.setColor(c) ; 
						firstHit.setColor(c);
						p.checkCollision = false ; 
						synth.play();
						synth.volume = .5f ; 
						synth.setSpeed( synthSpeed );
						synth.setPan( synthPan );
					}
				}
			}
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

//--------------------------------------------------------------	CONTACT LISTNER CLASS.

void MyContactListener :: Add( const b2ContactPoint* point )
{
	TuioKinect* appPtr = ((TuioKinect*)ofGetAppPtr());
	appPtr->box2dContactEventHandler( point );
}

void MyContactListener :: Remove(const b2ContactPoint* point)
{
	//
}