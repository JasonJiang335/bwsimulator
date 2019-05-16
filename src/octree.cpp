#include "octree.h"
#include "Util.h"

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

bool octree::doPointSelection(ofCamera& cam, ofVec3f mouse, ofVec3f * rtnPoint, ofVec3f * rtnNormal)
{
	
	glm::vec3 dir = glm::normalize(cam.screenToWorld(glm::vec3{ mouse }) - cam.getPosition());

	Ray r = { Vector3{ cam.getPosition().x,cam.getPosition().y ,cam.getPosition().z }, Vector3{ dir.x,dir.y,dir.z } };

	return doPointSelection(r, rtnPoint,rtnNormal);
}

bool octree::doPointSelection(Ray r, ofVec3f * rtnPoint, ofVec3f * rtnNormal)
{
	float minDistance = FLT_MAX;

	return findPoint(root, r, rtnPoint, rtnNormal, minDistance);
}

bool octree::findPoint(const node & treenode, const Ray r, ofVec3f * rtnPoint, ofVec3f * rtnNormal, float& minDistance)
{
	if (treenode.leaf) {
		ObjectPoint minVert = treenode.points->at(0);
		float minDis = glm::distance(glm::vec3{ meshes[minVert.meshIndex].getVertex(minVert.verticesIndex) }, glm::vec3{r.origin.x(), r.origin.y(), r.origin.z() });
		for (ObjectPoint& o : *treenode.points) {
			
			float tmpd = glm::distance(glm::vec3{ meshes[o.meshIndex].getVertex(o.verticesIndex) }, glm::vec3{ r.origin.x(), r.origin.y(), r.origin.z() });

			if (tmpd < minDis) {
				minVert = o;
				minDis = tmpd;
			}
		}

		ofVec3f point;
		ofVec3f raypoint{ r.origin.x(), r.origin.y() ,r.origin.z() };
		ofVec3f raydir{ r.direction.x(), r.direction.y() ,r.direction.z() };

		if (rayIntersectPlane(raypoint, raydir, meshes[minVert.meshIndex].getVertex(minVert.verticesIndex), meshes[minVert.meshIndex].getNormal(minVert.verticesIndex), point)) {
			float d = point.distance(ofVec3f{ r.origin.x(), r.origin.y(), r.origin.z() });
			if (d < minDistance) {
				if (rtnPoint) *rtnPoint = point;
				if (rtnNormal) *rtnNormal = meshes[minVert.meshIndex].getNormal(minVert.verticesIndex);
				minDistance = d;
			}
		}
		
		
	}
	else {
		for (const node& n : *treenode.children)
			if (n.box.intersect(r, 0.1, 500))
				findPoint(n, r, rtnPoint, rtnNormal, minDistance);
	}

	return minDistance < FLT_MAX;

}
