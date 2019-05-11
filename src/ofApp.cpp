
//--------------------------------------------------------------
//
//  Kevin M. Smith 
//
//  Mars HiRise Project - startup scene
// 
//  This is an openFrameworks 3D scene that includes an EasyCam
//  and example 3D geometry which I have reconstructed from Mars
//  HiRis photographs taken the Mars Reconnaisance Orbiter
//
//  You will use this source file (and include file) as a starting point
//  to implement assignment 5  (Parts I and II)
//
//  Please do not modify any of the keymappings.  I would like 
//  the input interface to be the same for each student's 
//  work.  Please also add your name/date below.

//  Please document/comment all of your work !
//  Have Fun !!
//
//  Student Name:   < Your Name goes Here >
//  Date: <date of last version>


#include "ofApp.h"
#include "Util.h"

Vector3 vecAdd(Vector3 u, ofPoint v) {
	
	return Vector3{ u.x() + v.x, u.y() + v.y, u.z() + v.z };
}

//--------------------------------------------------------------
// setup scene, lighting, state and load geometry
//
void ofApp::setup(){

	bWireframe = false;
	bDisplayPoints = false;
	bAltKeyDown = false;
	bCtrlKeyDown = false;
	bPointMoving = false;
	selectedPoint = -1;
	bDrawOctree = false;
	bPointSelectionMode = false;
	bPlayMode = false;
	bShowPath = false;

	cams.push_back(ofCamera{});

	ofEasyCam * cam = new ofEasyCam{};
	cam->enableMouseInput();
	
	currentCam = cam;

	currentCam->setNearClip(.1);
	currentCam->setFov(65.5);   // approx equivalent to 28mm in 35mm format
	ofSetVerticalSync(true);

	ofEnableSmoothing();
	ofEnableDepthTest();

	// setup rudimentary lighting 
	//
	initLightingAndMaterials();
	
	map.push_back(ofxAssimpModelLoader{});
	map.back().loadModel("geo/glass.obj");
	map.push_back(ofxAssimpModelLoader{});
	map.back().loadModel("geo/trees.obj");
	map.push_back(ofxAssimpModelLoader{});
	map.back().loadModel("geo/tw.obj");
	map.push_back(ofxAssimpModelLoader{});
	map.back().loadModel("geo/simtraxx_bw.obj");
	map.push_back(ofxAssimpModelLoader{});
	map.back().loadModel("geo/b1.obj");
	map.push_back(ofxAssimpModelLoader{});
	map.back().loadModel("geo/hrrm.obj");
	map.push_back(ofxAssimpModelLoader{});
	map.back().loadModel("geo/curbs.obj");
	map.push_back(ofxAssimpModelLoader{});
	map.back().loadModel("geo/treesFar.obj");
	map.push_back(ofxAssimpModelLoader{});
	map.back().loadModel("geo/ts.obj");
	
	for (auto& t : map) {
		t.disableTextures();
		t.setScaleNormalization(false);
	}

	
	raceTrack.loadModel("geo/cones_race26.obj");
	raceTrack.setScaleNormalization(false);

	//todo


	
	
}

