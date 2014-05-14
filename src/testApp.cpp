//todo: 

#include "testApp.h"


//--------------------------------------------------------------
void testApp::setup()
{

	ofSetFrameRate(60);

	ofSetFrameRate(30);
    //ofSetLogLevel(OF_LOG_VERBOSE);


	numBoards = ceil(float(numLEDs) / 16.0);
    ofLog() << "numboards: " << numBoards;
    
    numLightsInRow[0] =   11;
    numLightsInRow[1] =17;
    numLightsInRow[2] = 23;
    numLightsInRow[3] =  23;
    numLightsInRow[4] = 23;
    numLightsInRow[5] = 23;
    numLightsInRow[6] = 23;
    numLightsInRow[7] = 23;
    numLightsInRow[8] = 23;
    numLightsInRow[9] = 23;
    numLightsInRow[10] = 23;
    numLightsInRow[11] = 22;
    numLightsInRow[12] = 19;
    numLightsInRow[13] = 16;
    numLightsInRow[14] = 13;
    numLightsInRow[15] = 11;
    
    
	
    ofSetVerticalSync(false);
	cameraWidth		= 320;
	cameraHeight	= 240;
	cellSize = cameraHeight/numRows;
	numPixels = (cameraWidth/cellSize) * (cameraHeight/cellSize);
    
    int numberCols = numCols;
    int numberRows = numRows;
    ofLog() << "number rows * columns: " << numRows * numCols;
    ofLog() << "numPixels: " << numPixels;
    ofLog() << "cellSize: " << cellSize;
    ofLog() << "numCols: " << numberCols << " numRows: " << numberRows;
    
    ofLog() << "numCellsX: " << cameraWidth/cellSize << " numCellsY: " << cameraHeight/cellSize;
    

	displayCoeff = 1;
	

	//videoGrabber.listDevices();
	videoGrabber.setDesiredFrameRate(60);

	videoGrabber.initGrabber(cameraWidth, cameraHeight);
	regImage.allocate(cameraWidth, cameraHeight, OF_IMAGE_COLOR);
    
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
    
    
    gui.setup("");
    gui.add(bShowMask.setup("Show Binary Mask", false));
    gui.add(backgroundThresh.setup("bgThresh", 21, 0, 255));
    gui.add(learningTime.setup("learnTime", 50, 1, 200));
    gui.add(reset.setup("reset background"));
    gui.add(revealAmount.setup("reveal video", 0.5, 0.0, 1.0));
    gui.add(timeInc.setup("Noise Speed", 0.001, 0.0001, 0.005));
    gui.add(lowerLim.setup("Lower Limit", 0, 0, 125));
    gui.add(uppderLim.setup("Upper Limit", 255, 126, 255));
    gui.add(bShowNoiseVals.setup("Show Noise Values", false));
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
        
        
        for(int i = 0; i < numRows; i ++)
        {
            shiftY = i * cellSize;
            int rowStepper = 0;
            for(int j = (numCols - numLightsInRow[i]); j < numCols; j ++)
            {
                
                shiftX = j * cellSize;
                int total = 0;
                for(int x = shiftX; x < shiftX + cellSize; x ++)
                {
                    for(int y = shiftY; y < shiftY + cellSize; y++)
                    {
                        total = total + thresholded.getColor(x,y).getLightness();
                    }
                }
                
                
                int loc = (arraySum(i)+ rowStepper) - numLightsInRow[i];
                br[loc] = total / (cellSize*cellSize);
                ofLogVerbose() << "br[" << loc << "]: " << br[loc];
                rowStepper++;
            }
        }
       
    }
   // makeNoise();//make the noise
   
    //combine video and noise for all values
    for(int i = 0; i < numPixels; i++)
    {
        //scale video and noise appropriately, then combine
        finalVal[i] = (int)(br[i] * revealAmount) + (int)(noiseVal[i] * (1-revealAmount));
        //map to upper and lower limits as per GUI
        finalVal[i] = ofMap(finalVal[i], 0, 255, lowerLim, uppderLim);
    }
    
    if(lightsOn) sendLights();          //send it to raspberry Pi and LED drivers.
}

/////////////////////////// arraySum ////////////////////////////////////////
int testApp::arraySum(int index){
    int sum = 0;
    for(int i = 0; i <= index; i++)
    {
        sum += numLightsInRow[i];
    }
    ofLogVerbose() << "arraySum: " << sum;
    return sum;
}


///////////////////////////////////// DRAW /////////////////////////////////////////
void testApp::draw(){
	
    
    int leftSpace = 100;
	int space = 200-cellSize;
	int height = 230;

	ofBackground(10, 10, 10);

	ofSetColor(255);
    gui.draw();
	
    ofPushMatrix();
        ofTranslate(10,10);
        regImage.draw(0, 0);
        if(bShowMask) thresholded.draw(0, 0);
        ofNoFill();
        ofRect(0, 0, videoGrabber.getWidth(), videoGrabber.getHeight());
    ofPopMatrix();

	
    for(int i = 0; i < numRows; i++)
		{
			for(int j = (numCols - numLightsInRow[i]); j < numCols; j++)
			{
				ofFill();
                ofPushMatrix();
				ofTranslate(cameraWidth - 30, cameraHeight + 20);
                ofTranslate(j*cellSize* displayCoeff, i*cellSize * displayCoeff);
				
                    ofSetColor(finalVal[arraySum(i) + j]);
					ofRect(0.0,0.0,(float)cellSize*displayCoeff, (float)cellSize*displayCoeff);
                
                
                ofSetColor(255,0,0);
                    if(bShowNoiseVals) ofDrawBitmapString(ofToString(noiseVal[(i*numCols) + j]), 0,10);
                ofSetColor(0,255,0);
                    if(bShowIndexVals) ofDrawBitmapString(ofToString(i*numCols + j), 0, 20);
				ofPopMatrix();
			}
		}
	ofSetColor(255);
	ofDrawBitmapString("FPS: " + ofToString(ofGetFrameRate(), 0), 5, ofGetWindowHeight()-5);
}


/////////////////////////////// NOISE /////////////////////////////////////////
void testApp::makeNoise(void)
{
    
    for(int j = 0; j < numRows; j++)
    {
        for(int i = 0; i < numCols; i++)
        {
            noiseVal[(i*numCols) + j] = abs(noiseAmp * ofNoise(time * (j+10), time * (10 - i)) );
            
            
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





//////////////////////////// FIT TO SHAPE //////////////////////////////////
void testApp::fitToShape(){
    
    
  

    
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




