#include "testApp.h"



//--------------------------------------------------------------
void testApp::setup()
{
	//XCODE fix from OF forums
	#ifdef TARGET_OSX    
	
	// Get the absolute location of the executable file in the bundle.
	CFBundleRef appBundle     = CFBundleGetMainBundle();
	CFURLRef   executableURL = CFBundleCopyExecutableURL(appBundle);
	char execFile[4096];
	if (CFURLGetFileSystemRepresentation(executableURL, TRUE, (UInt8 *)execFile, 4096))
	{
		// Strip out the filename to just get the path
		string strExecFile = execFile;
		int found = strExecFile.find_last_of("/");
		string strPath = strExecFile.substr(0, found);
		
		// Change the working directory to that of the executable
		if(-1 == chdir(strPath.c_str())) {
			ofLog(OF_LOG_ERROR, "Unable to change working directory to executable's directory.");
		}
	}
	else {
		ofLog(OF_LOG_ERROR, "Unable to identify executable's directory.");
	}
	CFRelease(executableURL);
	#endif
	
	//kinect.init(true);  //shows infrared image
	kinect.init();
	kinect.setVerbose(true);
	kinect.open();
	
	
	//kinect.init(true);  //shows infrared image
	kinect.init();
	kinect.setVerbose(true);
	kinect.open();
	
	colorImg.allocate(kinect.width, kinect.height);
	grayImage.allocate(kinect.width, kinect.height);
	grayThresh.allocate(kinect.width, kinect.height);
	grayThreshFar.allocate(kinect.width, kinect.height);
	
	nearThreshold = 180;
	farThreshold  = 229;
	bThreshWithOpenCV = true;
	
	ofSetFrameRate(30);

	movie.loadMovie("movies/stripes.mov");
	movie.play();
	
	angle = 25;	
	
	//audio setup	
	ofSoundStreamSetup(0,2,this, 44100, 256, 4);	
	left = new float[256];
	right = new float[256];
	offset = ofPoint( .15 , -.55 , .75 ) ; 

	
	
	/*
	mask.allocate(640, 480, OF_IMAGE_COLOR_ALPHA) ;
	movie.loadMovie("movies/stripes.mov");
	movie.play();
	alphaPixels = mask.getPixels() ; 
	maskedImage.allocate(mask.width , mask.height, GL_RGBA ) ; 
	 */
	
}

//--------------------------------------------------------------
//--------------------------------------------------------------
void testApp::update()
{
	movie.update() ; 
	//ofEnableAlphaBlending() ; 
	//ofSetColor ( 15 , 15 , 15 , 15 ) ; 
	//ofRect( 0 , 0 , ofGetWidth() , ofGetHeight() ) ; 
	ofBackground(15	, 15, 15 );
	//ofDisableAlphaBlending();
	kinect.update();
	
	grayImage.setFromPixels(kinect.getDepthPixels(), kinect.width, kinect.height);
	
	//we do two thresholds - one for the far plane and one for the near plane
	//we then do a cvAnd to get the pixels which are a union of the two thresholds.	
	if( bThreshWithOpenCV )
	{
		grayThreshFar = grayImage;
		grayThresh = grayImage;
		grayThreshFar.threshold(farThreshold, true);
		grayThresh.threshold(nearThreshold);
		cvAnd(grayThresh.getCvImage(), grayThreshFar.getCvImage(), grayImage.getCvImage(), NULL);
	}
	else
	{
		
		//or we do it ourselves - show people how they can work with the pixels
		
		unsigned char * pix = grayImage.getPixels();
		int numPixels = grayImage.getWidth() * grayImage.getHeight();
		
		for(int i = 0; i < numPixels; i++){
			if( pix[i] > nearThreshold && pix[i] < farThreshold ){
				pix[i] = 255;
			}else{
				pix[i] = 0;
			}
		}
	}
	
	//update the cv image
	grayImage.flagImageChanged();
	
    // find contours which are between the size of 20 pixels and 1/3 the w*h pixels.
    // also, find holes is set to true so we will get interior contours as well....
    contourFinder.findContours(grayImage, 2000, (kinect.width*kinect.height)/8, 1, false);
}

