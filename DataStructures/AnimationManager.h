#ifndef ANIMATIONMANAGER_H
#define ANIMATIONMANAGER_H

#include <map>
#include "Animation.h"
#include "DataStructures/Scene.h"
#include <set>

class AnimationManager
{
public:

    bool animationEnabled;
    map<int, Animation> animations;		// maps from joint ID to animation

    AnimationManager();

    bool objectHasAnimation (int id);
    void addAnimation (int id);
    void addKeyFrame (int id, int frame, int tx, int ty, int tz, int rx, int ry, int rz, int sx, int sy, int sz);
    Point3d getPosition (int id, int frame);
    Point3d getRotation (int id, int frame);
    Point3d getScaling (int id, int frame);

	void saveAnimation(string path, scene* escena);
	void loadAnimations(string path, scene* escena);

	vector<int> getFrames() {
		set<int> frames;
		vector<int> kframes;
		for (map<int, Animation>::iterator it = animations.begin(); it != animations.end(); ++it) {
			for (map<int, Transform>::iterator itt = it->second.transforms.begin(); itt != it->second.transforms.end(); ++itt) {
				frames.insert(itt->first);
			}
		}
		for (set<int>::iterator it = frames.begin(); it != frames.end(); ++it) kframes.push_back(*it);
		return kframes;
	}


};

#endif // ANIMATIONMANAGER_H
