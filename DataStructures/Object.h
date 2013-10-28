#ifndef OBJECT_H
#define OBJECT_H

#include <vcg/complex/complex.h>
#include <render/shadingNode.h>
#include "Node.h"

using namespace std;
using namespace vcg;

// Each element of the scene
// Contains data of the transformation
// and operations to handle transformations.
class object : public node
{

public:

    object();
    object(unsigned int id);
    object(vcg::Point3d _pos);

	object(vcg::Point3d _pos, vcg::Quaternion<double> _qrot);
    //object(vcg::Point3d _pos, vcg::Point3d _rot);

    // transformation functions
    void resetTransformation();
    virtual void addTranslation(double tx, double ty, double tz);
    virtual void addRotation(double rx, double ry, double rz);
	virtual void addRotation(Quaternion<double> q);

    // node info propagation specification
    virtual bool update();
    virtual bool propagateDirtyness();

    void loadIdentity();

    // Selection Functions.
    virtual bool select(bool bToogle, unsigned int id);
    virtual Point3d getSelCenter(){ return pos; }

    virtual bool getBoundingBox(Point3d& minAuxPt,Point3d& maxAuxPt)
	{ 
		minAuxPt = minAuxPt; 
		maxAuxPt = maxAuxPt; 
		return false;
	}

	Point3d pos;
	vcg::Quaternion<double> qrot;

	Matrix44d tMatrix;

    shadingNode* shading;

};

#endif // OBJECT_H
