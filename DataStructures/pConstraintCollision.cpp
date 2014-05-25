#include "PConstraintCollision.h"

#include <utils/utilGL.h>

void pConstraintCollision::solve(vector< Vector3d >& estimation)
{
	/*
    if(elems.size() != 2) return;

    int idx1 = elems[0]->nodeId;
    int idx2 = elems[1]->nodeId;

    float weightSum = elems[0]->weight + elems[1]->weight;
    float norm = (estimation[idx1]-estimation[idx2]).norm();
    float sw = ( norm - distance)/(weightSum);
    Vector3d incr = sw*(estimation[idx1]-estimation[idx2])/norm;

    // dos fuerzas en la misma direccion
    estimation[idx1] -= elems[0]->weight*incr;
    estimation[idx2] += elems[1]->weight*incr;
	*/
}

void pConstraintCollision::init(Particle* p1, Particle* p2)
{
	/*
    elems.resize(2);
    elems[0] = p1;
    elems[1] = p2;

    distance = (p1->position-p2->position).norm();
	*/
}

void pConstraintCollision::drawFunc()
{
	/*
    if(elems.size() == 0) return;

    glColor3f(0.0, 1.0, 0);
	glBegin(GL_LINES);
		glVertex3d(elems[0]->position.x(), elems[0]->position.y(), elems[0]->position.z());
        glVertex3d(elems[1]->position.x(), elems[1]->position.y(), elems[1]->position.z());
	glEnd();
	*/
}