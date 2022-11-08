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
		
		ofSoundStream soundStream;

		vector <float> fftA;
		float 	pan;
		int		sampleRate;
		bool 	bNoise;
		float 	volume;
		int 	signal_type;
		float   t_start;
		size_t 	n_harmonics;


		vector <float> audio;
		vector <float> carre;

		
		//------------------- for the simple sine wave synthesis
		float 	targetFrequency;
		float 	phase;
		float 	phaseAdder;
		float 	phaseAdderTarget;
		float   ff; // ajout floatant ff
};
