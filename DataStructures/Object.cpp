#include "object.h"

#include <utils/util.h>

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

void object::addRotation(double rx, double ry, double rz)
{
    // Aplicar la rotación, creo que hay que hacerlo con una multiplicacion.
	Quaternion<double> qAux;
	qAux.FromEulerAngles(Deg2Rad(rx), Deg2Rad(ry), Deg2Rad(rz));
	qAux.Normalize();
	qrot = qAux * qrot;
	qrot.Normalize();
	//rot += Point3d(rx, ry, rz);
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

void object::select(bool bToogle, unsigned int id)
{
    if(shading)
        shading->selected = bToogle;
}

bool object::update()
{
	if(shading)
		return shading->update(this);

	return true;
}
