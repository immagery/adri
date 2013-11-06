#include "ShadingNode.h"

#include <utils/utilGL.h>
#include <DataStructures/Object.h>

bool shadingNode::update(object* obj)
{
	if(!obj) return false;

    if(!obj->dirtyFlag)
        return true;
    else
    {
        /*
		glMatrixMode(GL_MODELVIEW_MATRIX);
        glPushMatrix();
        glLoadIdentity();
        glTranslated(obj->pos.X(), obj->pos.Y(), obj->pos.Z());

		glRotated(obj->rot.Z(), 0, 0, 1);
        glRotated(obj->rot.Y(), 0, 1, 0);
        glRotated(obj->rot.X(), 1, 0, 0);

		glGetDoublev(GL_MODELVIEW_MATRIX, obj->tMatrix);
        glPopMatrix();
        */
		
		Eigen::Matrix3d rot = obj->qrot.toRotationMatrix();
		obj->tMatrix << rot(0,0), rot(0,1), rot(0,2), obj->pos.x(),
						rot(1,0), rot(1,1), rot(1,2), obj->pos.y(),
						rot(2,0), rot(2,1), rot(2,2), obj->pos.z(),
						0		, 0		  , 0		, 1;

		obj->dirtyFlag = false;
    }

    return true;
}

void shadingNode::beforeDraw(object* obj)
{
    if(dirtyFlag) 
		update(obj);


    glPushMatrix();
	GLdouble mat[16];

	for(int i = 0; i< 4; i++)
	{
		for(int j = 0; j< 4; j++)
		{
			mat[i*4+j] = obj->tMatrix(i,j);
		}
	}

    glMultMatrixd(mat);
}

void shadingNode::afterDraw(object* obj)
{
    glPopMatrix();
}