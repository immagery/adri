#include "object.h"

#include <utils/util.h>
#include <Eigen/Dense>

using namespace vcg;

object::object() : node()
{
    pos = Point3d(0,0,0);
    //rot = Point3d(0,0,0);
	qrot = Quaternion<double>(1,0,0,0);

    shading = new shadingNode();

    loadIdentity();

    dirtyFlag = false;
}

object::object(unsigned int id) : node(id)
{
    pos = Point3d(0,0,0);
    //rot = Point3d(0,0,0);
	qrot = Quaternion<double>(1,0,0,0);

    shading = new shadingNode(id+1);

    loadIdentity();

    dirtyFlag = false;
}

object::object(vcg::Point3d _pos) : node()
{
    pos = Point3d(_pos);
    //rot = Point3d(0,0,0);
	qrot = Quaternion<double>(1,0,0,0);

    shading = new shadingNode();

    loadIdentity();

    dirtyFlag = false;
}

/*
object::object(vcg::Point3d _pos, vcg::Point3d _rot) : node()
{
    pos = Point3d(_pos);
    //rot = Point3d(_rot);
	qrot = Quaternion<double>(_qrot);

    shading = new shadingNode();

    loadIdentity();

    dirtyFlag = false;
}
*/

object::object(vcg::Point3d _pos, vcg::Quaternion<double> _qrot) : node()
{
    pos = Point3d(_pos);
    //rot = Point3d(_rot);
	qrot = Quaternion<double>(_qrot);

    shading = new shadingNode();

    loadIdentity();

    dirtyFlag = false;
}

void object::resetTransformation()
{
    pos = Point3d(0,0,0);
    //rot = Point3d(0,0,0);
	qrot = Quaternion<double>(1,0,0,0);
    dirtyFlag = true;
}

void object::addTranslation(double tx, double ty, double tz)
{
    // Aplicar la rotación, creo que hay que hacerlo con una multiplicacion.
    pos += Point3d(tx, ty, tz);
    dirtyFlag = true;
}

void getAxisRotationQuaternion(Eigen::Quaterniond& q, int axis, double angle)
{
	// extract the rest of turns
	double a = angle;
	if(angle >= 0)
		a = angle - (floor(angle/360.0)*360);
	else
		a = angle - (ceil(angle/360.0)*360);

	if(a == 0)
	{
		q = Eigen::Quaterniond().Identity();
		return;
	}

	// This switch changes the case axis
	Eigen::Vector3d v, Axis2, Axis3;
	switch(axis)
	{
	case 2: // over X
		Axis2 << 0,1,0;
		Axis3 << 0,0,1;
	break;
	case 1: // over Y
		Axis2 << 0,0,1;
		Axis3 << 1,0,0;
	break;
	case 0: // over Z
		Axis2 << 1,0,0;
		Axis3 << 0,1,0;
	break;
	}

	v = Axis2*cos(Deg2Rad(a))+Axis3*sin(Deg2Rad(a));

	if(a >= 0)
	{
		if(a == 180)
		{
			q = Eigen::Quaterniond().setFromTwoVectors(Axis2, Axis3) * 
				Eigen::Quaterniond().setFromTwoVectors(Axis3, -Axis2);
		}
		else if(a < 180)
		{
			q = Eigen::Quaterniond().setFromTwoVectors(Axis2, v);
		}
		else if(a > 180)
		{
			q = Eigen::Quaterniond().setFromTwoVectors(Axis2, Axis3) * 
				Eigen::Quaterniond().setFromTwoVectors(Axis3, -Axis2) *
				Eigen::Quaterniond().setFromTwoVectors(-Axis2, v);
		}
	}
	else
	{
		if(a == -180)
		{
			q = Eigen::Quaterniond().setFromTwoVectors(Axis2, -Axis3) * 
				Eigen::Quaterniond().setFromTwoVectors(-Axis3, -Axis2);
		}
		else if(a > -180)
		{
			q = Eigen::Quaterniond().setFromTwoVectors(Axis2, v);
		}
		else if(a > -360)
		{
			q = Eigen::Quaterniond().setFromTwoVectors(Axis2, -Axis3) * 
				Eigen::Quaterniond().setFromTwoVectors(-Axis3, -Axis2) *
				Eigen::Quaterniond().setFromTwoVectors(-Axis2, v);
		}
	}
	
}

void object::addRotation(double rx, double ry, double rz)
{
    // Aplicar la rotación, creo que hay que hacerlo con una multiplicacion.
	
	/*
	Quaternion<double> qAux;
	qAux.FromEulerAngles(Deg2Rad(rx), Deg2Rad(ry), Deg2Rad(rz));
	qAux.Normalize();
	qrot = qAux * qrot;
	qrot.Normalize();
	//rot += Point3d(rx, ry, rz);
    dirtyFlag = true;
	*/
	
	Eigen::Quaterniond qX;
	Eigen::Quaterniond qY;
	Eigen::Quaterniond qZ;

	getAxisRotationQuaternion(qX, 0, rx);
	getAxisRotationQuaternion(qY, 1, ry);
	getAxisRotationQuaternion(qZ, 2, rz);

	Eigen::Quaterniond qrotAux =  qZ * qY * qX;
	Eigen::Quaterniond qrotAux2 = qrotAux* Eigen::Quaterniond(qrot.W(), qrot.X(), qrot.Y(), qrot.Z());
	qrot = vcg::Quaternion<double>(qrotAux2.x(),qrotAux2.y(),qrotAux2.z(),qrotAux2.w());
	dirtyFlag = true;

}

void object::addRotation(Quaternion<double> q) {
	q.Normalize();
	qrot = q * qrot;
	qrot.Normalize();
	dirtyFlag = true;
}

bool object::propagateDirtyness()
{
    dirtyFlag = false;
    return true;
}

void object::loadIdentity()
{
    for(int i = 0; i< 4; i++)
    {
		for(int j = 0; j< 4; j++)
		{
			if(i == j)
				tMatrix[i][j] = 1;
			else
				tMatrix[i][j] = 0;
		}
	}
}

bool object::select(bool bToogle, unsigned int id)
{
    if(shading)
        shading->selected = bToogle;

	return bToogle;
}

bool object::update()
{
	if(shading)
		return shading->update(this);

	return true;
}
