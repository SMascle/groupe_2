#include "ofApp.h"
#include <complex>  // besoin pour la transformée de Fourier
#include <cmath> // besoin pour les exposants

//--------------------------------------------------------------
void ofApp::setup(){

	ofBackground(34, 34, 34);
	title.load("title.png");
	title.resize(300,250);
	title2.load("title2.jpg");
	title2.resize(300,250);
	int bufferSize		= 512;
	sampleRate 			= 44100;
	phase 				= 0;
	phaseAdder 			= 0.0f;
	phaseAdderTarget 	= 0.0f;
	volume				= 0.1f;
	bNoise 				= false;
	aNoise       		= 0;
	signal_type         = 0;
	octave              = 3.0; // on commence à la 3ème octave, comprise en tre 0 et 9.

	t_start = 0;
	n_harmonics = 10;

	choice_filter = 0;
	

	audio.assign(bufferSize, 0.0);
	audio2.assign(bufferSize, 0.0);
	fftA.assign(bufferSize, 0.0);  			//sebastien pour Fourier
	audio_filtre.assign(bufferSize, 0.0);	//pour les filtres

	//Instanciation des couleurs :
	cbool=true;
	R = 51;
	G = 255;
	B = 51;
	BG_R = 0;
	BG_G = 0;
	BG_B = 0;
	P_R = 225;
	P_G = 225;
	P_B = 225;
	
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
	
	
	ofBackground(BG_R, BG_G, BG_B);
	ofPushStyle();
	ofSetColor(R, G, B);
	//ofDrawBitmapString("Synthesizer ARTEK808 v0.5", 32, 32);
	ofDrawBitmapString("Menu :",32, 20);
	ofDrawBitmapString("Press s/e to un/pause the audio",32, 37);
	ofDrawBitmapString("Press 'leftclick' to un/pause the noise", 32, 54);
	ofDrawBitmapString("Press w, x, c, v, b, n to play note (Do-to-Si)", 32, 71);
	//
	ofDrawBitmapString("Press f/q/d for getting sine/harmonies/saw wave", 32, 88);
	ofDrawBitmapString("Press o/p to increase/decrease octave", 32, 105);
	ofDrawBitmapString("Press u/i to increase/decrease noise", 32, 122);
	//filters
	ofDrawBitmapString("Press k/l to de/activate filters", 32, 139);
	ofDrawBitmapString("Move mouse to change cutoff frenquency", 32, 156);
	ofDrawBitmapString("Press up/down to increase filter quality", 32, 173);
	ofDrawBitmapString("Scroll to increase/decrease volume", 32, 190);

	// activation l/k/ m si 2 filtres 

	ofDrawBitmapString("Parameters :", 32, 250);
	ofDrawBitmapString("Noise on : " +ofToString(bNoise, 2), 32, 267);
	ofDrawBitmapString("Noise value : "+ofToString(aNoise, 2), 32, 284);
	ofDrawBitmapString("Octave : "+ofToString(octave, 2), 32, 301);
	ofDrawBitmapString("Volume: "+ofToString(volume, 2),  32, 318);
	
	ofNoFill();
	
	// draw the Audio channel:
	ofPushStyle();
		ofPushMatrix();
		ofTranslate(32, 325, 0);
			
		ofSetColor(P_R, P_G, P_B);
		ofDrawBitmapString("Audio_Output", 4, 18);
		
		ofSetLineWidth(1);	
		ofDrawRectangle(0, 0, 1020, 200);

		ofSetColor(R, G, B);
		ofSetLineWidth(3);
					
			ofBeginShape();
			for (unsigned int i = 0; i < audio.size(); i++){
				float x =  ofMap(i, 0, audio.size(), 0, 1020, true);
				ofVertex(x, 100 -audio[i]*180.0f);
			}
			ofEndShape(false);
			
		ofPopMatrix();
	ofPopStyle();


	// sebastien a mit son code pour la transformée de Fourier en affichage
	ofPushStyle();
		ofPushMatrix();
		ofTranslate(32, 525, 0);
			
		ofSetColor(P_R, P_G, P_B);
		ofDrawBitmapString("Fourier Transform", 4, 18);
		
		ofSetLineWidth(1);	
		ofDrawRectangle(0, 0, 1020, 200);

		ofSetColor(R, G, B); // changement couleur
		ofSetLineWidth(3);
					
			ofBeginShape();

			// appliquer la fft
			fft(audio, sampleRate);
			float max_fftA = *max_element(fftA.begin(), fftA.end());
			// cout << fftA[0] << endl;

			for (unsigned int i = 0; i < fftA.size(); i++){
				float x =  ofMap(i, 0, fftA.size(), 0, 1020, true);
				float y =  ofMap(fftA[i], 0, max_fftA, 0, 165, true);
				ofVertex(x, 190 - y);   // changer la constante en .0f pour avoir une échelle souhaitable
			}
			ofEndShape(false);
			
		ofPopMatrix();
	ofPopStyle();

		
	ofSetColor(R, G, B);
	ofDrawBitmapString("For change theme press 1 or 2", 32, 750);
	string reportString2 = "ArTek808, all right reserved ©";
	ofDrawBitmapString(reportString2, 790, 750);
	ofPopStyle();
	if (cbool){
		title.draw(800, 0);
	}else{
		title2.draw(800, 0);}

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
	if( key == 'i' ){
		aNoise+=0.05;
		/*if (aNoise>=1){
			aNoise=1;
		}*/
	}

	if( key == 'u' ){
		aNoise-=0.05;
		if (aNoise<0){
			aNoise=0;
		}
	}

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
		choice_filter = 1;
	}
	if( key == 'm' ){
		//p = passe-haut : on garde que les valeurs superieur au seuil
		choice_filter = 2;
	}
	if( key == 'k' ){
		//annule filtre
		choice_filter = 0;
	}
	
	if( key == OF_KEY_UP){
		q += 0.1;
		if(q>=1){
			q=0.8;
		}
	}

	if( key == OF_KEY_DOWN){
		q -= 0.1;
		if(q<=0){
			q=0.2;
		}
	}



	//gestion octave

	if( key == 'o'){
		// touche pour diminuer d'une octave. On ne peut pas avoir une octave inférieur à 0.
		if( octave > 0.0f){
			octave = octave - 1.0f;
			targetFrequency = targetFrequency / 2.0f; // Pour changer l'octave calculé il faut agir sur target frequency. Comme "la touche en cours" n'est
														// pas en mémoire, on agit directement sur sa valeur ainsi. ça ne pose pas de problème quand on rentre
															// une nouvelle touche.
		} 
		
	}

	if( key == 'p'){
		// touche pour augmenter d'une octave. On ne peut pas avoir une octave supérieur à 11.
		if( octave < 11.0f){
			octave = octave + 1.0f;
			targetFrequency = targetFrequency * 2.0f;
		} 
	}

	//gestion touche clavier (lié à octave)
	if( key == 'w' ){
		//do 261.6 à l'octave 3, 32.70 à l'octave 0. 
		targetFrequency = 32.70 * pow(2.0f, octave);
	}
	if( key == 'x' ){
		//re 293.7 à l'octave 3, 36.71 à l'octave 0. 
		targetFrequency = 36.71 * pow(2.0f, octave);
	}
	if( key == 'c' ){
		//mi 329.6 à l'octave 3, 41.20 à l'octave 0. 
		targetFrequency = 41.2 * pow(2.0f, octave);;
	}
	if( key == 'v' ){
		//fa 349.2 à l'octave 3, 43.65 à l'octave 0. 
		targetFrequency = 43.65 * pow(2.0f, octave);;
	}
	if( key == 'b' ){
		//sol 392 à l'octave 3, 49.00 à l'octave 0. 
		targetFrequency = 49.0 * pow(2.0f, octave);;
	}
	if( key == 'n' ){
		//la 440 à l'octave 3, 55.00 à l'octave 0. 
		targetFrequency = 55.0 * pow(2.0f, octave);;
	}
	if( key == ',' ){
		//si 493.9 à l'octave 3, 61.74 à l'octave 0. 
		targetFrequency = 61.74 * pow(2.0f, octave);;
	}
	phaseAdderTarget = (targetFrequency / (float) sampleRate) * TWO_PI;

	// Changement de theme :

		if( key == '1' ){
			cbool = true;
			R = 51;
			G = 255;
			B = 51;
			BG_R = 0;
			BG_G = 0;
			BG_B = 0;
			P_R = 225;
			P_G = 225;
			P_B = 225;

	}
		if( key == '2' ){
			cbool=false;
			R = 153;
			G = 0;
			B = 0;
			BG_R = 255;
			BG_G = 145;
			BG_B = 77;
			P_R = 32;
			P_G = 32;
			P_B = 32;
	}
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
	int width = ofGetWidth();
	pan = (float)x / (float)width;
	f0 = 10 + pan*8000;
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
	/*
	int width = ofGetWidth();
	pan = (float)x / (float)width;
	f0 = pan*22000;
*/
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
	if (bNoise == false){
		bNoise = true;
		if (aNoise==0){
			aNoise=0.05;
		}
	}else{
		bNoise = false;
	}
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseScrolled(int x, int y, float scrollX, float scrollY){
	if (scrollY == -1.){
		volume -= 0.05;
		volume = MAX(volume, 0);
	} else if (scrollY == 1.){
		volume += 0.05;
		volume = MIN(volume, 1);

	}
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
				S += 4./3.14f*sin((2*k+1)*6.28f*f_val*t_val)/(2*k+1)*volume;//*(1-aNoise)
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
				S += 2./3.14f*pow(-1.,k)*sin(k*6.28f*f_val*t_val)/k*volume;//*(1-aNoise)
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
	}else {
		phaseAdder = 0.95f * phaseAdder + 0.05f * phaseAdderTarget;
		for (size_t i = 0; i < buffer.getNumFrames(); i++){
			phase += phaseAdder;
			float sample = sin(phase);
			audio[i] = buffer[i*buffer.getNumChannels()    ] = sample * volume; //*(1-aNoise);* leftScale;

		}
	}

	if ( bNoise == true){
		// ---------------------- noise --------------
		for (size_t i = 0; i < buffer.getNumFrames(); i++){
			audio[i] = buffer[i*buffer.getNumChannels()    ] += ofRandom(0, 1)* aNoise * volume; // * leftScale;
			
		}
	} 

	if (choice_filter == 1 ){
	//passe bas
		audio2 = filter(audio, sampleRate, buffer);
		
		for (size_t i = 0; i < buffer.getNumFrames(); i++){
			audio[i] = buffer[i*buffer.getNumChannels()] = audio2[i] ;
		}
	}
	else if (choice_filter == 2){
	//pase haut

	}

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}


