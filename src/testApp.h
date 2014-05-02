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
    
    
    
    const static int numLEDs = 280;
	int br[numLEDs];
    int finalVal[numLEDs];
    
    
    
	unsigned char* pixels;
	int cellSize;
	float cellSizeFl;
	int numPixels;
    int numCellsX, numCellsY;
    bool bReadyToSend;

	int	cameraWidth;
	int	cameraHeight;

    
    vector <int> noiseCoeffs;
    
    
    //noise Vars
    float noiseAmp, time;
    int noiseVal[numLEDs];
    
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
    
    ofxButton reset;
    
    
     ofxUDPManager udpConnection;

    
    
    
    
private:
    
    const static int messPerFrame = 100;
    int numIterations;
    int iteration;



	
};
