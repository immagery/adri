#ifndef OBJECT_H
#define OBJECT_H

#include <render/shadingNode.h>
#include "Node.h"
#include <Eigen\Core>
#include <Eigen\Geometry>

//#include <vcg/complex/complex.h>

#include <Eigen\Dense>

using namespace std;
//using namespace vcg;

// Each element of the scene
// Contains data of the transformation
// and operations to handle transformations.
class object : public node
{

public:

    object();
    object(unsigned int id);
    object(Eigen::Vector3d _pos);

	object(Eigen::Vector3d _pos, Eigen::Quaternion<double> _qrot);
    //object( Eigen::Vector3d _pos,  Eigen::Vector3d _rot);

    // transformation functions
    void resetTransformation();
    virtual void addTranslation(double tx, double ty, double tz);
	virtual void setTranslation(double tx, double ty, double tz);
    virtual void addRotation(double rx, double ry, double rz);

	virtual void setRotation(double rx, double ry, double rz, bool radians = true);
	virtual void addRotation(Eigen::Quaternion<double> q);

    // node info propagation specification
    virtual bool update();
    virtual bool propagateDirtyness();

    void loadIdentity();

    // Selection Functions.
    virtual bool select(bool bToogle, unsigned int id);
    virtual Eigen::Vector3d getSelCenter(){ return pos; }

    virtual bool getBoundingBox(Eigen::Vector3d& minAuxPt,Eigen::Vector3d& maxAuxPt)
	{ 
		minAuxPt = minAuxPt; 
		maxAuxPt = maxAuxPt; 
		return false;
	}

	Eigen::Vector3d pos;
	Eigen::Quaternion<double> qrot;

	Eigen::Matrix4d tMatrix;

    shadingNode* shading;

};

//void getAxisRotationQuaternion(Eigen::Quaterniond& q, int axis, double angle);

#endif // OBJECT_H
