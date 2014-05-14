//todo: 

#include "testApp.h"


//--------------------------------------------------------------
void testApp::setup()
{
	ofSetFrameRate(60);

	numBoards = ceil(float(numLEDs) / 16.0);
    ofLog() << "numboards: " << numBoards;
    
    
	
    ofSetVerticalSync(false);
	cameraWidth		= 320;
	cameraHeight	= 240;
	cellSize = 320/16;
	cellSizeFl  = (float)cellSize;
	numPixels = (cameraWidth/cellSize) * (cameraHeight/cellSize);
    ofLog() << "numPixels: " << numPixels;
    
    numCellsX = cameraWidth / cellSize;
    numCellsY = cameraHeight / cellSize;
    
    ofLog() << "numCellsX: " << numCellsX << " numCellsY: " << numCellsY;
    

	displayCoeff = 1;
	
	//videoGrabber.listDevices();
	videoGrabber.setDesiredFrameRate(60);
	videoGrabber.initGrabber(cameraWidth, cameraHeight);
	//pixels = new unsigned char[numPixels];
    
    background.setLearningTime(400);
    background.setThresholdValue(10);
    
    
    //noise Vars
    noiseAmp = 254;
    time = 0;
    timeInc = 0.01;
    for(int i = 0; i < numLEDs; i++)
    {
        noiseCoeffs.push_back(ofRandom(1000));
    }
    
    //create the socket and set to send to 127.0.0.1:11999
	udpConnection.Create();
	udpConnection.Connect("169.254.0.2",11999);
	udpConnection.SetNonBlocking(true);
    
    
    gui.setup("panel");
    gui.add(backgroundThresh.setup("bgThresh", 21, 0, 255));
    gui.add(learningTime.setup("learnTime", 50, 30, 2000));
    gui.add(reset.setup("reset background"));
    gui.add(revealAmount.setup("reveal video", 0.5, 0.0, 1.0));
    gui.add(timeInc.setup("Noise Speed", 0.001, 0.0001, 0.005));
    gui.add(lowerLim.setup("Lower Limit", 0, 0, 125));
    gui.add(uppderLim.setup("Upper Limit", 255, 126, 255));
    gui.add(bShowNoiseVals.setup("Show Noise Values", false));
    gui.add(bShowMask.setup("Show Binary Mask", false));
    gui.add(bShowIndexVals.setup("Show Index Values", false));
    gui.add(lightsOn.setup("Lights On", false));
    gui.add(displayCoeff.setup("size of simulation", 3, 0.5, 5));
    
    gui.setPosition(10, cameraHeight + 10);
    gui.loadFromFile("settings.xml");

}


/////////////////////////////// UPDATE /////////////////////////////////////////
void testApp::update()
{
    background.setLearningTime(learningTime);
    background.setThresholdValue(backgroundThresh);
    if(reset)
    {
        background.reset();
    }

    int shiftX;
    int shiftY;
    
    
    //update the video grabber
	videoGrabber.update();

    //if a new frame is available do this:
	if(videoGrabber.isFrameNew())
    {
        regImage.setFromPixels(videoGrabber.getPixelsRef());
        background.update(regImage, thresholded);
        thresholded.update();

        for(int i = 0; i < numCellsY; i ++)
        {
            shiftY = i * cellSize;
            for(int j = 0; j < numCellsX; j ++)
            {
                shiftX = j * cellSize;
                int total = 0;
                for(int x = shiftX; x < shiftX + cellSize; x ++)
                {
                    for(int y = shiftY; y < shiftY + cellSize; y++)
                    {
                        total = total + thresholded.getColor(x,y).getLightness();
                        //ofLog() << "total: " << total;
                    }
                }
               br[ 1 + (i*numCellsX) + j ] = total / (cellSize*cellSize);
            }
        }
    }
    
    

   // ofLog() << message;

    makeNoise();
    
    for(int i = 0; i < numLEDs; i++)
    {
        finalVal[i] = (int)(br[i] * revealAmount) + (int)(noiseVal[i] * (1-revealAmount));
        finalVal[i] = ofMap(finalVal[i], 0, 255, lowerLim, uppderLim);
    }
    
    if(lightsOn)
    {
        sendLights();
    }
}


/////////////////////////////// DRAW /////////////////////////////////////////
void testApp::draw(){
	
    
    int leftSpace = 100;
	int space = 200-cellSize;
	int height = 230;

	ofBackground(10, 10, 10);

	ofSetColor(255);
    gui.draw();
	
    ofPushMatrix();
        ofTranslate(10,10);
        videoGrabber.draw(0, 0);
        if(bShowMask) thresholded.draw(0, 0);
        ofNoFill();
        ofRect(0, 0, videoGrabber.getWidth(), videoGrabber.getHeight());
    ofPopMatrix();

	for(int i = 0; i < numCellsY; i++)
		{
			for(int j = 0; j < numCellsX; j++)
			{
				ofFill();
                ofPushMatrix();
				ofTranslate(cameraWidth - 30, cameraHeight + 20);
                ofTranslate(j*cellSize* displayCoeff, i*cellSize * displayCoeff);
				
                    ofSetColor(finalVal[i*numCellsX+j]);
					ofRect(0.0,0.0,cellSizeFl*displayCoeff, cellSizeFl*displayCoeff);
                    ofSetColor(255,0,0);
                    if(bShowNoiseVals) ofDrawBitmapString(ofToString(noiseVal[(i*numCellsX) + j]), 0,10);
                    if(bShowIndexVals) ofDrawBitmapString(ofToString(i*numCellsX + j), 0, 10);
				ofPopMatrix();
			}
		}
	//ofPopMatrix();
	

	ofSetColor(255);
	ofDrawBitmapString("FPS: " + ofToString(ofGetFrameRate(), 0), 5, ofGetWindowHeight()-5);
    
}




/////////////////////////////// NOISE /////////////////////////////////////////
void testApp::makeNoise(void)
{
    
    for(int j = 0; j < numCellsX; j++)
    {
        for(int i = 0; i < numCellsY; i++)
        {
            noiseVal[(i*numCellsX) + j] = abs(noiseAmp * ofNoise(time * (j+10), time * (10 - i)) );
            
            
        }
    }
    time += timeInc;
}


//////////////////////////// RUN LIGHTS //////////////////////////////////
void testApp::sendLights(){
    
    
    
    
    string message = "";
    for(int i = 0; i < numLEDs; i++)
    {
        message+= ofToString(i) + "|" + ofToString(finalVal[i]) + "[/p]";
        //ofLog() << "index: " << i << " || value: " << (int)finalVal[i];
    }
    udpConnection.Send(message.c_str(),message.length());
    //ofLog() << "Message Length: " << message.length();
    
    
}


///////////////////////// KEY PRESSED //////////////////////////////
void testApp::keyPressed(int key){
    if(key == ' ')
    {
           }
    if(key == 't')
    {
        
    }
}




