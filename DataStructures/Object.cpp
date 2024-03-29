#include "object.h"

#include <utils/util.h>
#include <Eigen/Dense>
#include <Eigen/Geometry>
#include <Eigen/Core>

using namespace Eigen;

object::object() : node()
{
    pos = Eigen::Vector3d(0,0,0);
    //rot = Eigen::Vector3d(0,0,0);
	qrot = Eigen::Quaternion<double>(1,0,0,0);

    shading = new shadingNode();

    loadIdentity();

    dirtyFlag = false;
}

object::object(unsigned int id) : node(id)
{
    pos = Eigen::Vector3d(0,0,0);
    //rot = Eigen::Vector3d(0,0,0);
	qrot = Eigen::Quaternion<double>(1,0,0,0);

    shading = new shadingNode(id+1);

    loadIdentity();

    dirtyFlag = false;
}

object::object(Eigen::Vector3d _pos) : node()
{
    pos = Eigen::Vector3d(_pos);
    //rot = Eigen::Vector3d(0,0,0);
	qrot = Eigen::Quaternion<double>(1,0,0,0);

    shading = new shadingNode();

    loadIdentity();

    dirtyFlag = false;
}

/*
object::object( Eigen::Vector3d _pos,  Eigen::Vector3d _rot) : node()
{
    pos = Eigen::Vector3d(_pos);
    //rot = Eigen::Vector3d(_rot);
	qrot = Eigen::Quaternion<double>(_qrot);

    shading = new shadingNode();

    loadIdentity();

    dirtyFlag = false;
}
*/

object::object(Eigen::Vector3d _pos, Eigen::Quaternion<double> _qrot) : node()
{
    pos = Eigen::Vector3d(_pos);
    //rot = Eigen::Vector3d(_rot);
	qrot = Eigen::Quaternion<double>(_qrot);

    shading = new shadingNode();

    loadIdentity();

    dirtyFlag = false;
}

void object::resetTransformation()
{
    pos = Eigen::Vector3d(0,0,0);
    //rot = Eigen::Vector3d(0,0,0);
	qrot = Eigen::Quaternion<double>(1,0,0,0);
    dirtyFlag = true;
}

void object::setTranslation(double tx, double ty, double tz, bool local)
{
    // Aplicar la rotaci�n, creo que hay que hacerlo con una multiplicacion.
    pos = Vector3d(tx, ty, tz);
    dirtyFlag = true;
}

void object::addTranslation(double tx, double ty, double tz)
{
    // Aplicar la rotaci�n, creo que hay que hacerlo con una multiplicacion.
    pos += Eigen::Vector3d(tx, ty, tz);
    dirtyFlag = true;
}

/*
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
	case 0: // over X
		Axis2 << 0,1,0;
		Axis3 << 0,0,1;
	break;
	case 1: // over Y
		Axis2 << 0,0,1;
		Axis3 << 1,0,0;
	break;
	case 2: // over Z
		Axis2 << 1,0,0;
		Axis3 << 0,1,0;
	break;
	}

	v = Axis2*cos(Deg2Rad(a))+Axis3*sin(Deg2Rad(a));

	if(a >= 0)
	{
		if(a == 180)
		{
			q = Eigen::Quaterniond().setFromTwoVectors(Axis3, -Axis2) * Eigen::Quaterniond().setFromTwoVectors(Axis2, Axis3);
		}
		else if(a < 180)
		{
			q = Eigen::Quaterniond().setFromTwoVectors(Axis2, v);
		}
		else if(a > 180)
		{
			q = Eigen::Quaterniond().setFromTwoVectors(-Axis2, v) * 
				Eigen::Quaterniond().setFromTwoVectors(Axis3, -Axis2) * 
				Eigen::Quaterniond().setFromTwoVectors(Axis2, Axis3);
		}
	}
	else
	{
		if(a == -180)
		{
			q = Eigen::Quaterniond().setFromTwoVectors(-Axis3, -Axis2) *
				Eigen::Quaterniond().setFromTwoVectors(Axis2, -Axis3);
		}
		else if(a > -180)
		{
			q = Eigen::Quaterniond().setFromTwoVectors(Axis2, v);
		}
		else if(a > -360)
		{
			q = Eigen::Quaterniond().setFromTwoVectors(-Axis2, v) *
				Eigen::Quaterniond().setFromTwoVectors(-Axis3, -Axis2) *
				Eigen::Quaterniond().setFromTwoVectors(Axis2, -Axis3);
		}
	}
	
}
*/

void object::setRotation(double rx, double ry, double rz, bool radians)
{
	Eigen::Quaterniond q[3];
	double angles[3];

	angles[0] = rx;
	angles[1] = ry;
	angles[2] = rz;

	if(radians)
	{
		// Convert to degrees
		for(int i = 0; i< 3; i++)
			angles[i] = angles[i]*360/(M_PI*2);
	}

	// Rotation over each axis
	for(int i = 0; i< 3; i++)
		getAxisRotationQuaternion(q[i], i, angles[i]);

	// Concatenate all the values in X-Y-Z order
	Eigen::Quaterniond qrotAux =  q[2] * q[1] * q[0];

	// TOFIX: remove eigen rotations
	qrot = qrotAux; // Quaternion<double>(qrotAux.w(), qrotAux.x(),qrotAux.y(),qrotAux.z());

	int test = 0;
}

void object::addRotation(double rx, double ry, double rz)
{
    // Aplicar la rotaci�n, creo que hay que hacerlo con una multiplicacion.
	
	/*
	Eigen::Quaternion<double> qAux;
	qAux.FromEulerAngles(Deg2Rad(rx), Deg2Rad(ry), Deg2Rad(rz));
	qAux.Normalize();
	qrot = qAux * qrot;
	qrot.Normalize();
	//rot += Eigen::Vector3d(rx, ry, rz);
    dirtyFlag = true;
	*/
	
	Eigen::Quaterniond qX;
	Eigen::Quaterniond qY;
	Eigen::Quaterniond qZ;

	getAxisRotationQuaternion(qX, 0, rx);
	getAxisRotationQuaternion(qY, 1, ry);
	getAxisRotationQuaternion(qZ, 2, rz);

	Eigen::Quaterniond qrotAux =  qZ * qY * qX;
	Eigen::Quaterniond qrotAux2 = qrotAux * qrot;
	qrot = qrotAux2;

	dirtyFlag = true;

}

void object::addRotation(Eigen::Quaternion<double> q, bool local) 
{
	if(local)
	{
		q.normalize();
		qrot = qrot * q;
		qrot.normalize();
		dirtyFlag = true;
	}
	else
	{
		// TODO
		assert(false);
	}
}


void object::setRotation(Eigen::Quaternion<double> q, bool local) 
{
	if(local)
	{
		q.normalize();
		qrot = q;
		qrot.normalize();
		dirtyFlag = true;
	}
	else
	{
		// TODO
		q.normalize();
		qrot = q;
		qrot.normalize();
		dirtyFlag = true;
	}
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
				tMatrix(i,j) = 1;
			else
				tMatrix(i,j) = 0;
		}
	}
}

bool object::select(bool bToogle, unsigned int id)
{
	id = id; // for delete warning

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
