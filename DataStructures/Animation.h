#ifndef ANIMATION_H
#define ANIMATION_H

#include <vector>
#include <map>
#include "Transform.h"
#include <Eigen\Core>

// An animation is a series of pairs keyframe->transform.  To find the values for a desired frame,
// we interpolate the 2 closest keyframes
class Animation
{
public:
    map<int,MyTransform> transforms;

    Animation();
    void addTransform (int frame, int tx, int ty, int tz, int rx, int ry, int rz, int sx, int sy, int sz);

    Eigen::Vector3d getPosition (int frame);
    Eigen::Vector3d getRotation (int frame);
    Eigen::Vector3d getScaling (int frame);

	void saveToFile(string path);
};

#endif // ANIMATION_H