//--------------------------------------------------------------   
//Transformée de Fourier 
void ofApp::fft(vector <float>audio, float sampleRate){
    int maxVal = audio.size();
    // fftA.assign (maxVal, 0.0);
    const float pi = std::acos(-1);
    const std::complex<float> i(0, 1);

    for (int f = 0; f < maxVal; f++){
        float ff = float(f) / static_cast<float>(2 * maxVal); // vu qu'on divise et multiplie par samplerate dans tt et ff; autant les changer
		 													  // plus besoin de tt à la place de t, et plus de samplerate dans la formule de ff
        std::complex<float> integral (0.0,0.0);

        for(int t = 0; t < maxVal; t++){
            integral += audio[t]  * std::exp(-2 * pi * i * ff * float(t) ) / sampleRate;
			//cout << ff << " "<< integral <<" " << tt << endl;
        }
        fftA[f] = std::norm(integral);
    }
}


//--------------------------------------------------------------
//filtre le signal
vector <float> ofApp::filter(vector <float>audio, float sampleRate, ofSoundBuffer & buffer){
	float a=0, b=0, c=0,d=0, e=0;
	audio_filtre[0]=0;
	audio_filtre[1]=0;

	//q = 0.5;
	//f0 = 11000;
	Fs = sampleRate;

	omega = 2 * M_PI * (f0/Fs);
	alpha = sin( (omega)/(2*q) );
	
	b0 = alpha;
	b1 = 0;
	b2 = -1 * alpha;
	a0 = 1 + alpha;
	a1 = -2 * cos(omega);
	a2 = 1-alpha;

	a=(b0/a0);
	b=(b1/a0);
	c=(b2/a0);
	d=(a1/a0);
	e=(a2/a0);

	for (int t=0; t < buffer.getNumFrames(); t++){
		audio_filtre[t] = a*audio[t] + b*at1 + c*at2 - d*af1 - e*af2;
		at2 = at1;
		at1 = audio[t];
		af2 = af1;
		af1 = audio_filtre[t];
	}

	return audio_filtre;
}

