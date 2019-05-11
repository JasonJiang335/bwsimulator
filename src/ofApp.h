#pragma once

#include "ofMain.h"
#include  "ofxAssimpModelLoader.h"
#include "box.h"
#include "ray.h"
#include "octree.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
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
		void drawAxis(ofVec3f);
		void initLightingAndMaterials();
		void savePicture();
		void toggleWireframeMode();
		void togglePointsDisplay();
		//void toggleSelectTerrain();
		void setCameraTarget();
		//bool doPointSelection();
		void drawBox(const Box &box);
		Box meshBounds(const ofMesh &);
		
		Box meshBounds(ofxAssimpModelLoader &);
		//void updateRoverBox();
		

		vector<ofCamera> cams;
		ofCamera * currentCam;

		vector<ofxAssimpModelLoader> map;
		ofxAssimpModelLoader raceTrack;
		ofxAssimpModelLoader raceCar;

		ofLight light;
	
		bool bAltKeyDown;
		bool bCtrlKeyDown;
		bool bWireframe;
		bool bDisplayPoints;
		bool bPointHovered;
		bool bPlayMode;
		bool bShowPath;
		
		ofVec3f intersectPoint;

		int pmouseX;
		int pmouseY;

		octree ot;
		bool bDrawOctree;
		bool bPointSelectionMode;

		vector<ofVec3f> trails;
		
		int selectedPoint;
		bool bPointMoving;

		float playMode_t;
		int playMode_i;

		float speed = 1.0f;

		int lvl = 5;


};
