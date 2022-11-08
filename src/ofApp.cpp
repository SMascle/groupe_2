#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){

	ofBackground(34, 34, 34);
	
	int bufferSize		= 512;
	sampleRate 			= 44100;
	phase 				= 0;
	phaseAdder 			= 0.0f;
	phaseAdderTarget 	= 0.0f;
	volume				= 0.1f;
	bNoise 				= false;
	carre_bool 			= false;

	Audio.assign(bufferSize, 0.0);
	carre.assign(bufferSize, 0.0);
	
	soundStream.printDeviceList();

	ofSoundStreamSettings settings;

	// if you want to set the device id to be different than the default:
	//
	//	auto devices = soundStream.getDeviceList();
	//	settings.setOutDevice(devices[3]);

	// you can also get devices for an specific api:
	//
	//	auto devices = soundStream.getDeviceList(ofSoundDevice::Api::PULSE);
	//	settings.setOutDevice(devices[0]);

	// or get the default device for an specific api:
	//
	// settings.api = ofSoundDevice::Api::PULSE;

	// or by name:
	//
	//	auto devices = soundStream.getMatchingDevices("default");
	//	if(!devices.empty()){
	//		settings.setOutDevice(devices[0]);
	//	}

#ifdef TARGET_LINUX
	// Latest linux versions default to the HDMI output
	// this usually fixes that. Also check the list of available
	// devices if sound doesn't work
	auto devices = soundStream.getMatchingDevices("default");
	if(!devices.empty()){
		settings.setOutDevice(devices[0]);
	}
#endif

	settings.setOutListener(this);
	settings.sampleRate = sampleRate;
	settings.numOutputChannels = 2;
	settings.numInputChannels = 0;
	settings.bufferSize = bufferSize;
	soundStream.setup(settings);

	// on OSX: if you want to use ofSoundPlayer together with ofSoundStream you need to synchronize buffersizes.
	// use ofFmodSetBuffersize(bufferSize) to set the buffersize in fmodx prior to loading a file.
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){

	ofSetColor(225);
	ofDrawBitmapString("Synthétiseur ARTEK808 v0.1", 32, 32);
	ofDrawBitmapString("Press 's' to unpause the audio\npress 'e' to pause the audio", 32, 92);
	ofDrawBitmapString("\nPress 'W', 'X', 'C', 'V','B','N', for play note Do-Re-Mi-Fa-Sol-La-Si", 32, 105);
	ofDrawBitmapString("\nPress 'q' for activate harmonies", 32, 118);
	ofDrawBitmapString("\nPress 'f' for desactivate harmonies", 32, 131);
	
	ofNoFill();
	
	if (carre_bool){
		ofPushStyle();
		ofPushMatrix();
		ofTranslate(32, 150, 0);
			
		ofSetColor(225);
		ofDrawBitmapString("carre", 4, 18);
		
		ofSetLineWidth(1);	
		ofDrawRectangle(0, 0, 900, 200);

		ofSetColor(245, 58, 135);
		ofSetLineWidth(3);
					
			ofBeginShape();
			for (unsigned int i = 0; i < carre.size(); i++){
				float x =  ofMap(i, 0, carre.size(), 0, 900, true);
				ofVertex(x, 100 -carre[i]*180.0f);
			}
			ofEndShape(false);
			
		ofPopMatrix();
		ofPopStyle();
	}else{
	// draw the left channel:
	ofPushStyle();
		ofPushMatrix();
		ofTranslate(32, 150, 0);
			
		ofSetColor(225);
		ofDrawBitmapString("Audio_Output", 4, 18);
		
		ofSetLineWidth(1);	
		ofDrawRectangle(0, 0, 900, 200);

		ofSetColor(245, 58, 135);
		ofSetLineWidth(3);
					
			ofBeginShape();
			for (unsigned int i = 0; i < Audio.size(); i++){
				float x =  ofMap(i, 0, Audio.size(), 0, 900, true);
				ofVertex(x, 100 -Audio[i]*180.0f);
			}
			ofEndShape(false);
			
		ofPopMatrix();
	ofPopStyle();
	}

	// draw the right channel:
	/*
	ofPushStyle();
		ofPushMatrix();
		ofTranslate(32, 350, 0);
			
		ofSetColor(225);
		ofDrawBitmapString("Right Channel", 4, 18);
		
		ofSetLineWidth(1);	
		ofDrawRectangle(0, 0, 900, 200);

		ofSetColor(245, 58, 135);
		ofSetLineWidth(3);
					
			ofBeginShape();
			for (unsigned int i = 0; i < rAudio.size(); i++){
				float x =  ofMap(i, 0, rAudio.size(), 0, 900, true);
				ofVertex(x, 100 -rAudio[i]*180.0f);
			}
			ofEndShape(false);
			
		ofPopMatrix();
	ofPopStyle();
	*/
		
	ofSetColor(225);
	string reportString = "volume: ("+ofToString(volume, 2)+") modify with -/+ keys";//\npan: ("+ofToString(pan, 2)+") modify with mouse x\nsynthesis: ";
	//if( !bNoise ){
	//	reportString += "sine wave (" + ofToString(targetFrequency, 2) + "hz) modify with mouse y";
	//}else{
	//	reportString += "noise";	
	//}
	ofDrawBitmapString(reportString, 32, 579);

}

