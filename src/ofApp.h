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
		void toggleSelectTerrain();
		void setCameraTarget();
		bool  doPointSelection();
		void drawBox(const Box &box);
		Box meshBounds(const ofMesh &);
		
		Box meshBounds(ofxAssimpModelLoader &);
		void updateRoverBox();
        void savePath();
        void loadPath();

		ofCamera cam;
		float camLatitude = 0.0f;
		float camLongitude = -glm::half_pi<float>();

		ofCamera pathCam;
		ofCamera * currentCam;
		ofxAssimpModelLoader wholeTrack, track, car;
		ofLight light;
		Box boundingBox;
		Box roverBox;
	
		bool bAltKeyDown;
		bool bCtrlKeyDown;
		bool bWireframe;
		bool bDisplayPoints;
		bool bPointHovered;
		bool bPlayMode = false;
		bool bTerrainSelected;
		bool bDrawPath = true;
		
		ofVec3f intersectPoint;

		int pmouseX;
		int pmouseY;
		Vector3 roverOriMax;
		Vector3 roverOriMin;
		glm::vec3 diffToRover;
    glm::vec3 camPos;

		bool bRoverDraging;
		

		octree ot;
		bool bDrawOctree = false;
		bool bPointSelectionMode = false;
		vector<ofVec3f> trails;
        vector<int> speed;
		const float selectionRange = 4.0;

		int selectedPoint = -1;
		bool bPointMoving = false;

		float playMode_t;
		int playMode_i;

		glm::vec3 target;
		int lookatSelection=1;

		float carAngle;
		float carMaxOmega = 90.0f;

		glm::vec2 carInitOrientation{ 1, 0 };
		ofVec3f pathCamOffset { 0.0f, 5.0f, 0.0f };

		float globalSpeedScaler = 0.5f;
};