//--------------------------------------------------------------
// incrementally update scene (animation)
//
void ofApp::update() {
	if (bPlayMode) {
		if (playMode_t > 1.0f) {
			if (playMode_i < trails.size() - 2) {
				playMode_t = 0.0f;
				++playMode_i;
			}
			else {
				bPlayMode = false;
				currentCam = &cams[0];
				return;
			}
		}

		float timeElasted = ofGetElapsedTimef();
		ofResetElapsedTimeCounter();
		/*
		playMode_t += timeElasted / trails[playMode_i].distance(trails[playMode_i + 1]) * speed;
		pathCam.setPosition(trails[playMode_i] * (1 - playMode_t) + trails[playMode_i + 1] * playMode_t + ofVec3f{ 0, 5, 0 });
		if (lookatSelection == 1) {
			target = trails[playMode_i+1] + ofVec3f(0, 5, 0);
		} else if (lookatSelection == 2) {
			target = bRoverLoaded ? (rover.getSceneMax() + rover.getSceneMin()) / 2 + rover.getPosition() : ofPoint{ 0, 10, 0 };
		}
		
		pathCam.lookAt(target);
		*/

		//testPoint = trails[playMode_i] * (1 - playMode_t) + trails[playMode_i + 1] * playMode_t + ofVec3f{ 0, 5, 0 };
	}
	
}
//--------------------------------------------------------------
void ofApp::draw(){

	ofBackground(ofColor::black);
	currentCam->begin();
	ofPushMatrix();
	if (bWireframe) {                    // wireframe mode  (include axis)
		ofDisableLighting();
		ofSetColor(ofColor::slateGray);
		for (auto& t : map)
			t.drawWireframe();
		raceTrack.drawWireframe();
		raceCar.drawWireframe();
	}
	else {
		ofEnableLighting();     
		// shaded mode
		for (auto& t : map)
			t.drawFaces();
		raceTrack.drawFaces();
		raceCar.drawFaces();
	}



	//Path
	if (bShowPath)
		for (int i = 0; i < trails.size();++i) {
			ofSetColor(ofColor::red);
			ofDrawSphere(trails[i], .1);
			if (i) ofDrawLine(trails[i-1], trails[i]);
		}

	//Hovered point
	if (!bPointMoving && bPointSelectionMode && bPointHovered) {
		ofSetColor(ofColor::blue);
		ofDrawSphere(intersectPoint, .1);
	}

	//octree
	if (bDrawOctree) {
		ofSetColor(0, 0, 255);
		ot.draw(lvl);
	}

	currentCam->end();
}

// 

// Draw an XYZ axis in RGB at world (0,0,0) for reference.
//
void ofApp::drawAxis(ofVec3f location) {

	ofPushMatrix();
	ofTranslate(location);

	ofSetLineWidth(1.0);

	// X Axis
	ofSetColor(ofColor(255, 0, 0));
	ofDrawLine(ofPoint(0, 0, 0), ofPoint(1, 0, 0));
	

	// Y Axis
	ofSetColor(ofColor(0, 255, 0));
	ofDrawLine(ofPoint(0, 0, 0), ofPoint(0, 1, 0));

	// Z Axis
	ofSetColor(ofColor(0, 0, 255));
	ofDrawLine(ofPoint(0, 0, 0), ofPoint(0, 0, 1));

	ofPopMatrix();
}


void ofApp::keyPressed(int key) {

	switch (key) {
	case 'C':
	case 'c':
		break;
	case 'F':
	case 'f':
		ofToggleFullscreen();
		break;
	case 'H':
	case 'h':
		break;
	case 'r':
		break;
	case 's':
		savePicture();
		break;
	case 't':
		setCameraTarget();
		break;
	case 'u':
		break;
	case 'v':
		break;
	case 'V':
		break;
	case 'w':
		toggleWireframeMode();
		break;
	case 'o':
		bDrawOctree^=true;
		break;
	case 'n':
		bPointSelectionMode ^= true;
		bPointHovered = false;
		selectedPoint = -1;
		break;
	case 'p':
		
		if (!bPlayMode) {
			if (!bPointSelectionMode && trails.size() >= 2) {
				bPlayMode = true;
				playMode_t = 0.0f;
				playMode_i = 0;
				ofResetElapsedTimeCounter();
			}
		}
		else {
			bPlayMode = false;
		}
		break;
	case '+':
	case '=':
		if (bDrawOctree) ++lvl;
		else speed *= 2.0f;
		break;
	case '-':
	case '_':
		if (bDrawOctree) --lvl;
		else speed *= 0.5f;
		break;
	
	case '1':
		for (auto& t : map)
			t.disableTextures();
		break;
	case '2':
		for (auto& t : map)
			t.enableTextures();
		break;
	case '3':
		for (auto& t : map)
			t.disableColors();
		break;
	case '4':
		for (auto& t : map)
			t.enableColors();
		break;
	case '5':
		for (auto& t : map)
			t.disableMaterials();
		break;
	case '6':
		for (auto& t : map)
			t.enableMaterials();
		break;
	case '7':
		for (auto& t : map)
			t.disableNormals();
		break;
	case '8':
		for (auto& t : map)
			t.enableNormals();
		break;
	case OF_KEY_ALT:
		bAltKeyDown = true;
		break;
	case OF_KEY_CONTROL:
		bCtrlKeyDown = true;
		break;
	case OF_KEY_SHIFT:
		break;
	case OF_KEY_DEL:
		if (selectedPoint != -1) {
			trails.erase(trails.begin() + selectedPoint);
			selectedPoint = -1;
		}
		else trails.clear();
		break;
	default:
		break;
	}
}