//--------------------------------------------------------------
void ofApp::keyPressed  (int key){
	if (key == '-' || key == '_' ){
		volume -= 0.05;
		volume = MAX(volume, 0);
	} else if (key == '+' || key == '=' ){
		volume += 0.05;
		volume = MIN(volume, 1);
	}

	if( key == 'q' ){
		carre_bool = true;
	}

	if( key == 'f' ){
		carre_bool = false;
	}
	
	if( key == 's' ){
		soundStream.start();
	}
	
	if( key == 'e' ){
		soundStream.stop();
	}
	
	if( key == 'w' ){
		//do 261.6
		targetFrequency = 261.6;
	}
	if( key == 'x' ){
		//re 293.7
		targetFrequency = 293.7;
	}
	if( key == 'c' ){
		//mi 329.6
		targetFrequency = 329.6;
	}
	if( key == 'v' ){
		//fa 349.2
		targetFrequency = 349.2;
	}
	if( key == 'b' ){
		//sol 392
		targetFrequency = 392.0;
	}
	if( key == 'n' ){
		//la 440
		targetFrequency = 440.0;
	}
	if( key == ',' ){
		//si 493.9
		targetFrequency = 493.9;
	}
	phaseAdderTarget = (targetFrequency / (float) sampleRate) * TWO_PI;
}

//--------------------------------------------------------------
void ofApp::keyReleased  (int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
/*
	int width = ofGetWidth();
	pan = (float)x / (float)width;
	float height = (float)ofGetHeight();
	float heightPct = ((height-y) / height);
	targetFrequency = 2000.0f * heightPct;
	phaseAdderTarget = (targetFrequency / (float) sampleRate) * TWO_PI;
*/	
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
	/*
	int width = ofGetWidth();
	pan = (float)x / (float)width;
*/
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
	bNoise = true;
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
	bNoise = false;
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::audioOut(ofSoundBuffer & buffer){
	//pan = 0.5f;
	//float leftScale = 1 - pan;
	//float rightScale = pan;

	// sin (n) seems to have trouble when n is very large, so we
	// keep phase in the range of 0-TWO_PI like this:
	while (phase > TWO_PI){
		phase -= TWO_PI;
	}
	if (carre_bool){
		float f_val = targetFrequency;
		float dt = 1./float(sampleRate);
		//Transformée de Fourier
		for (size_t t = 0; t < buffer.getNumFrames(); t++){
			float S=0;
			float t_val = t*dt;
			for (size_t k = 0; k < 1000; k++){
				S += 4./3.14f*sin((2*k+1)*6.28f*f_val*t_val)/(2*k+1)*volume;
			}
		
			if (S>1){
				carre[t] = buffer[t*buffer.getNumChannels()    ] = 1;
			}else if (S<-1){
				carre[t] = buffer[t*buffer.getNumChannels()    ] = -1;
			}else{
				carre[t] = buffer[t*buffer.getNumChannels()    ] = S;
			}
		}
	}else if ( bNoise == true){
		// ---------------------- noise --------------
		for (size_t i = 0; i < buffer.getNumFrames(); i++){
			Audio[i] = buffer[i*buffer.getNumChannels()    ] = ofRandom(0, 1) * volume; // * leftScale;
			
		}
	} else {
		phaseAdder = 0.95f * phaseAdder + 0.05f * phaseAdderTarget;
		for (size_t i = 0; i < buffer.getNumFrames(); i++){
			phase += phaseAdder;
			float sample = sin(phase);
			Audio[i] = buffer[i*buffer.getNumChannels()    ] = sample * volume ; //* leftScale;

		}
	}

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
