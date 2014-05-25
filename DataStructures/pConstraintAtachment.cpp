#include "PConstraintAtachment.h"

#include <utils/utilGL.h>

#define ERROR_EPSILON 0.0000001

void pConstraintAtachment::solve(vector< Vector3d >& estimation, int nIterations)
{
	float weightAttachment = 0; 

    if(elems.size() != 2) return;

    int idx1 = elems[0]->nodeId;

    float weightSum = elems[0]->weight + 0;
    
	if(fabs(weightSum) < ERROR_EPSILON) return;
	
	float norm = (estimation[idx1]-atachmentPoint).norm();
    float sw = ( norm )/(weightSum);

    Vector3d incr = sw*(estimation[idx1]-atachmentPoint)/norm;

	//float RealStiffness = 1-pow((1-stiffness),1.0/nIterations);
	//incr *= stiffness;

    // dos fuerzas en la misma direccion
    estimation[idx1] -= elems[0]->weight*incr;
}

void pConstraintAtachment::init(Particle* p1, Vector3d atachment)
{
    elems.resize(1);
    elems[0] = p1;
    atachmentPoint = atachment;
}

void pConstraintAtachment::drawFunc()
{
    if(elems.size() == 0) return;

	glColor3f(0.0, 0.7, 0.7);
	glPointSize(30);
	glBegin(GL_POINTS);
		glVertex3d(atachmentPoint.x(), atachmentPoint.y(), atachmentPoint.z());
	glEnd();
}