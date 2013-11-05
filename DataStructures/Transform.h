#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <Eigen\Core>

using namespace std;

// Class Transform stores an absolute translation, rotation and scaling. Animations are simply
// arrays of Transforms, one for each frame.
class MyTransform
{
public:

    // Attributes
    Eigen::Vector3d position;
    Eigen::Vector3d rotation;
    Eigen::Vector3d scaling;


    MyTransform() {
		position = Eigen::Vector3d(0,0,0);
		rotation = Eigen::Vector3d(0,0,0);
		scaling = Eigen::Vector3d(1,1,1);
	}

    void setPosition(int tx, int ty, int tz) { position = Eigen::Vector3d(tx,ty,tz); }
    void setRotation(int rx, int ry, int rz) { rotation = Eigen::Vector3d(rx,ry,rz); }
    void setScaling(int sx, int sy, int sz)  { scaling = Eigen::Vector3d(sx,sy,sz); }

};

#endif // TRANSFORM_H
