#pragma once

#include "ofMain.h"

 // set to -1 to not use the enable pin (its optional)

class testApp : public ofBaseApp{

public:

	void setup();
	void update();
	void draw();
    void keyPressed(int key);
    
    
	void runLights(float br[]);
    void makeNoise(void);
    void testCom(int val);
    

	int val;
	int valInc;

    const static int numLEDs = 17;
	float br[numLEDs];
    float noiseVal[numLEDs];
	unsigned char* pixels;
	int cellSize;
	float cellSizeFl;
	int numPixels;

	int	cameraWidth;
	int	cameraHeight;

	float displayCoeff;
    
    
    //noise Vars
    float noiseSpeedX, noiseSpeedY, noiseAmp, time, timeInc;

	
	int numBoards;

	ofVideoGrabber videoGrabber;
	//ofTexture pixelTexture;
    
    ofSerial	serial;
    
    
    
    
private:
    
    const static int messPerFrame = 100;
    int numIterations;
    int iteration;



	
};
