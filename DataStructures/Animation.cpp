#include "Animation.h"
#include <fstream>

Animation::Animation() {
}

void Animation::addTransform(int frame, int tx, int ty, int tz, int rx, int ry, int rz, int sx, int sy, int sz) {
    MyTransform f;
    f.setPosition(tx, ty, tz);
    f.setRotation(rx,ry,rz);
    f.setScaling(sx, sy, sz);
    transforms[frame] = f;
}

Eigen::Vector3d Animation::getPosition(int frame) {
    MyTransform t1, t2;
    int f1, f2;
    double weight = 1;      //how much % of t1 is present in the interpolation
    for (map<int,MyTransform>::iterator it = transforms.begin(); it != transforms.end(); ++it) {
        if (frame > it->first) {
            f1 = it->first;
            t1 = it->second;
            ++it;
            if (it != transforms.end()) {
                f2 = it->first;
                t2 = it->second;
                weight = (double)(frame - f1) / (f2 - f1);
            }
            break;
        }
    }

    Eigen::Vector3d pos = t1.position*weight + t2.position*(1-weight);
    return pos;
}

Eigen::Vector3d Animation::getRotation(int frame) {
    MyTransform t1, t2;
    int f1 = 0, f2;
    double weight = 1;      //how much % of t1 is present in the interpolation

    for (map<int,MyTransform>::iterator it = transforms.begin(); it != transforms.end(); ++it) {
        // If we are on a keyframe
        if (frame == it->first) {
            f1 = it->first;
            t1 = it->second;
            break;
        }
        // If we need to interpolate..
        if (frame > it->first) {
            f1 = it->first;
            t1 = it->second;
            map<int,MyTransform>::iterator it2 = it; ++it2;
            if (it2 != transforms.end() && frame < it2->first) {
                f2 = it2->first;
                t2 = it2->second;
                weight = 1 - (double)(frame - f1) / (f2 - f1);
                break;
            } else if (it2 == transforms.end()) break;
        }
    }

    Eigen::Vector3d rot = t1.rotation*weight + t2.rotation*(1-weight);
    return rot;
}

Eigen::Vector3d Animation::getScaling(int frame) { return transforms[frame % transforms.size()].scaling; }

void Animation::saveToFile(string path) {
	ofstream file(path.c_str(), ios::out);
	for (map<int,MyTransform>::iterator it = transforms.begin(); it != transforms.end(); ++it) {

	}
}