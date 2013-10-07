#include "AnimationManager.h"
#include <fstream>
#include <QtCore/qfile.h>
#include <QtCore/qtextstream.h>
#include <QtCore/qstringlist.h>
#include <QtCore/qregexp.h>


AnimationManager::AnimationManager() {
    animationEnabled = false;
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
	QFile file(path.c_str());
	if (!file.exists()) return;
	file.open(QFile::ReadOnly);
    QTextStream in(&file);
    QString line;

	bool first = true;
	int currentID, keyframe;
	Animation a;

    while (!in.atEnd()) {
        line = in.readLine();
		if (line.toStdString() == "###") {
			if (!first) animations[currentID] = a;
			else first = false;
			line = in.readLine();
			currentID = escena->findIdByName(line.toStdString());
			continue;
		}
        
		QStringList list = line.split(QRegExp(" "));
		keyframe = list.at(0).toInt();
		int tx = list.at(1).toInt();
		int ty = list.at(2).toInt();
		int tz = list.at(3).toInt();
		int rx = list.at(4).toInt();
		int ry = list.at(5).toInt();
		int rz = list.at(6).toInt();
		int sx = list.at(7).toInt();
		int sy = list.at(8).toInt();
		int sz = list.at(9).toInt();
		a.addTransform(keyframe,tx,ty,tz,rx,ry,rz,sx,sy,sz);
		
    }
	animations[currentID] = a;
}
