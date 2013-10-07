#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <vcg/complex/complex.h>

using namespace std;
using namespace vcg;

// Class Transform stores an absolute translation, rotation and scaling. Animations are simply
// arrays of Transforms, one for each frame.
class Transform
{
public:

    // Attributes
    Point3d position;
    Point3d rotation;
    Point3d scaling;


    Transform();
    void setPosition(int tx, int ty, int tz) { position = Point3d(tx,ty,tz); }
    void setRotation(int rx, int ry, int rz) { rotation = Point3d(rx,ry,rz); }
    void setScaling(int sx, int sy, int sz)  { scaling = Point3d(sx,sy,sz); }

};

#endif // TRANSFORM_H
