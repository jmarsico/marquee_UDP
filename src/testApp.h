#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "ofxOpenCv.h"
#include "ofxGui.h"
#include "ofxNetwork.h"
#include "IPVideoGrabber.h"
#include "ofxXmlSettings.h"



using ofx::Video::IPVideoGrabber;
//using ofx::Video::SharedIPVideoGrabber;

class testApp : public ofBaseApp{

public:

	void setup();
	void update();
	void draw();
    void keyPressed(int key);
    
    void sendLights();
    void makeNoise(void);
    int arraySum(int index);
    void testLoop();
    
    
    
    const static int numLEDs = 254;
    const static int numRows = 14;
    const static int numCols = 21;
	int br[numLEDs];
    int finalVal[numLEDs];
    int fittedVals[numLEDs];
    
    int numLightsInRow[numRows]; 
    
    const static int numWordLights = 120;
    int wordLightVals[numWordLights];
    
    
	unsigned char* pixels;
	//int cellSize;
	float cellSizeFl;
	int numPixels;
    int numCellsX, numCellsY;
    bool bReadyToSend;

	int	cameraWidth;
	int	cameraHeight;

    
    vector <int> noiseCoeffs;
    
    
    //noise Vars
    float noiseAmp, time;
    int noiseVal[numRows * numCols];
    
    int sendTime;
    int testCounter;
    int testTimer;
    
    
	
	int numBoards;
    
    //video stuff
//    SharedIPVideoGrabber grabber;
    
    ofx::Video::IPVideoGrabber grabber;
//	ofVideoGrabber videoGrabber;
    ofxCv::RunningBackground background;
    ofImage thresholded;
    ofImage regImage;
    
    ofxPanel gui;
    ofxIntSlider backgroundThresh;
    ofxIntSlider blurAmount;
    ofxIntSlider erodeAmount;
    ofxIntSlider dilateAmount;
    ofxIntSlider learningTime;
    ofxFloatSlider revealAmount;
    ofxFloatSlider timeInc;
    ofxIntSlider lowerLim;
    ofxIntSlider uppderLim;
    ofxToggle bShowNoiseVals;
    ofxToggle bShowIndexVals;
    ofxToggle bShowMask;
    ofxToggle  lightsOn;
    ofxFloatSlider displayCoeff;
    ofxIntSlider cellSize;
    ofxIntSlider horizShift;
    ofxIntSlider vertShift;
    ofParameter<float> frameRate;
    ofParameter<int> udp1MessLeng;
    ofParameter<int> udp2MessLeng;
    ofxButton reset;
    ofxToggle test;
    ofxIntSlider testWaitTime;
    
    
    ofxUDPManager udp1;
    ofxUDPManager udp2;
    
    ofxXmlSettings config;


	
};
