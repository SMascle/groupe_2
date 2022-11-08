#include "ofApp.h"
#include <complex>  // besoin pour la transformée de Fourier


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
	signal_type         = 0;


	t_start = 0;
	n_harmonics = 10;

	filter				=0;
	vfilter.assign(bufferSize, 0.0);

	audio.assign(bufferSize, 0.0);
	carre.assign(bufferSize, 0.0);
	fftA.assign (bufferSize, 0.0);  //sebastien pour Fourier
	
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
	settings.numOutputChannels = 1;
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
			for (unsigned int i = 0; i < audio.size(); i++){
				float x =  ofMap(i, 0, audio.size(), 0, 900, true);
				ofVertex(x, 100 -audio[i]*180.0f);
			}
			ofEndShape(false);
			
		ofPopMatrix();
	ofPopStyle();


	// draw the right channel:       // sebastien a mit son code pour la transformée de Fourier en affichage
	/*
	ofPushStyle();
		ofPushMatrix();
		ofTranslate(32, 350, 0);
			
		ofSetColor(225);
		ofDrawBitmapString("Right Channel", 4, 18);
		
		ofSetLineWidth(1);	
		ofDrawRectangle(0, 0, 900, 200);

		ofSetColor(145, 958, 35); // changement couleur
		ofSetLineWidth(3);
					
			ofBeginShape();

			// appliquer la fft
			fft(lAudio, sampleRate);
			// cout << fftA[0] << endl;

			for (unsigned int i = 0; i < fftA.size(); i++){
				float x =  ofMap(i, 0, fftA.size(), 0, 900, true);
				ofVertex(x, 100 - fftA[i]*720000000.0f);   // changer la constante en .0f pour avoir une échelle souhaitable
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
	/*
	//gestion volume
	if (key == '-' || key == '_' ){
		volume -= 0.05;
		volume = MAX(volume, 0);
	} else if (key == '+' || key == '=' ){
		volume += 0.05;
		volume = MIN(volume, 1);
	}
	*/
	//gestion forme signal
	if( key == 'q' ){
		signal_type=1;
	}

	if( key == 'f' ){
		signal_type=0;
	}

	if( key == 'd' ){
		signal_type=2;
	}

	if( key == '+' ){
		n_harmonics+=1;
	}

	if( key == '-' ){
		n_harmonics-=1;
	}
	
	//gestion pause
	if( key == 's' ){
		soundStream.start();
	}
	
	if( key == 'e' ){
		soundStream.stop();
	}

	//gestion filtres
	if( key == 'l' ){
		//l = passe-bas : on garde que les valeurs inferieures au seuil
		filter = 1;
	}
	if( key == 'm' ){
		//p = passe-haut : on garde que les valeurs superieur au seuil
		filter = 2;
	}

	//gestion octave
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
void ofApp::mouseScrolled(int x, int y, float scrollX, float scrollY){

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
	//signal_type = 2;

	if (signal_type==1){
		float f_val = targetFrequency;
		float dt = 1./float(sampleRate);
		//Transformée de Fourier
		for (size_t t = 0; t < 0+buffer.getNumFrames(); t++){
			float S=0;
			float t_val = (t+t_start)*dt;
			for (size_t k = 0; k < n_harmonics; k++){
				S += 4./3.14f*sin((2*k+1)*6.28f*f_val*t_val)/(2*k+1)*volume;
			}
		
			if (S>1){
				audio[t] = buffer[t*buffer.getNumChannels()    ] = 1;
			}else if (S<-1){
				audio[t] = buffer[t*buffer.getNumChannels()    ] = -1;
			}else{
				audio[t] = buffer[t*buffer.getNumChannels()    ] = S;
			}
		}
		t_start = t_start+buffer.getNumFrames();
	}else if (signal_type==2){
		float f_val = targetFrequency;
		float dt = 1./float(sampleRate);
		//signal dents de scie
		for (size_t t = 0; t < buffer.getNumFrames(); t++){
			float S=0;
			float t_val = (t+t_start)*dt;
			for (size_t k = 1; k < n_harmonics; k++){
				S += 2./3.14f*pow(-1.,k)*sin(k*6.28f*f_val*t_val)/k*volume;
			}
		
			if (S>1){
				audio[t] = buffer[t*buffer.getNumChannels()    ] = 1;
			}else if (S<-1){
				audio[t] = buffer[t*buffer.getNumChannels()    ] = -1;
			}else{
				audio[t] = buffer[t*buffer.getNumChannels()    ] = S;
			}
		}
		t_start = t_start+buffer.getNumFrames();
	}else if ( bNoise == true){
		// ---------------------- noise --------------
		for (size_t i = 0; i < buffer.getNumFrames(); i++){
			audio[i] = buffer[i*buffer.getNumChannels()    ] = ofRandom(0, 1) * volume; // * leftScale;
			
		}
	} 
	else {
		phaseAdder = 0.95f * phaseAdder + 0.05f * phaseAdderTarget;
		for (size_t i = 0; i < buffer.getNumFrames(); i++){
			phase += phaseAdder;
			float sample = sin(phase);
			audio[i] = buffer[i*buffer.getNumChannels()    ] = sample * volume ; //* leftScale;

		}
	}

	if(filter=2){
		for (int i=0; i < Audio.size() ; i++){
			vfilter[i] = 0;
		}
		for (int i=0; i > Audio.size()*0.3; i++){
			vfilter[i] = 1;
			Audio[i] = Audio[i] * vfilter[i];
		}
	}
	else if (filter=1){
		for (int i=0; i< Audio.size(); i++){
			vfilter[i] = 0;
		}
		for (int i=0; i< Audio.size()*0.3; i++){
			vfilter[i] = 1;
			Audio[i] = Audio[i] * vfilter[i];
		}
	}

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
//-----   Transformée de Fourier 
void ofApp::fft(vector <float >lAudio, float sampleRate){
    int maxVal = lAudio.size();
    // fftA.assign (maxVal, 0.0);
    const float pi = std::acos(-1);
    const std::complex<float> i(0, 1);

    for (int f = 0; f < maxVal; f++){
        float ff = float(f) / static_cast<float>(2 * maxVal); // vu qu'on divise et multiplie par samplerate dans tt et ff; autant les changer
		 													 // plus besoin de tt à la place de t, et plus de samplerate dans la formule de ff
        std::complex<float> integral (0.0,0.0);

        for(int t = 0; t < maxVal; t++){
            integral += lAudio[t]  * std::exp(-2 * pi * i * ff * float(t) ) / sampleRate;
			//cout << ff << " "<< integral <<" " << tt << endl;
        }
        fftA[f] = std::norm(integral);
    }
}