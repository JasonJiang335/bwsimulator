
//--------------------------------------------------------------
//
//  Kevin M. Smith 
//
//  wholeTrack HiRise Project - startup scene
// 
//  This is an openFrameworks 3D scene that includes an EasyCam
//  and example 3D geometry which I have reconstructed from wholeTrack
//  HiRis photographs taken the wholeTrack Reconnaisance Orbiter
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
	bTerrainSelected = true;
//	ofSetWindowShape(1024, 768);
	
	cam.setNearClip(.1);
    cam.setFarClip(100000.0);
	cam.setFov(65.5);   // approx equivalent to 28mm in 35mm format
	cam.setPosition(0.f, 20.f, 0.f);
	cam.lookAt(glm::vec3{ 0.f,0.f,0.f });

	pathCam.setNearClip(.1);
	pathCam.setFov(65.5);   // approx equivalent to 28mm in 35mm format

	ofSetVerticalSync(true);
	ofEnableSmoothing();
	ofEnableDepthTest();

	// setup rudimentary lighting 
	//
	initLightingAndMaterials();

	wholeTrack.loadModel("geo/simtraxx_bw.obj");
	wholeTrack.setScaleNormalization(false);
	wholeTrack.disableTextures();
    car.loadModel("geo/lander.obj");
    car.setScaleNormalization(false);
    car.disableTextures();

	boundingBox = meshBounds(wholeTrack.getMesh(0));

	ot.create(wholeTrack);

	currentCam = &cam;
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

				carAngle = glm::orientedAngle(glm::normalize(glm::vec2(trails[playMode_i+1].x - trails[playMode_i].x, trails[playMode_i+1].z - trails[playMode_i].z)), carInitOrientation) / 3.1415926 * 180;
				//cout << carAngle << endl;
				
				;
			}
			else {
				bPlayMode = false;
				currentCam = &cam;
				return;
			}
		}

		float timeElasted = ofGetElapsedTimef();
		ofResetElapsedTimeCounter();
		
		if (car.getRotationAngle(0) != carAngle) {
			float diff = car.getRotationAngle(0) - carAngle;
			float maxTurning = carMaxOmega * timeElasted;
			//cout << diff << endl;
			if (abs(diff) <= maxTurning || (360.0f - abs(diff)) <= maxTurning) {
				car.setRotation(0, carAngle, 0, 1, 0);
			}
			else if ((0 < diff && diff < 180) || (-360 < diff && diff < -180)) {
				car.setRotation(0, car.getRotationAngle(0) - maxTurning, 0, 1, 0);
			}
			else {
				car.setRotation(0, car.getRotationAngle(0) + maxTurning, 0, 1, 0);
			}
		}
		

		//car.setRotation(0, carAngle, 0, 1, 0);


		playMode_t += timeElasted / trails[playMode_i].distance(trails[playMode_i + 1]) * speed[playMode_i] * globalSpeedScaler;

		glm::vec3 pos = trails[playMode_i] * (1 - playMode_t) + trails[playMode_i + 1] * playMode_t;
		pathCam.setPosition(pos + pathCamOffset);
		car.setPosition(pos.x,pos.y,pos.z);

		if (playMode_i != trails.size() - 2) target = trails[playMode_i + 1] * (1 - playMode_t) + trails[playMode_i + 2] * playMode_t + pathCamOffset;
		pathCam.lookAt(target);


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
		wholeTrack.drawWireframe();
        
        
	}
	else {
		ofEnableLighting();              // shaded mode
		wholeTrack.drawFaces();

        if(trails.size() > 1){
            car.drawFaces();
        }
        
	}


	if (bDisplayPoints) {                // display points as an option    
		glPointSize(3);
		ofSetColor(ofColor::green);
		wholeTrack.drawVertices();
	}

	ofNoFill();
	ofSetColor(ofColor::white);
	

	//if (!bPlayMode)
	ofSetLineWidth(5);
	for (int i = 0; i < trails.size();++i) {
		if (i==selectedPoint) ofSetColor(ofColor::yellowGreen);
		else ofSetColor(ofColor::red);
		ofDrawSphere(trails[i], 3);
		ofSetColor(ofColor::red);
        /*
		switch (speed[i]) {
            case 5:
                ofSetColor(255, 0, 0);
                break;
            case 10:
                ofSetColor(255, 255, 0);
                break;
            case 25:
                ofSetColor(0, 255, 0);
                break;
            default:
                break;
        }
		*/
		ofSetColor(256 - speed[i] ,255 , 256- speed[i]);
        
		if (i<trails.size()-1) ofDrawLine(trails[i] + ofVec3f{0, 5, 0}, trails[i+1] + ofVec3f{0, 5, 0});
        ofSetLineWidth(1);
        ofSetColor(255, 255, 255);
	}

	// highlight selected point (draw sphere around selected point)
	//
	if (!bPointMoving && bPointSelectionMode && bPointHovered) {
		ofSetColor(ofColor::blue);
		ofDrawSphere(intersectPoint, 3);
	}

	
	if (bDrawOctree) {
		ofSetColor(0, 0, 255);
		ot.draw(20);
	}

	
	if (bPlayMode) {
		ofSetColor(ofColor::green);
		pathCam.draw();
	}

	//if (bPlayMode) ofDrawSphere(testPoint, 1);
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

	cout << key << endl;
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
		//cam.reset();
		break;
	case 19: //CRTL s
		savePath();
		break;
	case 12: //CTRL l
		loadPath();
		if (trails.size() > 1) {
			car.setPosition(trails[0].x, trails[0].y, trails[0].z);
			carAngle = glm::orientedAngle(glm::normalize(glm::vec2(trails[1].x - trails[0].x, trails[1].z - trails[0].z)), carInitOrientation) / 3.1415926 * 180;
			car.setRotation(0, carAngle, 0, 1, 0);
		}
		break;
	case 't':
		toggleWireframeMode();
		break;
	case 'u':
		break;
	case 'v':
		togglePointsDisplay();
		break;
	case 'V':
		break;
	case 'w':
	case 'W':
		cam.setPosition(cam.getPosition() - 5 * glm::vec3{ glm::sin(camLatitude),0,glm::cos(camLatitude) });
		break;
	case 's':
	case 'S':
		cam.setPosition(cam.getPosition() + 5 * glm::vec3{ glm::sin(camLatitude),0,glm::cos(camLatitude) });
		break;
	case 'a':
	case 'A':
	case OF_KEY_LEFT:
		cam.setPosition(cam.getPosition() - 5 * cam.getSideDir());
		break;
	case 'd':
	case 'D':
	case OF_KEY_RIGHT:
		cam.setPosition(cam.getPosition() + 5 * cam.getSideDir());
		break;
	case OF_KEY_UP:
		cam.setPosition(cam.getPosition() + 5 * cam.getLookAtDir());
		break;
	case OF_KEY_DOWN:
		cam.setPosition(cam.getPosition() + -5 * cam.getLookAtDir());
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
				
				currentCam = &pathCam;

				currentCam->lookAt(trails[1] + pathCamOffset);
				carAngle = glm::orientedAngle(glm::normalize(glm::vec2(trails[1].x - trails[0].x, trails[1].z - trails[0].z)), carInitOrientation) / 3.1415926 * 180;
				car.setRotation(0, carAngle, 0, 1, 0);

				playMode_t = 0.0f;
				playMode_i = 0;
				
				ofResetElapsedTimeCounter();
			}
		}
		else {
			bPlayMode = false;
			currentCam = &cam;
		}
		break;
	case ' ':
		cam.setPosition(cam.getPosition() + glm::vec3{ 0.0f, 5.0f,0.0f });
		break;
	case '+':
	case '=':
		if (bPlayMode) globalSpeedScaler *= 2.f;
		else if (selectedPoint > 0 && speed[selectedPoint-1] < 256)
			speed[selectedPoint-1] +=16;
		break;
	case '-':
	case '_':
		if (bPlayMode) globalSpeedScaler *= .5f;
        else if(selectedPoint > 0 && speed[selectedPoint-1] >16)
            speed[selectedPoint-1] -=16;
		break;
    
	case OF_KEY_ALT:
		//cam.enableMouseInput();
		bAltKeyDown = true;
		break;
	case OF_KEY_CONTROL:
		bCtrlKeyDown = true;
		break;
	case OF_KEY_SHIFT:
		cam.setPosition(cam.getPosition() + glm::vec3{ 0.0f, -5.0f,0.0f });
		break;
	case OF_KEY_DEL:
		if (selectedPoint != -1) {
			trails.erase(trails.begin() + selectedPoint);
			speed.erase(speed.begin() + selectedPoint);
			if (selectedPoint==1 && trails.size() > 1) {
				carAngle = glm::orientedAngle(glm::normalize(glm::vec2(trails[1].x - trails[0].x, trails[1].z - trails[0].z)), carInitOrientation) / 3.1415926 * 180;
				car.setRotation(0, carAngle, 0, 1, 0);
			}
			if (selectedPoint == 0 && trails.size() > 0) {
				car.setPosition(trails[0].x, trails[0].y, trails[0].z);
			}
			selectedPoint = -1;
		}
		else {
			trails.clear();
			speed.clear();
		}
		break;
        case OF_KEY_F1:
            if (bPlayMode) {
                if (currentCam == &cam) currentCam = &pathCam;
                else currentCam = &cam;
            }
        break;
		
    default:
		break;
	}
}

