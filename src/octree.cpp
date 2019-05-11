#include "octree.h"

void octree::create(ofxAssimpModelLoader& obj)
{
	ofPoint min = obj.getSceneMin();
	ofPoint max = obj.getSceneMax();
	root.box = Box{ Vector3{min.x,min.y,min.z}, Vector3{max.x,max.y,max.z} };
	vector<ObjectPoint> *points = new vector<ObjectPoint>{};
	for (int i = 0; i < obj.getNumMeshes(); ++i) {
		meshes.push_back(obj.getMesh(i));
		for (int j = 0; j < meshes.back().getVertices().size(); ++j)
			points->push_back({ i,j });
	}
		
	subdivide(root, points, 0);
}

void octree::subdivide(node & n, vector<ObjectPoint>* points, int lvl)
{
	//cout << points->size() << endl;
	if (points->size() == 1 || lvl == maxLvl) {
		n.points = points;
		n.leaf = true;
		//cout << n.points->size() << endl;
	}
	else {
		//cout << n.box.min().x() << ',' << n.box.min().y() << ',' << n.box.min().z() << "  " << n.box.max().x() << ',' << n.box.max().y() << ',' << n.box.max().z() << endl;
		vector<Box> subBox = subDivideBox8(n.box);
		n.children = new vector<node>{};
		n.leaf = false;
		for (int i = 0; i < 8; i++) {
			vector<ObjectPoint> *points_next = new vector<ObjectPoint>{ getPointsInBox(*points, subBox[i]) };
			if (points_next->size() > 0) {
				node tmp;
				tmp.box = subBox[i];
				subdivide(tmp, points_next, lvl + 1);
				n.children->push_back(tmp);
			}
		}
		delete points;
	}
}

vector<Box> octree::subDivideBox8(Box box)
{
	Vector3 min = box.parameters[0];
	Vector3 max = box.parameters[1];
	Vector3 size = max - min;
	Vector3 center = size / 2 + min;
	float xdist = (max.x() - min.x()) / 2;
	float ydist = (max.y() - min.y()) / 2;
	float zdist = (max.z() - min.z()) / 2;
	Vector3 h = Vector3(0, ydist, 0);

	//  generate ground floor
	//
	Box b[8];
	b[0] = Box(min, center);
	b[1] = Box(b[0].min() + Vector3(xdist, 0, 0), b[0].max() + Vector3(xdist, 0, 0));
	b[2] = Box(b[1].min() + Vector3(0, 0, zdist), b[1].max() + Vector3(0, 0, zdist));
	b[3] = Box(b[2].min() + Vector3(-xdist, 0, 0), b[2].max() + Vector3(-xdist, 0, 0));

	vector<Box> boxList;
	for (int i = 0; i < 4; i++)
		boxList.push_back(b[i]);

	// generate second story
	//
	for (int i = 4; i < 8; i++) {
		b[i] = Box(b[i - 4].min() + h, b[i - 4].max() + h);
		boxList.push_back(b[i]);
	}
	return boxList;
}

vector<ObjectPoint> octree::getPointsInBox(vector<ObjectPoint>& p, Box & box)
{
	vector<ObjectPoint> points;
	for (int i = 0; i < p.size(); i++) {
		ofVec3f v = meshes[p[i].meshIndex].getVertex(p[i].verticesIndex);
		if (inside(box, Vector3(v.x, v.y, v.z))) 
			points.push_back(p[i]);
	}
	return points;
}

void octree::draw(node & n, int numLevels, int lvl)
{
	//if (lvl >= numLevels) return;
	
	if (!n.leaf && lvl < numLevels)
		for (int i = 0; i < n.children->size(); i++) 
			draw(n.children->at(i), numLevels, lvl+1);
	else
		drawBox(n.box);
}

bool octree::inside(Box & box, Vector3 v)
{
	return box.min() <= v && v <= box.max();
}

//draw a box from a "Box" class  
//
void octree::drawBox(const Box &box) {
	Vector3 min = box.parameters[0];
	Vector3 max = box.parameters[1];
	Vector3 size = max - min;
	Vector3 center = size / 2 + min;
	ofVec3f p = ofVec3f(center.x(), center.y(), center.z());
	float w = size.x();
	float h = size.y();
	float d = size.z();
	//ofSetColor(0, 0, 255);
	ofDrawBox(p, w, h, d);
	//ofSetColor(255,255, 255);
}

bool octree::doPointSelection(ofCamera& cam, ofVec3f mouse, ofVec3f & rtnPoint)
{
	vector<ObjectPoint> selections;

	glm::vec3 dir = glm::normalize(cam.screenToWorld(glm::vec3{ mouse }) - cam.getPosition());

	Ray r = { Vector3{ cam.getPosition().x,cam.getPosition().y ,cam.getPosition().z }, Vector3{ dir.x,dir.y,dir.z } };

	findPoint(root, r, cam, mouse, selections);
	if (selections.size() > 0) {
		float distance = 0;
		ObjectPoint selectedPoint;
		for (int i = 0; i < selections.size(); i++) {
			ofVec3f point = cam.worldToCamera(meshes[selections[i].meshIndex].getVertex(selections[i].verticesIndex));
			// In camera space, the camera is at (0,0,0), so distance from 
			// the camera is simply the length of the point vector
			//
			float curDist = point.length();
				if (i == 0 || curDist < distance) {
				distance = curDist;
				selectedPoint = selections[i];
			}
		}
		rtnPoint = meshes[selectedPoint.meshIndex].getVertex(selectedPoint.verticesIndex);
		return true;
	}

	return false;
}

void octree::findPoint(const node & treenode, const Ray r, ofCamera& cam, ofVec3f & mouse, vector<ObjectPoint>& selections)
{
	if (treenode.leaf) {
		for (ObjectPoint o : *treenode.points) {
			ofVec3f vert = meshes[o.meshIndex].getVertex(o.verticesIndex);
			ofVec3f posScreen = cam.worldToScreen(vert);
			float distance = posScreen.distance(mouse);
			if (distance < selectionRange)
				selections.push_back(o);
		}
	}
	else {
		for (const node& n : *treenode.children)
			if (n.box.intersect(r, 0.1, 500))
				findPoint(n, r, cam, mouse, selections);
	}
}
