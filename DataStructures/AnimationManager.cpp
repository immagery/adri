#include "AnimationManager.h"
#include <fstream>
#include <sstream>


AnimationManager::AnimationManager() {
    animationEnabled = false;
	simulationEnabled = false;
}

bool AnimationManager::objectHasAnimation (int id) {
    return animations.find(id) != animations.end();
}

void AnimationManager::addAnimation (int id) {
    animations.insert(pair<int, Animation> (id, Animation()));
    animations[id].addTransform(0,0,0,0,0,0,0,0,0,0);
}

void AnimationManager::addKeyFrame (int id, int frame, int tx, int ty, int tz,
                                 int rx, int ry, int rz, int sx,
                                 int sy, int sz) {
    animations[id].addTransform(frame,tx,ty,tz,rx,ry,rz,sx,sy,sz);
}

Point3d AnimationManager::getPosition (int id, int frame) {
    return animations[id].getPosition(frame);
}

Point3d AnimationManager::getRotation (int id, int frame) {
    return animations[id].getRotation(frame);
}

Point3d AnimationManager::getScaling (int id, int frame) {
    return animations[id].getScaling(frame);
}

void AnimationManager::saveAnimation(string path, scene* escena) {
	ofstream file(path.c_str(), ios::out);
	for (map<int,Animation>::iterator it = animations.begin(); it != animations.end(); ++it) {
		file << "###" << endl;
		file << escena->findNameById(it->first) << endl;
		for (map<int,Transform>::iterator itt = it->second.transforms.begin(); itt != it->second.transforms.end(); ++itt) {
			int keyframe = itt->first;
			Transform t = itt->second;
			file << keyframe << " " << t.position.X() << " " << t.position.Y() << " " << t.position.Z();
			file << " " << t.rotation.X() << " " << t.rotation.Y() << " " << t.rotation.Z();
			file << " " << t.scaling.X() << " " << t.scaling.Y() << " " << t.scaling.Z() << endl;
		}
	}
	file.close();
}

void AnimationManager::loadAnimations(string path, scene* escena){
	ifstream file(path.c_str(), ios::in);
	if (!file.is_open()) return;
	string line;

	bool first = true;
	int currentID, keyframe;
	Animation a;

	while (file.good()) {
        getline(file,line);
		if (line == "###") {
			if (!first) animations[currentID] = a;
			else first = false;
			getline(file,line);
			currentID = escena->findIdByName(line);
			continue;
		}
        
		stringstream ss(line);
		int tx, ty, tz, rx, ry, rz, sx, sy, sz;
		ss >> tx >> ty >> tz >> rx >> ry >> rz >> sx >> sy >> sz;
		a.addTransform(keyframe,tx,ty,tz,rx,ry,rz,sx,sy,sz);
		
    }
	animations[currentID] = a;
}
