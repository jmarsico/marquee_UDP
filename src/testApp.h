#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "ofxOpenCv.h"
#include "ofxGui.h"
#include "ofxNetwork.h"

 // set to -1 to not use the enable pin (its optional)

class testApp : public ofBaseApp{

public:

	void setup();
	void update();
	void draw();
    void keyPressed(int key);
    
    void sendLights();
    void makeNoise(void);
    int arraySum(int index);
    
    
    
    const static int numLEDs = 314;
    const static int numRows = 16;
    const static int numCols = 23;
	int br[numLEDs];
    int finalVal[numLEDs];
    int fittedVals[numLEDs];
    
    int numLightsInRow[numRows]; 
    
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
    
    
	
	int numBoards;
    
    //video stuff
	ofVideoGrabber videoGrabber;
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
    
    ofxButton reset;
    ofxUDPManager udp1;
    ofxUDPManager udp2;


	
};