void ofApp::toggleWireframeMode() {
	bWireframe = !bWireframe;
}

void ofApp::togglePointsDisplay() {
	bDisplayPoints = !bDisplayPoints;
}

void ofApp::keyReleased(int key) {

	switch (key) {
	
	case OF_KEY_ALT:
		bAltKeyDown = false;
		break;
	case OF_KEY_CONTROL:
		bCtrlKeyDown = false;
		break;
	case OF_KEY_SHIFT:
		break;
	default:
		break;

	}
}



//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
	
	pmouseX = x;
	pmouseY = y;

	if (bPointSelectionMode) {
		bPointHovered = ot.doPointSelection(*currentCam, ofVec3f(x, y, 0), intersectPoint);
	}
}


//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {

	if (currentCam!=&cams[0]) return;

	if (bPointSelectionMode) {
		if (button == 2) selectedPoint = -1;
		else if (!button && bPointHovered) {
			int tmp = -1;
			for (int i = 0; i < trails.size(); ++i)
				if (trails[i] == intersectPoint)
					tmp = i;
			if (tmp != -1) {
				selectedPoint = tmp;
				bPointMoving = true;
				//bPointSelectionMode=false;
			}
			else {
				trails.push_back(intersectPoint);
				selectedPoint = trails.size() - 1;
				//bPointSelectionMode = false;
				bPointMoving = true;
			}
		}
	}
	else {
		//TODO
		//MOVE SCREEN
	}

	


	
}


//draw a box from a "Box" class  
//
void ofApp::drawBox(const Box &box) {
	Vector3 min = box.parameters[0];
	Vector3 max = box.parameters[1];
	Vector3 size = max - min;
	Vector3 center = size / 2 + min;
	ofVec3f p = ofVec3f(center.x(), center.y(), center.z());
	float w = size.x();
	float h = size.y();
	float d = size.z();
	ofDrawBox(p, w, h, d);
}

// return a Mesh Bounding Box for the entire Mesh
//
Box ofApp::meshBounds(const ofMesh & mesh) {
	int n = mesh.getNumVertices();
	ofVec3f v = mesh.getVertex(0);
	ofVec3f max = v;
	ofVec3f min = v;
	for (int i = 1; i < n; i++) {
		ofVec3f v = mesh.getVertex(i);

		if (v.x > max.x) max.x = v.x;
		else if (v.x < min.x) min.x = v.x;

		if (v.y > max.y) max.y = v.y;
		else if (v.y < min.y) min.y = v.y;

		if (v.z > max.z) max.z = v.z;
		else if (v.z < min.z) min.z = v.z;
	}
	return Box(Vector3(min.x, min.y, min.z), Vector3(max.x, max.y, max.z));
}

Box ofApp::meshBounds(ofxAssimpModelLoader &a)
{
	int m = a.getNumMeshes();
	ofVec3f v = a.getMesh(0).getVertex(0);
	ofVec3f max = v;
	ofVec3f min = v;
	
	for (int j = 0; j < m; ++j) {
		const ofMesh& mesh = a.getMesh(j);
		int n = mesh.getNumVertices();
		for (int i = 1; i < n; i++) {
			ofVec3f v = mesh.getVertex(i);

			if (v.x > max.x) max.x = v.x;
			else if (v.x < min.x) min.x = v.x;

			if (v.y > max.y) max.y = v.y;
			else if (v.y < min.y) min.y = v.y;

			if (v.z > max.z) max.z = v.z;
			else if (v.z < min.z) min.z = v.z;
		}
	}
	return Box(Vector3(min.x, min.y, min.z), Vector3(max.x, max.y, max.z));
}
/*
void ofApp::updateRoverBox()
{
	roverBox.parameters[0] = vecAdd(roverOriMin, rover.getPosition());
	roverBox.parameters[1] = vecAdd(roverOriMax, rover.getPosition());
}
*/



