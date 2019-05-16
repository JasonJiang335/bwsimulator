#pragma once

#include "ofMain.h"
#include  "ofxAssimpModelLoader.h"
#include "box.h"
#include "ray.h"

struct ObjectPoint {
	int meshIndex;
	int verticesIndex;
};

struct node {
	Box box;
	union {
		vector<ObjectPoint> *points;
		vector<node> *children;
	};
	
	bool leaf = false;
};

class octree
{
public:
	void create(ofxAssimpModelLoader& obj);
	void subdivide(node & n, vector<ObjectPoint>* points, int lvl);
	vector<Box> subDivideBox8(Box box);
	vector<ObjectPoint> getPointsInBox(vector<ObjectPoint>& p, Box& box);

	void draw(node & n, int numLevels, int lvl);
	void draw(int numLevels =20) {
		draw(root, numLevels, 0);
	}

	static void drawBox(const Box &box);
	bool doPointSelection(ofCamera& cam, ofVec3f mouse, ofVec3f * rtnPoint, ofVec3f * rtnNormal);
	bool doPointSelection(Ray r, ofVec3f * rtnPoint, ofVec3f * rtnNormal);
	bool findPoint(const node& box, const Ray r, ofVec3f * rtnPoint, ofVec3f * rtnNormal, float& minDis);
	bool inside(Box& box, Vector3 v);
	node root;
	int maxLvl = 9;
	vector<ofMesh> meshes;

	//const float selectionRange = 10.0;
};

