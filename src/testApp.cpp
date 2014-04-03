#include "testApp.h"


//--------------------------------------------------------------
void testApp::setup()
{
	ofSetFrameRate(15);

	//construct new PCA9685 object with the number of boards you're using
	numBoards = ceil(float(numLEDs) / 16.0);
    ofLog() << "numboards: " << numBoards;
    

	
    ofSetVerticalSync(false);
	cameraWidth		= 320;
	cameraHeight	= 240;
	cellSize = 48;
	cellSizeFl  = (float)cellSize;
	numPixels = (cameraWidth/cellSize) * (cameraHeight/cellSize);
    ofLog() << "numPixels: " << numPixels;

	displayCoeff = 1;
	
	videoGrabber.listDevices();
	videoGrabber.setDesiredFrameRate(60); 
	videoGrabber.initGrabber(cameraWidth, cameraHeight);
	pixels = new unsigned char[numPixels];
    
    
    //noise Vars
    noiseSpeedX = 0.01;
    noiseSpeedY = 0.01;
    noiseAmp = 2000;
    time = 0;
    timeInc = 0.01;
    
    
    //setup serial
    serial.listDevices();
	vector <ofSerialDeviceInfo> deviceList = serial.getDeviceList();
	
	// this should be set to whatever com port your serial device is connected to.
	// (ie, COM4 on a pc, /dev/tty.... on linux, /dev/tty... on a mac)
	// arduino users check in arduino app....
	int baud = 57600;
	serial.setup(0, baud); //open the first device
	//serial.setup("COM4", baud); // windows example
	//serial.setup("/dev/tty.usbserial-A4001JEC", baud); // mac osx example
	//serial.setup("/dev/ttyUSB0", baud); //linux example
    
    
	numIterations = ceil(float(numPixels) / float(messPerFrame));
    iteration = 0;


  
}


//--------------------------------------------------------------
void testApp::update()
{
	
//////////////////Prod Section //////////////////
    iteration ++;
    if(iteration > numIterations)
    {
        iteration = 0;
    }
    
    
    
    //update the video grabber
	videoGrabber.update();

    //if a new frame is available do this:
	if(videoGrabber.isFrameNew())
    {
        //get the pixels of the new frame
        ofPixels pix = videoGrabber.getPixelsRef();
        //mirror the pixels
        pix.mirror(false, true); 
        
        
        
        for(int j = 0; j < cameraHeight/cellSize; j++)
        {
        	for (int i = 0; i < cameraWidth/cellSize; i++)
			{
				int x = i*cellSize;
				int y = j*cellSize;
				br[(j*(cameraWidth/cellSize))+i] = (float)pix.getColor(x,y).getLightness();
				br[(j*(cameraWidth/cellSize))+i] = ofMap(br[(j*(cameraWidth/cellSize))+i], 0, 255, 0, 2000);
			}
        }
    }

    makeNoise();
    runLights(br);
    
}



///////////////////////// NOISE ///////////////////////////////////
void testApp::makeNoise(void)
{
    for(int i = 0; i < messPerFrame + (messPerFrame * iteration); i++)
    {
        if(i <= numLEDs)
        {
            noiseVal[i] = abs(noiseAmp * ofNoise(time + (i*10)));
           // ofLog() << "index: " << i << " || value: " << noiseVal[i];
        }
    }
    time += timeInc;
}