//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {
	

	
	if (bPointMoving) {
		ot.doPointSelection(*currentCam, ofVec3f(x, y, 0), trails[selectedPoint]);
	}
	else if (!bPointSelectionMode) {
		//Move
	}
	pmouseX = x;
	pmouseY = y;
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {
	
	bPointMoving = false;
	bPointHovered = false;
}


//
//  ScreenSpace Selection Method: 
//  This is not the octree method, but will give you an idea of comparison
//  of speed between octree and screenspace.
//
//  Select Target Point on Terrain by comparing distance of mouse to 
//  vertice points projected onto screenspace.
//  if a point is selected, return true, else return false;
//
/*
bool ofApp::doPointSelection() {

	ofMesh mesh = mars.getMesh(0);
	int n = mesh.getNumVertices();
	float nearestDistance = 0;
	int nearestIndex = 0;

	bPointHovered = false;

	ofVec2f mouse(mouseX, mouseY);
	vector<ofVec3f> selection;

	// We check through the mesh vertices to see which ones
	// are "close" to the mouse point in screen space.  If we find 
	// points that are close, we store them in a vector (dynamic array)
	//
	for (int i = 0; i < n; i++) {
		ofVec3f vert = mesh.getVertex(i);
		ofVec3f posScreen = cam.worldToScreen(vert);
		float distance = posScreen.distance(mouse);
		if (distance < selectionRange) {
			selection.push_back(vert);
			bPointHovered = true;
		}
	}

	//  if we found selected points, we need to determine which
	//  one is closest to the eye (camera). That one is our selected target.
	//
	if (bPointHovered) {
		float distance = 0;
		for (int i = 0; i < selection.size(); i++) {
			ofVec3f point =  cam.worldToCamera(selection[i]);

			// In camera space, the camera is at (0,0,0), so distance from 
			// the camera is simply the length of the point vector
			//
			float curDist = point.length(); 

			if (i == 0 || curDist < distance) {
				distance = curDist;
				intersectPoint = selection[i];
			}
		}
	}
	return bPointHovered;
}
*/

// Set the camera to use the selected point as it's new target
//  
void ofApp::setCameraTarget() {

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
void ofApp::gotMessage(ofMessage msg){

}



//--------------------------------------------------------------
// setup basic ambient lighting in GL  (for now, enable just 1 light)
//
void ofApp::initLightingAndMaterials() {

	static float ambient[] =
	{ .5f, .5f, .5, 1.0f };
	static float diffuse[] =
	{ 1.0f, 1.0f, 1.0f, 1.0f };

	static float position[] =
	{5.0, 5.0, 5.0, 0.0 };

	static float lmodel_ambient[] =
	{ 1.0f, 1.0f, 1.0f, 1.0f };

	static float lmodel_twoside[] =
	{ GL_TRUE };


	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0, GL_POSITION, position);

	glLightfv(GL_LIGHT1, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT1, GL_POSITION, position);


	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
	glLightModelfv(GL_LIGHT_MODEL_TWO_SIDE, lmodel_twoside);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
//	glEnable(GL_LIGHT1);
	glShadeModel(GL_SMOOTH);
} 

void ofApp::savePicture() {
	ofImage picture;
	picture.grabScreen(0, 0, ofGetWidth(), ofGetHeight());
	picture.save("screenshot.png");
	cout << "picture saved" << endl;
}

//--------------------------------------------------------------
//
// support drag-and-drop of model (.obj) file loading.  when
// model is dropped in viewport, place origin under cursor
//
void ofApp::dragEvent(ofDragInfo dragInfo) {
	
	
}