//--------------------------------------------------------------
void testApp::draw()
{
	
	glBlendFunc(GL_SRC_COLOR, GL_ONE);
	ofEnableAlphaBlending() ; 
	//ofSetColor(255,255,255 , 25 );
//	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	//Additive blend mode

	int _y = ofGetHeight() /2 ; 
	int _x = ofGetWidth()/2 ;
	int frameCount = ofGetFrameNum() % 25 ; 
	ofPoint mid , newPos , prevPos ; 
	prevPos = ofPoint( 0 ,  sin( frameCount)*25+_y ) ;
	for (int i = 0; i < 256; i+=4)
	{	
	//	ofLine(i*4,sin(i+frameCount)*25+_y,_x ,_y+left[i]*1000.0f);
		//ofSetColor( i , 255-i, 150 , i/2  ) ; 
//		float newX = (float)i * 4 ; 
		//ofCircle( newX , sin(i+frameCount)*100+_y , sin(i) * 15 + 10 ) ; 
		ofSetColor( i , 255-i , 150 , i/10 ) ;
		ofBezier( i * 4 ,sin(i)*200+_y,  _x , _y , _x , _y , _x ,_y+left[i]*3000.0f  );
	}
	
	//ofEnableAlphaBlending() ; 

	drawSoundWave() ;
	ofSetColor(255, 255, 255);
	//kinect.drawDepth(10, 10, 180, 135);
//	kinect.draw(420, 10, 400, 300);
	
	grayImage.draw(10, 10, 180, 135 );
	contourFinder.draw(10, 10, 180, 135);
	ofSetColor(255, 255, 255);
	ofFill() ;
	
	
	ofPushMatrix();
	ofTranslate(420, 320, 200);
	// point cloud is commented out because we need a proper camera class to explore it effectively
	drawPointCloud();
	ofPopMatrix();
	
	ofSetColor(255, 255, 255);
	ofFill() ; 
	char reportStr[1024];
	sprintf(reportStr, "offset is: %f , %f , %f",offset.x , offset.y ,offset.z) ;
	ofDrawBitmapString(reportStr, 20, 675);
	
	char newStr[1024];
	sprintf(newStr, "using opencv threshold = %i (press spacebar)\nset near threshold %i (press: + -)\nset far threshold %i (press: < >) num blobs found %i, fps: %f",bThreshWithOpenCV, nearThreshold, farThreshold, contourFinder.nBlobs, ofGetFrameRate());
	ofDrawBitmapString(newStr, 20, 700);

	//ofDrawBitmapString(reportStr, 20, 690);
	//ofDrawBitmapString("tilt angle: " + ofToString(angle),20,670);
	}
/*
//--------------------------------------------------------------
void testApp::draw()
{
	ofSetColor(255, 255, 255);
	//ofEnableAlphaBlending();
	//mask.draw(10, 10) ;
	//ofDisableAlphaBlending() ; 
	/*
//	//this code was on the OF forums! Super helpful and awesome to use some openGL to mask video	
	// make sure alpha blending is enabled
	ofEnableAlphaBlending();
	
    // set up multi-texturing
	glActiveTexture(GL_TEXTURE0);
	movie.getTextureReference().bind(); // may generate an allocation warning
    glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_EXT);
    glTexEnvf (GL_TEXTURE_ENV, GL_COMBINE_RGB_EXT, GL_REPLACE);
	
	glActiveTexture(GL_TEXTURE1);
	alphaImg.getTextureReference().bind(); // may generate an allocation warning
    glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_EXT);
    glTexEnvf (GL_TEXTURE_ENV, GL_COMBINE_RGB_EXT, GL_BLEND);
	
	// render masked and mask images as one multi-textured quad
	glBegin(GL_QUADS);
	glMultiTexCoord2f(GL_TEXTURE0, 0.0f, 480.0f);
	glMultiTexCoord2f(GL_TEXTURE1, 0.0f, 480.0f);
	glVertex3f(0.0f, 480.0f, 0);
	glMultiTexCoord2f(GL_TEXTURE0, 0.0f, 0.0f);
	glMultiTexCoord2f(GL_TEXTURE1, 0.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 0);
	glMultiTexCoord2f(GL_TEXTURE0, 640.0f, 0.0f);
	glMultiTexCoord2f(GL_TEXTURE1, 640.0f, 0.0f);
	glVertex3f(640.0f, 0.0f, 0);
	glMultiTexCoord2f(GL_TEXTURE0, 640.0f, 480.0f);
	glMultiTexCoord2f(GL_TEXTURE1, 640.0f, 480.0f);
	glVertex3f(640.0f, 480.0f, 0);
	glEnd();
	
    // properly unbind the textures
    alphaImg.getTextureReference().unbind();
	glActiveTexture(GL_TEXTURE0);
	movie.getTextureReference().unbind();
	
	// disable alpha blending again
	ofDisableAlphaBlending();
	
	//grayImage.draw(10, 10, 640, 480 );
	
	//movie.draw(10,10);

	ofPushMatrix();
	ofTranslate(420, 320);
	// point cloud is commented out because we need a proper camera class to explore it effectively
	drawPointCloud();
	ofPopMatrix();
	
	grayImage.draw( 10 , 10 ) ; 

	ofSetColor(255, 255, 255);
	ofDrawBitmapString("accel is: " + ofToString(kinect.getMksAccel().x, 2) + " / " 
									+ ofToString(kinect.getMksAccel().y, 2) + " / "
									+ ofToString(kinect.getMksAccel().z, 2), 20, 658 );
	char reportStr[1024];
	sprintf(reportStr, "using opencv threshold = %i (press spacebar)\nset near threshold %i (press: + -)\nset far threshold %i (press: < >) num blobs found %i, fps: %f",bThreshWithOpenCV, nearThreshold, farThreshold, contourFinder.nBlobs, ofGetFrameRate());
	ofDrawBitmapString(reportStr, 20, 690);
	ofDrawBitmapString("tilt angle: " + ofToString(angle),20,670);
	
	 
}
*/
void testApp::audioReceived 	(float * input, int bufferSize, int nChannels){	
	// samples are "interleaved"
	for (int i = 0; i < bufferSize; i++){
		left[i] = input[i*2];
		right[i] = input[i*2+1];
	}
	
}