////////////////////// RUN LIGHTS //////////////////////////////////
void testApp::runLights(float br[])
{
	//create array to hold final LED values
    int finalBright[16*numBoards];
    
    //variable to keep track of how many messages we are sending each frame
    int numMessages = 0;
    
    
    
    /*////////////////////////////////////////////////////////////////////
    each frame we send two groups of values... first, we send the video + noise values (white only)
    then, we send the iteration of noise updates.
     */////////////////////////////////////////////////////////////////////
    
    
    
    /////////////// fist we send the video values (including the noise value)
	for(int i = 0; i < numLEDs; i++)
    {
    	
        //if the video pixel is > black, get it's value and add it to noise
        if(br[i] > 0)
        {
            //final brightness equals video vals + noiseVals
            finalBright[i] = br[i] + noiseVal[i];
            
            
            
            
            //message buffer
            unsigned char sendBuff[5];
            
            //this 16bit value needs to be split into two bytes (8bit values)
            //uint16_t value = finalBright[i];
            uint16_t value = (int)noiseVal[i];
            
            //split the 16 bit value into two bytes
            uint8_t byte1 = value & 0xff;
            uint8_t byte2 = value >> 8;
            
            
            //send the buffer
            sendBuff[0] = 'a';  //handshake
            sendBuff[1] = i;    //channel
            sendBuff[2] = byte1;    //part one of LED value
            sendBuff[3] = byte2;    //part two of LED value
            
            int test;
            test = byte1 | byte2 << 8;
            //ofLog() << "original val: " << (int)noiseVal[i] << " | test byte: " << test;
            
            
            int time = ofGetElapsedTimeMicros();
            serial.writeBytes(sendBuff, 4);
            numMessages ++;   //increment this counter after each message buffer is sent
            ofDrawBitmapString("time per packet" + ofToString(ofGetElapsedTimeMicros() - time), 0,0);
        }
    }
    
    
    /////////////// then we send the noise values
    for(int i = 0; i < messPerFrame + (messPerFrame * iteration); i++)
    {
        if(i <= numLEDs)
        {
            //message buffer
            unsigned char sendBuff[5];
            
            //this 16bit value needs to be split into two bytes (8bit values)
            uint16_t value = (int)noiseVal[i];
            
            //split the 16 bit value into two bytes
            uint8_t byte1 = value & 0xff;
            uint8_t byte2 = value >> 8;
            
            
            //send the buffer
            sendBuff[0] = 'a';  //handshake
            sendBuff[1] = i;    //channel
            sendBuff[2] = byte1;    //part one of LED value
            sendBuff[3] = byte2;    //part two of LED value
            
            int test;
            test = byte1 | byte2 << 8;
            //ofLog() << "original val: " << (int)noiseVal[i] << " | test byte: " << test;
            
            
            int time = ofGetElapsedTimeMicros();
            serial.writeBytes(sendBuff, 4);
            numMessages ++;   //increment this counter after each message buffer is sent
            
            //
            ofDrawBitmapString("time per packet" + ofToString(ofGetElapsedTimeMicros() - time), 0,0);
            
        }
    }
    
    //report how many messages are being sent.
    ofDrawBitmapString("number of messages per frame: " + ofToString(numMessages), 0, 10);
}





//--------------------------------------------------------------
void testApp::draw(){
	
    int leftSpace = 100;
	int space = 200-cellSize;
	int height = 230;

	ofBackground(10, 10, 10);

	ofSetColor(255);
	videoGrabber.draw(leftSpace, height, cameraWidth*displayCoeff, cameraHeight*displayCoeff);
	ofDrawBitmapString("Source", leftSpace, height-20);


			
	
		//ofTranslate(cameraWidth+100,100);
		//pixelTexture.draw(0,0);
	
	ofDrawBitmapString("Pixelated and Mirrored", cameraWidth*displayCoeff+leftSpace+space+cellSize, height - 20);

	for(int j = 0; j < (cameraHeight/cellSize); j++)
		{
			for(int i = 0; i < (cameraWidth/cellSize); i++)
			{
				ofPushMatrix();
				ofTranslate(space,0);
				ofTranslate(i*cellSize*displayCoeff+cameraWidth*displayCoeff+space, j*cellSize*displayCoeff+height);
				//ofSetColor(stevensLaw(br[j*(cameraWidth/cellSize)+i]));
                int noiseMapped = ofMap(noiseVal[j*(cameraWidth/cellSize)+i], 0, 2000, 0, 125);
                int camMapped = ofMap(br[j*(cameraWidth/cellSize)+i], 0, 2000, 0, 125);
                ofSetColor(noiseMapped + camMapped);
					ofRect(0.0,0.0,cellSizeFl*displayCoeff, cellSizeFl*displayCoeff);
                    ofSetColor(255,0,0);
                    ofDrawBitmapString(ofToString(j*(cameraWidth/cellSize)+i), 0,10);
				ofPopMatrix();
			}
		}
	//ofPopMatrix();
	

	ofSetColor(255);
	ofDrawBitmapString("FPS: " + ofToString(ofGetFrameRate(), 0), 5, ofGetWindowHeight()-5);
    
}


///////////////////////// TEST COMMUNICATION ///////////////////////////////////
void testApp::testCom(int val)
{
    unsigned char sendBuff[5];
    
    for(uint8_t i; i < numLEDs; i++)
    {
        uint16_t value = val;
        
        uint8_t byte1 = value & 0xff;
        uint8_t byte2 = value >> 8;
        
        sendBuff[0] = 'a';  //handshake
        sendBuff[1] = i;    //channel
        sendBuff[2] = byte1;
        sendBuff[3] = byte2;
        // sendBuff[4] = 'e'; //end byte
        
        int test;
        test = byte1 | byte2 << 8;
        //ofLog() << "original val: " << (int)noiseVal[i] << " | test byte: " << test;
        
        
        int time = ofGetElapsedTimeMicros();
        serial.writeBytes(sendBuff, 4);
        ofDrawBitmapString("time per packet" + ofToString(ofGetElapsedTimeMicros() - time), 0,0);
    }
}


///////////////////////// KEY PRESSED //////////////////////////////
void testApp::keyPressed(int key){
    if(key == ' ')
    {
        testCom(4095);
        //serial.writeByte('a');
        
    }
    if(key == 't')
    {
        testCom(0);
    }
}




