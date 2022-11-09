#pragma once

#include "ofMain.h"

class ofApp : public ofBaseApp{

	public:

		void setup();
		void update();
		void draw();

		void keyPressed  (int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		
		void audioOut(ofSoundBuffer & buffer);
		void mouseScrolled(int x, int y, float scrollX, float scrollY);
		void fft(vector <float> audio, float sampleRate);
		vector <float> filter(vector <float>audio, float sampleRate, ofSoundBuffer & buffer);
		
		ofSoundStream soundStream;
		ofImage title;



		float   octave; // variable du n° de l'octave.
		float 	pan;
		int		sampleRate;
		bool 	bNoise;
		float 	volume;
		int 	signal_type;
		float   t_start;
		size_t 	n_harmonics;
		float   aNoise;
		
		vector <float> audio;
		vector <float> audio2;
		vector <float> fftA;



		//----
		//param filtre
		int choice_filter;
		vector <float> audio_filtre;
		float a, b, c, d, e;
		float q=0.5;
		float f0=8000;
		float Fs;
		float omega;
		float alpha;
		float b0;
		float b1;
		float b2;
		float a0;
		float a1;
		float a2;

		float at1=0, at2=0,	af1=0, af2=0; 

		

		//------------------- for the simple sine wave synthesis
		float 	targetFrequency;
		float 	phase;
		float 	phaseAdder;
		float 	phaseAdderTarget;
		float   ff; // ajout floatant ff
};