void testApp::drawSoundWave() 
{
	ofSetColor(150, 150 , 0 ) ; 
	ofEnableAlphaBlending();
	int w = ofGetWidth() ; 
	int h = ofGetHeight() ; 
	float _y , ratio , _z , sample1 ;
	for (int i = 0; i < 25; i++)
	{
		//ratio = (float)i / (float)256 ; 
		
		ratio = (float)i / (float)25 ; 
		_y = ratio * h ; 
		
		_z = left[i] * 40.0f +- 1.0f ; 
		ofTranslate(0, _y, _z  );
		for ( int a = 1 ; a < 50 ; a++ )
		{
			//sample1 = left[i+a] * 44100 ; 
			//ofCircle( a * 200 , _y , left[i] * -10.0f ) ;  
			ofSetColor( sample1 / 12 , 255 - sample1 / 12 , 24 , 15 ) ; 
			ofCircle( a * 50 , 0 , 15 + left[i] * 15 ); 
		}
		ofTranslate(0, -_y, -_z );
		
		//ofSetColor(0, 12, 245);
		//ofLine(ratio * w ,h/2 ,ratio+1.0f * w , h/2 + left[i] * 4000);
	}
	ofDisableAlphaBlending();


	
}

void testApp::drawPointCloud()
{
	ofScale(1000, 1000, 700);
	ofTranslate(offset.x, offset.y, offset.z);
	int w = 640;
	int h = 480;
	
	
	
	if ( contourFinder.blobs.size() > 0 ) 
	{	
		boundsRect = contourFinder.blobs[0].boundingRect ;  
		lastRect = boundsRect ;
		
		//ofRotateY(mouseX);
		//ofEnableAlphaBlending() ; 
		unsigned char * colorPixels = movie.getPixels() ; 
		unsigned char * grayPixels = grayImage.getPixels();
		float* distancePixels = kinect.getDistancePixels();
		glBegin(GL_POINTS);
		int step = 1;
		
		for(int y = 0; y < h; y += step)
		{
			for(int x = 0; x < w; x += step) 
			{
				int pos = (y * w + x);  
				
				bool hitBounds = inside ( x , y , boundsRect ) ; 
				if ( hitBounds ) 
				{
					if ( grayPixels[pos] == 255 ) 
					{
						ofPoint cur = kinect.getWorldCoordinateFor(x, y);
						ofSetColor(colorPixels[pos*3],colorPixels[pos*3+1], colorPixels[pos*3+2] ) ; 
						glVertex3f(cur.x, cur.y, -cur.z);	
					}
				}
			}
		}
		glEnd();

		
	}
	

		//ofDisableAlphaBlending() ; 
}

bool testApp::inside(float px, float py, ofRectangle rect )
{
	if( px > rect.x && py > rect.y && px < rect.x + rect.width && py < rect.y + rect.height ){
		return true;
	}
	return false;
}

//--------------------------------------------------------------
void testApp::exit(){
	kinect.close();
}

//--------------------------------------------------------------
void testApp::keyPressed (int key)
{
	switch (key)
	{
		case ' ':
			bThreshWithOpenCV = !bThreshWithOpenCV;
		break;
	
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
	
		case 'q':
			movie.loadMovie("movies/noise.mov");
			movie.play() ; 
			break ; 
				
		case 'i':
			offset.y+= .05f;
			break ; 
			
		case 'j' :
			offset.x -= .05f ; 
			break ; 
			
		case 'k':
			offset.y -= .05f ; 
			break ; 
			
		case 'l':
			offset.x += .05f ; 
			break;

		case 'e':
			movie.loadMovie("movies/stripes.mov") ; 
			movie.play() ; 
			break ; 

		case 'z':
			offset.z+=.05f ;
			break ; 
			
		case 'x':
			offset.z-=.05f ; 
			break ; 
			
		case OF_KEY_UP:
			angle++;
			if(angle>30) angle=30;
			kinect.setCameraTiltAngle(angle);
			break;

		case OF_KEY_DOWN:
			angle--;
			if(angle<-30) angle=-30;
			kinect.setCameraTiltAngle(angle);
			break;
	}
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y)
{}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button)
{}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button)
{}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button)
{}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h)
{}