void ofApp::toggleWireframeMode() {
	bWireframe = !bWireframe;
}

void ofApp::toggleSelectTerrain() {
	bTerrainSelected = !bTerrainSelected;
}

void ofApp::togglePointsDisplay() {
	bDisplayPoints = !bDisplayPoints;
}

void ofApp::keyReleased(int key) {

	switch (key) {
	
	case OF_KEY_ALT:
		//cam.disableMouseInput();
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
		bPointHovered = ot.doPointSelection(cam, ofVec3f(x, y, 0), &intersectPoint, NULL);
	}
}


//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {
	

	if (bPointSelectionMode) {
		if (button == 2) selectedPoint = -1;
		else if (!button && bPointHovered) {
			int tmp = -1;
			for (int i = 0; i < trails.size(); ++i)
				if (glm::distance(currentCam->worldToScreen(trails[i]), currentCam->worldToScreen(intersectPoint)) < 30) {
					tmp = i;
					//cout << currentCam->worldToScreen(trails[i])<<' '<< currentCam->worldToScreen(intersectPoint)<<' '<<glm::distance(currentCam->worldToScreen(trails[i]), currentCam->worldToScreen(intersectPoint)) << endl;
				}
			if (tmp != -1) {
				selectedPoint = tmp;
				bPointMoving = true;
				//bPointSelectionMode=false;
			}
			else {
				trails.push_back(intersectPoint);
                speed.push_back(128);
				selectedPoint = trails.size() - 1;
				//bPointSelectionMode = false;
				bPointMoving = true;
				
			}
			if (selectedPoint == 0) {
				car.setPosition(trails[0].x, trails[0].y, trails[0].z);
			}
			if (selectedPoint == 1) {
				carAngle = glm::orientedAngle(glm::normalize(glm::vec2(trails[1].x - trails[0].x, trails[1].z - trails[0].z)), carInitOrientation) / 3.1415926 * 180;
				car.setRotation(0, carAngle, 0, 1, 0);
			}
		}
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

void ofApp::updateRoverBox()
{
	roverBox.parameters[0] = vecAdd(roverOriMin, car.getPosition());
	roverBox.parameters[1] = vecAdd(roverOriMax, car.getPosition());
}



//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {
	
	if (bPointMoving) {
		ot.doPointSelection(cam, ofVec3f(x, y, 0), &trails[selectedPoint], NULL);
		if (selectedPoint == 0) {
			car.setPosition(trails[0].x, trails[0].y, trails[0].z);
		}
		if (selectedPoint == 1) {
			carAngle = glm::orientedAngle(glm::normalize(glm::vec2(trails[1].x - trails[0].x, trails[1].z - trails[0].z)), carInitOrientation) / 3.1415926 * 180;
			car.setRotation(0, carAngle, 0, 1, 0);
		}
	}
	if (!bPointSelectionMode) {
		camLatitude -= (x - pmouseX) / 500.f;
		camLongitude -= (y - pmouseY) / 500.f;
		if (camLongitude > glm::half_pi<float>())  camLongitude = glm::half_pi<float>();
		if (camLongitude < -glm::half_pi<float>())  camLongitude = -glm::half_pi<float>();
		glm::quat q = glm::angleAxis(0.f, glm::vec3(0, 0, 1)) * glm::angleAxis(camLatitude, glm::vec3(0, 1, 0)) * glm::angleAxis(camLongitude, glm::vec3(1, 0, 0));
		cam.setOrientation(q);
	}
	pmouseX = x;
	pmouseY = y;
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {
	bRoverDraging = false;
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
bool ofApp::doPointSelection() {

	ofMesh mesh = wholeTrack.getMesh(0);
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

void ofApp::savePath()
{
    nlohmann::json j;
    j["points"] = nlohmann::json::array();
    for (int i = 0; i < trails.size();++i) {
        nlohmann::json p;
        p["x"] = trails[i].x;
        p["y"] = trails[i].y;
        p["z"] = trails[i].z;
        p["s"] = speed[i];
        j["points"].push_back(p);
    }
    
    std::ofstream o("data/save.json");
    o << std::setw(4) << j << std::endl;
}

void ofApp::loadPath()
{
    ifstream i("data/save.json");
    nlohmann::json j;//="{\"layers\":[{\"height\":444.0,\"path\":\"C:\\\\Users\\\\Forrest-m\\\\Desktop\\\\rockman.png\",\"width\":466.0,\"x\":515.0,\"y\":312.0}],\"selected\":-1}"_json;
    i >> j;
    //cout << j.dump() << endl;;
    
    trails.clear();
    speed.clear();
    for (auto& t : j["points"]) {
        trails.emplace_back(t["x"], t["y"], t["z"]);
        speed.push_back(t["s"]);
    }
	
}
